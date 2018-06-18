/* Borrowing some ideas from ASC:
   https://github.com/PSML/asc/blob/master/kernels/crt0.s */

/* We don't have a way for our target to affect the outside world yet,
   so returning to AppMain will allow us to return a value for debugging.
   Assume caller has passed argc through rdi and argv through rsi. 
   Note that if RET_TO_APPMAIN is 0, caller is left with return addr
   still hanging off the end of his stack. */
#define RET_TO_APPMAIN 1

  /* A simple C runtime bootstrap.  */
  .globl _svEntryPt

_svEntryPt:

#if RET_TO_APPMAIN
  /* Store old rbp. */
  push %rbp
  /* Make copy of rsp */
  mov %rsp, %rax
#endif

  /* Required by the ABI.  */
  xor %rbp, %rbp

  /* Swizzle stack. TODO: using symbol injected by linker script. */
  mov $0xFFFFC07FFFFFFFFF, %rsp
  /* mov $sv_last, %rsp */

  /* Align stack pointer as per ABI.  */
  and $0xfffffffffffffff0, %rsp

#if RET_TO_APPMAIN
  /* Push old rsp onto new stack. */
  push %rax
#endif

  /* Jump into C entry point.  */
  call _start

#if RET_TO_APPMAIN
  /* Still on new stack, recover old rsp. */
  pop %rsp

  /* Restore rbp */
  pop %rbp

  /* Return to AppMain */
  ret

  /* We should not be here.  */
  hlt
#else
  /* Normally crt0 would just record return code and exit. */
  /* Program return code.  */
  mov %rax, %rdi

  /* Exit.  */
  mov $0x3c, %rax
  syscall

  /* We should not be here.  */
  hlt

#endif
