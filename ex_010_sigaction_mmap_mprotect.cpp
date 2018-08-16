#include <signal.h>
#include <sys/mman.h>
#include <stdio.h>
#include <string.h>
#include <ucontext.h>
#include <stdlib.h>

#include <distorm3/distorm.h>
#include <distorm3/mnemonics.h>

void sigsegv_sigaction (int signum, siginfo_t *sigi, void *ucontext);
void sigtrap_sigaction (int signum, siginfo_t *sigi, void *ucontext);
char *addr;
int failed = 0;

void foo() {
}

void bar() {
foo();
}

int main () {
  addr = (char*)mmap(NULL, 8192, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
  if (!addr) {
    printf ("ERROR: addr == NULL\n");
    return 0;
  }
  bar();
  memcpy(addr, "123", 4);
  memcpy(addr + 4096, "456", 4);
  printf ("%s\n%s\n", addr, addr + 4096);
  if (mprotect(addr, 4096, PROT_NONE)) {
    printf ("ERROR: mprotect returned non-zero value\n");
    return 0;
  }
  struct sigaction siga, sigaold;
  struct sigaction sigat, sigatold;
  sigemptyset(&siga.sa_mask);
  siga.sa_flags = SA_SIGINFO;
  siga.sa_handler = NULL;
  siga.sa_sigaction = sigsegv_sigaction;
  if (sigaction (SIGSEGV, &siga, &sigaold)) {
    printf ("ERROR: sigaction returned non-zero value\n");
    return 0;
  }
  sigemptyset(&sigat.sa_mask);
  sigat.sa_flags = SA_SIGINFO;
  sigat.sa_handler = NULL;
  sigat.sa_sigaction = sigtrap_sigaction;
  if (sigaction (SIGTRAP, &sigat, &sigatold))
  {
    perror ("sigaction");
    abort ();
  }
//  * (int*) NULL = 0;
  printf ("%s\n", addr);
  if (failed)
     printf ("addr has been accessed.\n");
  return 0;
}

void sigtrap_sigaction (int signum, siginfo_t *sigi, void *ucontext) {
  ucontext_t *current = (ucontext_t *) ucontext;
  printf ("sigtrap_sigaction has been called at 0x%llx\n", current->uc_mcontext.gregs[REG_RIP]);
  current->uc_mcontext.gregs[REG_EFL] &= ~(1 << 8);
  mprotect (addr, 4096, PROT_NONE);
}

void sigsegv_sigaction (int signum, siginfo_t *sigi, void *ucontext) {
  ucontext_t *current = (ucontext_t *) ucontext;
  printf ("sigsegv_sigaction has been called at 0x%llx\n", current->uc_mcontext.gregs[REG_RIP]);
  printf ("crashed at %p\n", sigi->si_addr);
  printf ("addr is %p\n", addr);
  
  _DInst decodedInstructions[1];
  unsigned int decodedInstructionsCount = 0;
  _DecodeType dt = Decode64Bits;
  unsigned char buf[20];
  
//  memset(buf, 0x90, 20);
  memcpy(buf, (char*)current->uc_mcontext.gregs[REG_RIP], 20);
  
  _CodeInfo ci;
  ci.code = buf;
  ci.codeLen = sizeof(buf);
  ci.codeOffset = 0;
  ci.dt = dt;
  ci.features = DF_NONE;
  distorm_decompose(&ci, decodedInstructions, 1, &decodedInstructionsCount);
  printf ("%s\n", GET_MNEMONIC_NAME(decodedInstructions[0].opcode));

  if (sigi->si_addr < addr || sigi->si_addr >= addr + 4096)
  {
    // Got SIGSEGV not on a guard page.
    abort();
  }
  current->uc_mcontext.gregs[REG_EFL] |= 1 << 8;
  mprotect (addr, 4096, PROT_READ);
  failed = 1;
}
