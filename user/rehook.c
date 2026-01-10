/* SPDX-License-Identifier: GPL-2.0-or-later */
/* 
 * Copyright (C) 2026 rifsxd.
 * All Rights Reserved.
 */

#include <stdio.h>
#include <stdlib.h>
#include <error.h>
#include <string.h>
#include <errno.h>
#include "supercall.h"

extern const char program_name[];

static void rehook_usage(int status)
{
    if (status != EXIT_SUCCESS)
        fprintf(stderr, "Try `%s rehook help' for more information.\n", program_name);
    else {
        printf("Usage: %s rehook <0|1|2>\n\n", program_name);
        printf(
            "Syscall rehook mode command.\n\n"
            "help                 Print this help message.\n"
            "0                    Disable rehook syscalls.\n"
            "1                    Enable target syscall rehooks (specific syscalls only).\n"
            "2                    Enable minimal syscall rehooks (all syscalls except whitelist).\n"
            "\n"
            "Note: Only one mode can be active at a time.\n"
            "See also: rehook_status\n"
        );
    }
    exit(status);
}

static void rehook_status_usage(int status)
{
    if (status != EXIT_SUCCESS)
        fprintf(stderr, "Try `%s rehook_status help' for more information.\n", program_name);
    else {
        printf("Usage: %s rehook_status\n\n", program_name);
        printf(
            "Check syscall rehook mode status.\n\n"
            "help                 Print this help message.\n"
            "\n"
            "Returns:\n"
            "  0 = All rehooks disabled\n"
            "  1 = Target rehooks enabled\n"
            "  2 = Minimal rehooks enabled\n"
        );
    }
    exit(status);
}

long set_rehook_mode(int mode)
{
    if (mode < 0 || mode > 2)
        error(-EINVAL, 0, "mode must be 0, 1, or 2");

    long minimal_status = sc_minimal_hooks_status();
    long target_status = sc_target_hooks_status();
    
    if (minimal_status < 0) {
        printf("Error getting minimal rehooks status: %ld\n", minimal_status);
        return minimal_status;
    }
    if (target_status < 0) {
        printf("Error getting target rehooks status: %ld\n", target_status);
        return target_status;
    }

    int current_mode = 0;
    if (minimal_status == 1) current_mode = 2;
    else if (target_status == 1) current_mode = 1;

    if (current_mode == mode) {
        const char *mode_str[] = {"disabled", "target mode", "minimal mode"};
        printf("Syscall rehooks already in mode %d (%s)\n", mode, mode_str[mode]);
        return 0;
    }

    long rc = 0;

    if (current_mode == 2) {
        rc = sc_minimal_syscall_hooks(0);
        if (rc < 0) {
            printf("Error disabling minimal rehooks: %ld\n", rc);
            return rc;
        }
    } else if (current_mode == 1) {
        rc = sc_target_syscall_hooks(0);
        if (rc < 0) {
            printf("Error disabling target rehooks: %ld\n", rc);
            return rc;
        }
    }

    if (mode == 2) {
        rc = sc_minimal_syscall_hooks(1);
        if (rc < 0) {
            printf("Error enabling minimal rehooks: %ld\n", rc);
            return rc;
        }
        printf("Syscall rehooks switched to mode 2 (minimal mode)\n");
    } else if (mode == 1) {
        rc = sc_target_syscall_hooks(1);
        if (rc < 0) {
            printf("Error enabling target rehooks: %ld\n", rc);
            return rc;
        }
        printf("Syscall rehooks switched to mode 1 (target mode)\n");
    } else {
        printf("Syscall rehooks switched to mode 0 (all rehooks disabled)\n");
    }

    return rc;
}

long get_rehook_status(void)
{
    long minimal_status = sc_minimal_hooks_status();
    long target_status = sc_target_hooks_status();
    
    if (minimal_status < 0) {
        printf("Error getting minimal rehooks status: %ld\n", minimal_status);
        return minimal_status;
    }
    if (target_status < 0) {
        printf("Error getting target rehooks status: %ld\n", target_status);
        return target_status;
    }

    int mode = 0;
    const char *mode_desc = "disabled";
    
    if (minimal_status == 1) {
        mode = 2;
    } else if (target_status == 1) {
        mode = 1;
    }

    printf("Syscall rehooks mode: %d\n", mode);
    
    return mode;
}

int kprehook_main(int argc, char **argv)
{
    if (argc != 1)
        rehook_usage(EXIT_FAILURE);

    if (!strcmp(argv[0], "help"))
        rehook_usage(EXIT_SUCCESS);

    int mode = atoi(argv[0]);

    return set_rehook_mode(mode);
}

int kprehook_status_main(int argc, char **argv)
{
    if (argc > 0 && !strcmp(argv[0], "help"))
        rehook_status_usage(EXIT_SUCCESS);
    
    return get_rehook_status();
}