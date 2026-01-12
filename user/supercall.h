/* SPDX-License-Identifier: GPL-2.0-or-later */
/* 
 * Copyright (C) 2023 bmax121. All Rights Reserved.
 */

#ifndef _KPU_SUPERCALL_H_
#define _KPU_SUPERCALL_H_

#include <unistd.h>
#include <sys/syscall.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <errno.h>

#include "uapi/scdefs.h"
#include "../version"

static inline long ver_and_cmd(long cmd)
{
    uint32_t version_code = (MAJOR << 16) + (MINOR << 8) + PATCH;
    return ((long)version_code << 32) | (0x2026 << 16) | (cmd & 0xFFFF);
}

static inline long compact_cmd(long cmd)
{
    long ver = syscall(__NR_supercall, NULL, ver_and_cmd(SUPERCALL_KERNELPATCH_VER));
    if (ver >= 0xa05) return ver_and_cmd(cmd);
    return cmd;
}

static inline long sc_hello(void)
{
    return syscall(__NR_supercall, NULL, compact_cmd(SUPERCALL_HELLO));
}

static inline bool sc_ready(void)
{
    return sc_hello() == SUPERCALL_HELLO_MAGIC;
}

static inline long sc_klog(const char *msg)
{
    if (!msg || strlen(msg) <= 0) return -EINVAL;
    return syscall(__NR_supercall, NULL, compact_cmd(SUPERCALL_KLOG), msg);
}

static inline uint32_t sc_kp_ver(void)
{
    long ret = syscall(__NR_supercall, NULL, compact_cmd(SUPERCALL_KERNELPATCH_VER));
    return (uint32_t)ret;
}

static inline uint32_t sc_k_ver(void)
{
    long ret = syscall(__NR_supercall, NULL, compact_cmd(SUPERCALL_KERNEL_VER));
    return (uint32_t)ret;
}

static inline long sc_kpm_load(const char *path, const char *args, void *reserved)
{
    if (!path || strlen(path) <= 0) return -EINVAL;
    return syscall(__NR_supercall, NULL, compact_cmd(SUPERCALL_KPM_LOAD), path, args, reserved);
}

static inline long sc_kpm_control(const char *name, const char *ctl_args, char *out_msg, long outlen)
{
    if (!name || strlen(name) <= 0) return -EINVAL;
    if (!ctl_args || strlen(ctl_args) <= 0) return -EINVAL;
    return syscall(__NR_supercall, NULL, compact_cmd(SUPERCALL_KPM_CONTROL), name, ctl_args, out_msg, outlen);
}

static inline long sc_kpm_unload(const char *name, void *reserved)
{
    if (!name || strlen(name) <= 0) return -EINVAL;
    return syscall(__NR_supercall, NULL, compact_cmd(SUPERCALL_KPM_UNLOAD), name, reserved);
}

static inline long sc_kpm_nums(void)
{
    return syscall(__NR_supercall, NULL, compact_cmd(SUPERCALL_KPM_NUMS));
}

static inline long sc_kpm_list(char *names_buf, int buf_len)
{
    if (!names_buf || buf_len <= 0) return -EINVAL;
    return syscall(__NR_supercall, NULL, compact_cmd(SUPERCALL_KPM_LIST), names_buf, buf_len);
}

static inline long sc_kpm_info(const char *name, char *buf, int buf_len)
{
    if (!buf || buf_len <= 0) return -EINVAL;
    return syscall(__NR_supercall, NULL, compact_cmd(SUPERCALL_KPM_INFO), name, buf, buf_len);
}

static inline long sc_bootlog(void)
{
    return syscall(__NR_supercall, NULL, compact_cmd(SUPERCALL_BOOTLOG));
}

static inline long sc_panic(void)
{
    return syscall(__NR_supercall, NULL, compact_cmd(SUPERCALL_PANIC));
}

static inline long sc_kstorage_read(int gid, long did, void *out_data, int offset, int dlen)
{
    return syscall(__NR_supercall, NULL, ver_and_cmd(SUPERCALL_KSTORAGE_READ), gid, did, out_data, (((long)offset << 32) | dlen));
}

static inline long sc_kstorage_write(int gid, long did, void *data, int offset, int dlen)
{
    return syscall(__NR_supercall, NULL, ver_and_cmd(SUPERCALL_KSTORAGE_WRITE), gid, did, data, (((long)offset << 32) | dlen));
}

static inline long sc_kstorage_remove(int gid, long did)
{
    return syscall(__NR_supercall, NULL, ver_and_cmd(SUPERCALL_KSTORAGE_REMOVE), gid, did);
}

static inline long sc_set_ap_mod_exclude(uid_t uid, int exclude)
{
    if(exclude) {
        return sc_kstorage_write(KSTORAGE_EXCLUDE_LIST_GROUP, uid, &exclude, 0, sizeof(exclude));
    } else {
        return sc_kstorage_remove(KSTORAGE_EXCLUDE_LIST_GROUP, uid);
    }
}

static inline int sc_get_ap_mod_exclude(uid_t uid)
{
    int exclude = 0;
    int rc = sc_kstorage_read(KSTORAGE_EXCLUDE_LIST_GROUP, uid, &exclude, 0, sizeof(exclude));
    if (rc < 0) return 0;
    return exclude;
}

static inline int sc_rehook_syscall(int enable)
{
    return syscall(__NR_supercall, NULL, ver_and_cmd(SUPERCALL_REHOOK_SYSCALL), (long)enable);
}

static inline int sc_rehook_status(void)
{
    return syscall(__NR_supercall, NULL, ver_and_cmd(SUPERCALL_REHOOK_STATUS));
}

#endif