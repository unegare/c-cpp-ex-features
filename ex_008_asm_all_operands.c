#include <stdio.h>

extern int arr[8];
extern int arr2[8];

__asm__ (".data ; arr: .zero 32 ; .comm arr2,40,32 ; .text");

__attribute__((/*fastcall,*/noinline)) int _asm_foo (int a1, int a2, int a3, int a4, int a5, int a6, int a7);

__asm__ ("_asm_foo: movq %rdi, %rax ; addq 0x8(%rsp), %rax ; ret ;");
//0x8(%rax,%rbx,0x4) \equiv [rax + rbx*4h - 8h]

__attribute__((/*fastcall,*/noinline)) int _asm_foo2 (int a1, int a2, int a3, int a4, int a5, int a6, int a7);

__asm__ (".intel_syntax noprefix\n_asm_foo2:\n\tmov rax, rdi\n\tadd rax, rsi\n\tadd rax, [rsp+8]\n\tret\n\t.att_syntax\n\t");

void foo (long x) {
  printf ("%s : %ld\n", __FUNCTION__, x);
}

int main () {
  long var = 4;
  printf ("var == %ld\n", var);
  __asm__ volatile ("movq %1, %0":"=r"(var):"i"(2):"memory");//r - register, i - integer, m - memory; = for write, + for read and write
  printf ("var == %ld\n", var);
  __asm__ volatile ("movq %1, %[var]\n\t\
                     movq $10, %%rdi\n\t\
                     call foo\n\t":[var]"=m"(var):"i"(6):"memory"); //args: rdi,rsi,rdx,rcx,r8,r9,stack; return value: rax;
                                                                    //assumption: rbx,rbp,r12,r13,r14,r15 should be restored by foo
                                                                    //rcx,rdx,rsi,rdi,r8,r9,10,r11 should be saved before call
  
  foo (15);
  arr[5] = 10;
  arr2[7] = 4;
  printf ("var == %ld\n", var);
  printf ("_asm_foo ret value == %d\n", _asm_foo(19,2,3,4,5,6,7));
  printf ("_asm_foo2 ret value == %d\n", _asm_foo2(25,2,3,4,5,6,7));
  printf ("%d\n%d\n", arr[5], arr2[7]);
  return 0;
}
