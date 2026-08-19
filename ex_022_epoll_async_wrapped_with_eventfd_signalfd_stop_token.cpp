#include <iostream>
#include <iomanip>
#include <coroutine>
#include <syncstream>
#include <unordered_map>
#include <stop_token>

#include <unistd.h>
#include <sys/epoll.h>
#include <sys/eventfd.h>
#include <sys/signalfd.h>
#include <signal.h>
#include <fcntl.h>
#include <cstring>
#include <errno.h>

std::string_view trim(std::string_view str) {
  const char whitespace[] = " \t\r\n\f\v";
  const size_t start = str.find_first_not_of(whitespace);
  if (start == std::string_view::npos) return "";

  const size_t end = str.find_last_not_of(whitespace);
  return std::string_view(str.begin() + start, str.begin() + end + 1);
}

void throw_sys_error() {
  char errbuff[128];
  const char* errstr = strerror_r(errno, errbuff, sizeof(errbuff));
  throw std::runtime_error(errstr);
}

struct Task {
  struct promise_type {
    Task get_return_object() {
      return Task{std::coroutine_handle<promise_type>::from_promise(*this)};
    }
    std::suspend_always initial_suspend() const noexcept { return {}; }
    std::suspend_always final_suspend() const noexcept { return {}; }
    void return_void() const noexcept {}
    void unhandled_exception() { std::terminate(); }
  };

  std::coroutine_handle<> handle;
  Task(std::coroutine_handle<> h) : handle(h) {}
  ~Task() { if (handle) handle.destroy(); } 
};

struct UniqueFd {
  int fd = -1;
  ~UniqueFd() {
    close(fd);
  }
};

class EventLoop {
  UniqueFd epoll_fd;
  UniqueFd stop_fd;
  UniqueFd sig_fd;
  std::unordered_map<int, std::coroutine_handle<>> suspended_coros;

public:
  EventLoop() {
    epoll_fd.fd = epoll_create1(EPOLL_CLOEXEC);
    if (epoll_fd.fd < 0) {
      throw_sys_error();
    }

    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGINT);
    sigaddset(&mask, SIGTERM);
    sigaddset(&mask, SIGHUP);

    if (sigprocmask(SIG_BLOCK, &mask, nullptr) < 0) {
      throw_sys_error();
    }

    sig_fd.fd = signalfd(-1, &mask, SFD_NONBLOCK | SFD_CLOEXEC);
    if (sig_fd.fd < 0) {
      throw_sys_error();
    }

    stop_fd.fd = eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
    if (stop_fd.fd < 0) {
      throw_sys_error();
    }

    for (const auto fd : {stop_fd.fd, sig_fd.fd}) {
      epoll_event ev{};
      ev.events = EPOLLIN;
      ev.data.fd = fd;
      if (epoll_ctl(epoll_fd.fd, EPOLL_CTL_ADD, fd, &ev) < 0) {
        throw_sys_error();
      }
    }
  }
  ~EventLoop() {
  }

  void wake_up() {
    uint64_t u = 1;
    [[maybe_unused]] ssize_t s = write(stop_fd.fd, &u, sizeof(uint64_t));
  }

  void register_event(int fd, uint32_t events, std::coroutine_handle<> h) {
    epoll_event ev{};
    ev.events = events | EPOLLONESHOT;
    ev.data.fd = fd;

    suspended_coros[fd] = h;

    if (epoll_ctl(epoll_fd.fd, EPOLL_CTL_MOD, fd, &ev) < 0) {
      if (epoll_ctl(epoll_fd.fd, EPOLL_CTL_ADD, fd, &ev) < 0) {
        throw_sys_error();
      }
    }
  }

  void run_once(std::stop_source& source) {
    epoll_event evs[10];

    int nfds = epoll_wait(epoll_fd.fd, evs, 10, -1);
    if (nfds < 0) {
      if (errno == EINTR) return; // signal interrupted; okay
      throw_sys_error();
    }
    for (int i = 0; i < nfds; ++i) {
      const auto fd = evs[i].data.fd;
      if (fd == stop_fd.fd) {
        uint64_t u;
        [[maybe_unused]] ssize_t s = read(stop_fd.fd, &u, sizeof(uint64_t));
        continue;
      }
      if (fd == sig_fd.fd) {
        signalfd_siginfo fdsi;
        ssize_t s = read(sig_fd.fd, &fdsi, sizeof(fdsi));
        if (s == sizeof(fdsi)) {
          std::osyncstream(std::cout) << "\nInterrupted by signal " << fdsi.ssi_signo <<
            ". Exiting gracefully..." << std::endl;
        }
        source.request_stop();
        continue;
      }
      if (const auto it = suspended_coros.find(fd); it != suspended_coros.end()) {
        const auto h = it->second;
        suspended_coros.erase(it);

        if (epoll_ctl(epoll_fd.fd, EPOLL_CTL_DEL, fd, nullptr) < 0) {
          if (errno != ENOENT) {
            throw_sys_error();
          }
        }

        h.resume();
      }
    }
  }
};

struct EpollAwaitable {
  EventLoop& loop;
  int fd;
  uint32_t events;
  
  bool await_ready() const noexcept {
    return false;
  }
  void await_suspend(std::coroutine_handle<> h) {
    loop.register_event(fd, events, h);
  }
  void await_resume() const noexcept {
  }
};

void make_nonblocking(int  fd) {
  int flags = fcntl(fd, F_GETFL, 0);
  if (flags < 0) {
    throw_sys_error();
  }
  if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) < 0) {
    throw_sys_error();
  }
}

Task async_read_stdin(EventLoop& loop, std::stop_source source) {
  int fd = STDIN_FILENO;
  try {
    make_nonblocking(fd);
  } catch(const std::exception& ex) {
    std::osyncstream(std::cout) << "Initialization error: " << ex.what() << std::endl;
    source.request_stop();
    co_return;
  }

  char buff[1024];
  std::stop_token token = source.get_token();

  while (!token.stop_requested()) {
    co_await EpollAwaitable{loop, fd, EPOLLIN};

    if (token.stop_requested()) {
      co_return;
    }

    ssize_t bytes = read(fd, buff, sizeof(buff)-1);
    if (bytes > 0) {
      buff[bytes] = '\0';
      auto sv = trim(std::string_view(buff, bytes));
      if (sv == "exit") {
        source.request_stop();
        co_return;
      }
      std::osyncstream(std::cout) << std::quoted(buff) << std::endl;
    } else if (bytes == 0) {
      source.request_stop();
      co_return;
    } else {
      if (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR) {
        continue;
      }

      char errbuff[128];
      const char* errstr = strerror_r(errno, errbuff, sizeof(errbuff));
      std::osyncstream(std::cout) << "Read error: " << errstr<< std::endl;
      source.request_stop();
      co_return;
    }
  }
}

int main() try {
  EventLoop loop;
  std::stop_source source;
  std::stop_token token = source.get_token();

  std::stop_callback cb(token, [&loop]{
    loop.wake_up();
  });

  Task my_task = async_read_stdin(loop, source);

  if (my_task.handle && !my_task.handle.done()) {
    my_task.handle.resume();
  }

  while (!token.stop_requested()) {
    loop.run_once(source);
  }

  return 0;
} catch (const std::exception& ex) {
  std::osyncstream(std::cout) << "Fatal error: " << ex.what() << std::endl;
  return 1;
}

