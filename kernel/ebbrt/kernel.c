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
  int (*hypercall_)(volatile void *);
  hypercall_ = (int (*)(volatile void *))(hypercall_table[n]);
  if (hypercall_(arg)) {
    solo5_abort();
  }
}

/* Setup the hypercall pointer table */
void hypercall_init(void *arg)
{
  struct ukvm_boot_info *bi = arg;
  hypercall_table[0] = bi->cpu.hypercall_ptr[UKVM_HYPERCALL_HALT];
  for (uint8_t i = 1; i < UKVM_HYPERCALL_MAX; ++i)
    hypercall_table[i] = bi->cpu.hypercall_ptr[i];
}

void _start(void *arg)
{

  static struct solo5_start_info si;

  hypercall_init(arg); 
  console_init();
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
  solo5_exit(solo5_app_main(&si));
}
