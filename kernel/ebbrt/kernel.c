/* 
 * Copyright (c) 2015-2017 Contributors as noted in the AUTHORS file
 *
 * This file is part of Solo5, a unikernel base layer.
 *
 * Permission to use, copy, modify, and/or distribute this software
 * for any purpose with or without fee is hereby granted, provided
 * that the above copyright notice and this permission notice appear
 * in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL
 * WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE
 * AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
 * OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT,
 * NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include "kernel.h"

void ukvm_do_hypercall(int n, volatile void *arg)
{

  volatile struct ukvm_puts *puts_buf = arg;
  volatile struct ukvm_walltime *wt = arg;
  switch(n){
    case UKVM_HYPERCALL_PUTS: 
      for(unsigned int i = 0; i < puts_buf->len; i++)
        ebbrt_printf("%c", puts_buf->data[i]);
      break;
    case UKVM_HYPERCALL_WALLTIME:
      wt->nsecs = ebbrt_walltime();
      break;
    case UKVM_HYPERCALL_POLL:
    case UKVM_HYPERCALL_BLKINFO:
    case UKVM_HYPERCALL_BLKWRITE:
    case UKVM_HYPERCALL_BLKREAD:
    case UKVM_HYPERCALL_NETINFO:
    case UKVM_HYPERCALL_NETWRITE:
    case UKVM_HYPERCALL_NETREAD:
    case UKVM_HYPERCALL_HALT:
      ebbrt_printf("\nHalting Solo5. Goodbye!\n"); 
      ebbrt_exit();
    case UKVM_HYPERCALL_MAX:
      ebbrt_printf("Error: Unsupported hypercall #%d\n", n); 
      solo5_abort();
  }
}

void hypercall_init(void *arg)
{
  struct ukvm_boot_info *bi = arg;
  // Setup printf hypercall
  assert(bi->cpu.ebbrt_printf_addr != 0);
  ebbrt_printf= (void (*)(const char *, ...))bi->cpu.ebbrt_printf_addr;
  ebbrt_printf("Hello Solo5!\n");
  // Setup walltime hypercall
  assert(bi->cpu.ebbrt_walltime_addr != 0);
  ebbrt_walltime= (uint64_t (*)())bi->cpu.ebbrt_walltime_addr;
  ebbrt_printf("EbbRT walltime is %llu\n", ebbrt_walltime());
  // Exit call
  assert(bi->cpu.ebbrt_exit_addr != 0);
  ebbrt_printf("EbbRT exit point is %llu\n", bi->cpu.ebbrt_exit_addr);
  ebbrt_exit = (void (*)())bi->cpu.ebbrt_exit_addr;
}

void _start(void *arg)
{

  static struct solo5_start_info si;

  hypercall_init(arg); 
  console_init();
  /* cpu_init(); // TODO: verify this doesnt break EbbRT */
  platform_init(arg);
  si.cmdline = cmdline_parse(platform_cmdline());

  log(INFO, "            |      ___|\n");
  log(INFO, "  __|  _ \\  |  _ \\ __ \\\n");
  log(INFO, "\\__ \\ (   | | (   |  ) |\n");
  log(INFO, "____/\\___/ _|\\___/____/\n");

  mem_init();
  time_init(arg); 
  net_init();

  // maintain locking semantics
  mem_lock_heap(&si.heap_start, &si.heap_size);

  // Overwrite the ukvm defaults for heap start and size
  struct ukvm_boot_info *bi = arg;
  si.heap_start = bi->kernel_end;
  si.heap_size = bi->mem_size;
  /* solo5_exit(solo5_app_main(&si)); */
  solo5_app_main(&si);

}
