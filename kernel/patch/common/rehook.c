// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Copyright (C) 2026 rifsxd.
 * All Rights Reserved.
 */

#include <syscall.h>
#include <uapi/scdefs.h>
#include <symbol.h>
#include <linux/printk.h>
#include <kputils.h>

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof(arr[0]))

extern void *sys_call_table;
extern void *compat_sys_call_table;
extern int has_config_compat;

static int rehook_enabled = 0;

static int nr_arg = 2;

static const int rehook_native_syscalls[] = {
    __NR_getpriority
};

// static const int rehook_compat_syscalls[] = {
//     96
// };

static void rehook_before(hook_fargs2_t *args, void *udata)
{
    if (current_uid() != 0u) // only allow root 
        return;
}

static inline bool syscall_is_valid(int nr)
{
    void **table;
    
    table = (void **)sys_call_table;
    if (!table || table[nr] == NULL)
        return false;
    
    return true;
}

// static inline bool compat_syscall_is_valid(int nr)
// {
//     void **table;
    
//     table = (void **)compat_sys_call_table;
//     if (!table || table[nr] == NULL)
//         return false;
    
//     return true;
// }

static void hook_native_syscalls(void)
{
    logki("hook_native_syscalls start\n");

    for (size_t i = 0; i < ARRAY_SIZE(rehook_native_syscalls); i++) {
        int nr = rehook_native_syscalls[i];
        
        // skip invalid/unimplemented syscalls
        if (!syscall_is_valid(nr)) {
            logkfd("skipping invalid native syscall %d\n", nr);
            continue;
        }
        
        hook_syscalln(nr, nr_arg, rehook_before, 0, 0);
        logkfd("hooked native syscall %d\n", nr);
    }

    logki("hook_native_syscalls done\n");
}

static void unhook_native_syscalls(void)
{
    logki("unhook_native_syscalls start\n");

    for (size_t i = 0; i < ARRAY_SIZE(rehook_native_syscalls); i++) {
        int nr = rehook_native_syscalls[i];
        
        // skip invalid/unimplemented syscalls
        if (!syscall_is_valid(nr)) {
            logkfd("skipping invalid native syscall %d\n", nr);
            continue;
        }
        
        unhook_syscalln(nr, rehook_before, 0);
        logkfd("unhooked native syscall %d\n", nr);
    }

    logki("unhook_native_syscalls done\n");
}

/* ---------------- Compat ---------------- */
// static void hook_compat_syscalls(void)
// {
//     if (!has_config_compat || !compat_sys_call_table)
//         return;

//     logki("hook_compat_syscalls start\n");

//     for (size_t i = 0; i < ARRAY_SIZE(rehook_compat_syscalls); i++) {
//         int nr = rehook_compat_syscalls[i];
        
//         // skip invalid/unimplemented syscalls
//         if (!compat_syscall_is_valid(nr)) {
//             logkfd("skipping invalid compat syscall %d\n", nr);
//             continue;
//         }
        
//         hook_compat_syscalln(nr, 0, rehook_before, 0, 0);
//         logkfd("hooked compat syscall %d\n", nr);
//     }

//     logki("hook_compat_syscalls done\n");
// }

// static void unhook_compat_syscalls(void)
// {
//     if (!has_config_compat || !compat_sys_call_table)
//         return;

//     logki("unhook_compat_syscalls start\n");

//     for (size_t i = 0; i < ARRAY_SIZE(rehook_compat_syscalls); i++) {
//         int nr = rehook_compat_syscalls[i];
        
//         // skip invalid/unimplemented syscalls
//         if (!compat_syscall_is_valid(nr)) {
//             logkfd("skipping invalid compat syscall %d\n", nr);
//             continue;
//         }
        
//         unhook_compat_syscalln(nr, rehook_before, 0);
//         logkfd("unhooked compat syscall %d\n", nr);
//     }

//     logki("unhook_compat_syscalls done\n");
// }

/* ---------------- Init / Exit ---------------- */
int rehook_init(void)
{
    logki("rehook_init start\n");

    hook_native_syscalls();
    // hook_compat_syscalls();

    rehook_enabled = 1;

    logki("rehook_init complete\n");
    return 0;
}
KP_EXPORT_SYMBOL(rehook_init);

int rehook_exit(void)
{
    logki("rehook_exit start\n");

    unhook_native_syscalls();
    // unhook_compat_syscalls();

    rehook_enabled = 0;

    logki("rehook_exit complete\n");
    return 0;
}
KP_EXPORT_SYMBOL(rehook_exit);

int rehook_status(void)
{
    return rehook_enabled;
}
KP_EXPORT_SYMBOL(rehook_status);