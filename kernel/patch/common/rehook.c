// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Copyright (C) 2026 rifsxd.
 * All Rights Reserved.
 */

#include <syscall.h>
#include <uapi/scdefs.h>
#include <symbol.h>
#include <supercall.h>
#include <linux/printk.h>
#include <kputils.h>

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof(arr[0]))
#define MIN_SYSCALL_NR 0
#define MAX_SYSCALL_NR 451

static int minimal_hooks_enabled = 0;
static int target_hooks_enabled = 0;

static const int native_skip_syscalls[] = {
    __NR_execve,
    __NR_execveat,
    __NR_getuid,
    __NR_getegid,
    __NR_getgid,
    __NR_setfsuid,
    __NR_setfsgid,
    __NR_setresuid,
    __NR_setresgid,
    __NR_openat,
    __NR3264_fstatat,
    __NR_faccessat,
    __NR_faccessat2,
    __NR_openat2,
    __NR_uname,
    __NR_supercall,
};

static const int compat_skip_syscalls[] = {
    11, 24, 47, 49, 50,
    122, 138, 139,
    164, 170,
    322, 327, 334,
    387,
    437, 439,
};

static const int target_native_syscalls[] = {
    __NR_getpriority
};

static const int target_compat_syscalls[] = {
    96
};

static void minimal_before(hook_fargs0_t *args, void *udata)
{
    if (current_uid() != 0u) // only allow root 
        return;
        
    return;
}

static void target_before(hook_fargs0_t *args, void *udata)
{
    if (current_uid() != 0u) // only allow root 
        return;
        
    return;
}

static inline bool syscall_is_skipped(int nr, const int *list, size_t count)
{
    for (size_t i = 0; i < count; i++) {
        if (nr == list[i])
            return true;
    }
    return false;
}

static void hook_native_syscalls(const int *syscalls, size_t count, void (*callback)(hook_fargs0_t *, void *))
{
    logki("hook_native_syscalls start\n");

    if (syscalls) {
        for (size_t i = 0; i < count; i++) {
            int nr = syscalls[i];
            hook_syscalln(nr, 0, callback, 0, 0);
            logkfd("hooked native syscall %d\n", nr);
        }
    } else {
        for (int nr = MIN_SYSCALL_NR; nr <= MAX_SYSCALL_NR; nr++) {
            if (!syscall_is_skipped(nr, native_skip_syscalls, ARRAY_SIZE(native_skip_syscalls))) {
                hook_syscalln(nr, 0, callback, 0, 0);
                logkfd("hooked native syscall %d\n", nr);
            }
        }
    }

    logki("hook_native_syscalls done\n");
}

static void unhook_native_syscalls(const int *syscalls, size_t count, void (*callback)(hook_fargs0_t *, void *))
{
    logki("unhook_native_syscalls start\n");

    if (syscalls) {
        for (size_t i = 0; i < count; i++) {
            int nr = syscalls[i];
            unhook_syscalln(nr, callback, 0);
            logkfd("unhooked native syscall %d\n", nr);
        }
    } else {
        for (int nr = MIN_SYSCALL_NR; nr <= MAX_SYSCALL_NR; nr++) {
            if (!syscall_is_skipped(nr, native_skip_syscalls, ARRAY_SIZE(native_skip_syscalls))) {
                unhook_syscalln(nr, callback, 0);
                logkfd("unhooked native syscall %d\n", nr);
            }
        }
    }

    logki("unhook_native_syscalls done\n");
}

/* ---------------- Compat ---------------- */
static void hook_compat_syscalls(const int *syscalls, size_t count, void (*callback)(hook_fargs0_t *, void *))
{
    if (!has_config_compat || !compat_sys_call_table)
        return;

    logki("hook_compat_syscalls start\n");

    if (syscalls) {
        for (size_t i = 0; i < count; i++) {
            int nr = syscalls[i];
            hook_compat_syscalln(nr, 0, callback, 0, 0);
            logkfd("hooked compat syscall %d\n", nr);
        }
    } else {
        for (int nr = MIN_SYSCALL_NR; nr <= MAX_SYSCALL_NR; nr++) {
            if (!syscall_is_skipped(nr, compat_skip_syscalls, ARRAY_SIZE(compat_skip_syscalls))) {
                hook_compat_syscalln(nr, 0, callback, 0, 0);
                logkfd("hooked compat syscall %d\n", nr);
            }
        }
    }

    logki("hook_compat_syscalls done\n");
}

static void unhook_compat_syscalls(const int *syscalls, size_t count, void (*callback)(hook_fargs0_t *, void *))
{
    if (!has_config_compat || !compat_sys_call_table)
        return;

    logki("unhook_compat_syscalls start\n");

    if (syscalls) {
        for (size_t i = 0; i < count; i++) {
            int nr = syscalls[i];
            unhook_compat_syscalln(nr, callback, 0);
            logkfd("unhooked compat syscall %d\n", nr);
        }
    } else {
        for (int nr = MIN_SYSCALL_NR; nr <= MAX_SYSCALL_NR; nr++) {
            if (!syscall_is_skipped(nr, compat_skip_syscalls, ARRAY_SIZE(compat_skip_syscalls))) {
                unhook_compat_syscalln(nr, callback, 0);
                logkfd("unhooked compat syscall %d\n", nr);
            }
        }
    }

    logki("unhook_compat_syscalls done\n");
}

/* ---------------- Init / Exit ---------------- */
int minimal_hook_init(void)
{
    logki("minimal_hook_init start\n");

    hook_native_syscalls(NULL, 0, minimal_before);
    hook_compat_syscalls(NULL, 0, minimal_before);

    minimal_hooks_enabled = 1;

    logki("minimal_hook_init complete\n");
    return 0;
}
KP_EXPORT_SYMBOL(minimal_hook_init);

int minimal_hook_exit(void)
{
    logki("minimal_hook_exit start\n");

    unhook_native_syscalls(NULL, 0, minimal_before);
    unhook_compat_syscalls(NULL, 0, minimal_before);

    minimal_hooks_enabled = 0;

    logki("minimal_hook_exit complete\n");
    return 0;
}
KP_EXPORT_SYMBOL(minimal_hook_exit);

int target_hook_init(void)
{
    logki("target_hook_init start\n");

    hook_native_syscalls(target_native_syscalls, ARRAY_SIZE(target_native_syscalls), target_before);
    hook_compat_syscalls(target_compat_syscalls, ARRAY_SIZE(target_compat_syscalls), target_before);

    target_hooks_enabled = 1;

    logki("target_hook_init complete\n");
    return 0;
}
KP_EXPORT_SYMBOL(target_hook_init);

int target_hook_exit(void)
{
    logki("target_hook_exit start\n");

    unhook_native_syscalls(target_native_syscalls, ARRAY_SIZE(target_native_syscalls), target_before);
    unhook_compat_syscalls(target_compat_syscalls, ARRAY_SIZE(target_compat_syscalls), target_before);

    target_hooks_enabled = 0;

    logki("target_hook_exit complete\n");
    return 0;
}
KP_EXPORT_SYMBOL(target_hook_exit);

int minimal_hooks_status(void)
{
    return minimal_hooks_enabled;
}
KP_EXPORT_SYMBOL(minimal_hooks_status);

int target_hooks_status(void)
{
    return target_hooks_enabled;
}
KP_EXPORT_SYMBOL(target_hooks_status);
