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

#ifndef __UKVM_KERNEL_H__
#define __UKVM_KERNEL_H__

#define EBBRT_PRINTF 0x24fcbb 

#include "../kernel.h"

//#include "ukvm_guest.h"

#    define UKVM_GUEST_PTR(T) T
enum ukvm_hypercall {
    /* UKVM_HYPERCALL_RESERVED=0 */
    UKVM_HYPERCALL_WALLTIME=1,
    UKVM_HYPERCALL_PUTS,
    UKVM_HYPERCALL_POLL,
    UKVM_HYPERCALL_BLKINFO,
    UKVM_HYPERCALL_BLKWRITE,
    UKVM_HYPERCALL_BLKREAD,
    UKVM_HYPERCALL_NETINFO,
    UKVM_HYPERCALL_NETWRITE,
    UKVM_HYPERCALL_NETREAD,
    UKVM_HYPERCALL_HALT,
    UKVM_HYPERCALL_MAX
};

/*
 * Hypercall definitions follow.
 */

/* UKVM_HYPERCALL_WALLTIME */
struct ukvm_walltime {
    /* OUT */
    uint64_t nsecs;
};

/* UKVM_HYPERCALL_PUTS */
struct ukvm_puts {
    /* IN */
    UKVM_GUEST_PTR(const char *) data;
    size_t len;
};

/* UKVM_HYPERCALL_BLKINFO */
struct ukvm_blkinfo {
    /* OUT */
    size_t sector_size;
    size_t num_sectors;
    int rw;
};

/* UKVM_HYPERCALL_BLKWRITE */
struct ukvm_blkwrite {
    /* IN */
    size_t sector;
    UKVM_GUEST_PTR(const void *) data;
    size_t len;

    /* OUT */
    int ret;
};

/* UKVM_HYPERCALL_BLKREAD */
struct ukvm_blkread {
    /* IN */
    size_t sector;
    UKVM_GUEST_PTR(void *) data;

    /* IN/OUT */
    size_t len;

    /* OUT */
    int ret;
};

/* UKVM_HYPERCALL_NETINFO */
struct ukvm_netinfo {
    /* OUT */
    uint8_t mac_address[6];
};

/* UKVM_HYPERCALL_NETWRITE */
struct ukvm_netwrite {
    /* IN */
    UKVM_GUEST_PTR(const void *) data;
    size_t len;

    /* OUT */
    int ret;
};

/* UKVM_HYPERCALL_NETREAD */
struct ukvm_netread {
    /* IN */
    UKVM_GUEST_PTR(void *) data;

    /* IN/OUT */
    size_t len;

    /* OUT */
    int ret;
};

/*
 * UKVM_HYPERCALL_POLL: Block until timeout_nsecs have passed or I/O is
 * possible, whichever is sooner. Returns 1 if I/O is possible, otherwise 0.
 *
 * TODO: Extend this interface to select which I/O events are of interest.
 */
struct ukvm_poll {
    /* IN */
    uint64_t timeout_nsecs;

    /* OUT */
    int ret;
};

/*
 * UKVM_HYPERCALL_HALT: Terminate guest execution.
 *
 * (exit_status) will be returned to the host.
 *
 * Additionally, the guest may supplied a (cookie) providing a hint to the
 * monitor about where e.g. a trap frame may be found in guest memory. The
 * memory area pointed to by (cookie) must allow for at least
 * UKVM_HALT_COOKIE_MAX bytes.
 */
#define UKVM_HALT_COOKIE_MAX 512

struct ukvm_halt {
    /* IN */
    UKVM_GUEST_PTR(void *) cookie;

    /* IN */
    int exit_status;
};

/*
 * A pointer to this structure is passed by the monitor as the sole argument to
 * the guest entrypoint.
 */
struct ukvm_cpu_boot_info {
    uint64_t tsc_freq;
    uint64_t ebbrt_printf_addr;
};

struct ukvm_boot_info {
    uint64_t mem_size;                  /* Memory size in bytes */
    uint64_t kernel_end;                /* Address of end of kernel */
    UKVM_GUEST_PTR(char *) cmdline;     /* Address of command line (C string) */
    struct ukvm_cpu_boot_info cpu;      /* Arch-dependent part (see above) */
};

void ukvm_do_hypercall(int n, volatile void *arg);

void time_init(struct ukvm_boot_info *bi);
void console_init(void);
void net_init(void);

/* tscclock.c: TSC-based clock */
uint64_t tscclock_monotonic(void);
int tscclock_init(uint64_t tsc_freq);
uint64_t tscclock_epochoffset(void);

void process_bootinfo(void *arg);
#endif
