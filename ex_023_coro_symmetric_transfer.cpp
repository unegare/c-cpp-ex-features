#include <coroutine>
#include <exception>
#include <iostream>
#include <syncstream>
#include <optional>
#include <utility>

#ifndef DBG
#define DBG 1
#endif

template<typename T>
struct PromiseStorageType {
  std::optional<T> value;
  void return_value(T&& val) {
    value = std::move(val);
  }
};

template<>
struct PromiseStorageType<void> {
  void return_void() const noexcept {
  }
};

template<typename T>
struct [[nodiscard]] Task {
//  using StorageType = std::conditional_t<std::is_void_v<T>, std::monostate, T>;
  struct promise_type : PromiseStorageType<T> {
//    std::optional<StorageType> value;
    std::exception_ptr exception;
    std::coroutine_handle<> continuation;

    Task get_return_object() {
        return Task{std::coroutine_handle<promise_type>::from_promise(*this)};
    }
    std::suspend_always initial_suspend() noexcept { return {}; }

    struct final_awaiter {
      bool await_ready() const noexcept {
        return false;
      }
      std::coroutine_handle<> await_suspend(std::coroutine_handle<promise_type> h) noexcept {
        if constexpr (DBG) {
          std::osyncstream(std::cout) << __PRETTY_FUNCTION__ << std::endl;
        }
        if (h.promise().continuation) {
          return h.promise().continuation;
        }
        return std::noop_coroutine();
      }
      void await_resume() const noexcept {
      }
    };
    final_awaiter final_suspend() noexcept { return {}; }
    void unhandled_exception() { exception = std::current_exception(); }
  };

  std::coroutine_handle<promise_type> handle;

  Task(std::coroutine_handle<promise_type> h) : handle(h) {}
  Task(const Task&) = delete;
  Task& operator= (const Task&) = delete;
  Task(Task&& other) noexcept : handle(other.handle) {other.handle = nullptr;}
  Task& operator= (Task&& other) noexcept {
    if (this != &other) {
      if (handle) handle.destroy();
      handle = other.handle;
      other.handle = nullptr;
    }
    return *this;
  }
  ~Task() {
    if constexpr (DBG) {
      std::osyncstream(std::cout) << __PRETTY_FUNCTION__ << " : handle is nullptr: " << (handle == nullptr) << std::endl;
    }
    if (handle) handle.destroy();
  }

  auto operator co_await() && {
    struct awaiter {
      std::coroutine_handle<promise_type> h;

      bool await_ready() const noexcept { return h.done(); }

      std::coroutine_handle<> await_suspend(std::coroutine_handle<> signaling) noexcept {
        if constexpr (DBG) {
          std::osyncstream(std::cout) << __PRETTY_FUNCTION__ << std::endl;
        }
        h.promise().continuation = signaling;
        return h;
      }

      T await_resume() {
        if constexpr (DBG) {
          std::osyncstream(std::cout) << __PRETTY_FUNCTION__ << std::endl;
        }
        if (h.promise().exception) {
          std::rethrow_exception(h.promise().exception);
        }
        if constexpr (!std::is_void_v<T>) {
          T result = std::move(*h.promise().value);
          h.destroy();
          return result;
        } else {
          h.destroy();
        }
      }
    };
    return awaiter{std::exchange(handle, nullptr)};
  }
};

Task<int> calculate_value(int x) {
  co_return x;
}

Task<void> run() {
  int result = co_await calculate_value(42);
  std::osyncstream(std::cout) << "Value received: " << result << std::endl;

  int result2 = co_await calculate_value(43);
  std::osyncstream(std::cout) << "Value received: " << result2 << std::endl;
  co_return;
}

int main() {
  auto my_task = run();
  if (my_task.handle && !my_task.handle.done()) {
    my_task.handle.resume();
  }
  return 0;
}
