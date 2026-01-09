/* SPDX-License-Identifier: GPL-2.0-or-later */
/* 
 * Copyright (C) 2026 rifsxd.
 * All Rights Reserved.
 */

#include <stdio.h>
#include <stdlib.h>
#include <error.h>
#include <string.h>
#include "supercall.h"

extern const char *key;
extern const char program_name[];

static void minimal_usage(int status)
{
    if (status != EXIT_SUCCESS)
        fprintf(stderr, "Try `%s rehook_minimal help' for more information.\n", program_name);
    else {
        printf("Usage: %s rehook_minimal <0|1>\n\n", program_name);
        printf(
            "Minimal syscall hooks command.\n\n"
            "help                 Print this help message.\n"
            "1                    Enable minimal syscall hooks.\n"
            "0                    Disable minimal syscall hooks.\n"
            "\n"
            "Note: Cannot enable while target hooks are enabled.\n"
            "See also: rehook_minimal_status\n"
        );
    }
    exit(status);
}

static void target_usage(int status)
{
    if (status != EXIT_SUCCESS)
        fprintf(stderr, "Try `%s rehook_target help' for more information.\n", program_name);
    else {
        printf("Usage: %s rehook_target <0|1>\n\n", program_name);
        printf(
            "Target syscall hooks command.\n\n"
            "help                 Print this help message.\n"
            "1                    Enable target syscall hooks.\n"
            "0                    Disable target syscall hooks.\n"
            "\n"
            "Note: Cannot enable while minimal hooks are enabled.\n"
            "See also: rehook_target_status\n"
        );
    }
    exit(status);
}

static void minimal_status_usage(int status)
{
    if (status != EXIT_SUCCESS)
        fprintf(stderr, "Try `%s rehook_minimal_status help' for more information.\n", program_name);
    else {
        printf("Usage: %s rehook_minimal_status\n\n", program_name);
        printf(
            "Check minimal syscall hooks status.\n\n"
            "help                 Print this help message.\n"
            "\n"
            "Returns: 0 = disabled, 1 = enabled\n"
        );
    }
    exit(status);
}

static void target_status_usage(int status)
{
    if (status != EXIT_SUCCESS)
        fprintf(stderr, "Try `%s rehook_target_status help' for more information.\n", program_name);
    else {
        printf("Usage: %s rehook_target_status\n\n", program_name);
        printf(
            "Check target syscall hooks status.\n\n"
            "help                 Print this help message.\n"
            "\n"
            "Returns: 0 = disabled, 1 = enabled\n"
        );
    }
    exit(status);
}

long set_minimal_hooks(int enable)
{
    if (enable != 0 && enable != 1)
        error(-EINVAL, 0, "enable must be 0 or 1");

    long current = sc_minimal_hooks_status(key);
    if (current < 0) {
        printf("Error getting current status: %ld\n", current);
        return current;
    }

    if (current == enable) {
        printf("Minimal syscall hooks already %s\n",
               enable ? "enabled" : "disabled");
        return 0;
    }

    // If trying to enable, check if target hooks are enabled
    if (enable == 1) {
        long target_status = sc_target_hooks_status(key);
        if (target_status < 0) {
            printf("Error checking target hooks status: %ld\n", target_status);
            return target_status;
        }
        if (target_status == 1) {
            printf("Cannot enable minimal hooks: target hooks are currently enabled\n");
            printf("Please disable target hooks first: %s rehook_target 0\n", program_name);
            return -EBUSY;
        }
    }

    long rc = sc_minimal_syscall_hooks(key, enable);
    if (rc < 0)
        return rc;

    printf("Minimal syscall hooks %s\n",
           enable ? "enabled" : "disabled");

    return rc;
}

long set_target_hooks(int enable)
{
    if (enable != 0 && enable != 1)
        error(-EINVAL, 0, "enable must be 0 or 1");

    long current = sc_target_hooks_status(key);
    if (current < 0) {
        printf("Error getting current status: %ld\n", current);
        return current;
    }

    if (current == enable) {
        printf("Target syscall hooks already %s\n",
               enable ? "enabled" : "disabled");
        return 0;
    }

    // If trying to enable, check if minimal hooks are enabled
    if (enable == 1) {
        long minimal_status = sc_minimal_hooks_status(key);
        if (minimal_status < 0) {
            printf("Error checking minimal hooks status: %ld\n", minimal_status);
            return minimal_status;
        }
        if (minimal_status == 1) {
            printf("Cannot enable target hooks: minimal hooks are currently enabled\n");
            printf("Please disable minimal hooks first: %s rehook_minimal 0\n", program_name);
            return -EBUSY;
        }
    }

    long rc = sc_target_syscall_hooks(key, enable);
    if (rc < 0)
        return rc;

    printf("Target syscall hooks %s\n",
           enable ? "enabled" : "disabled");

    return rc;
}

long get_minimal_hooks_status(void)
{
    long rc = sc_minimal_hooks_status(key);
    if (rc < 0) {
        printf("Error getting minimal hooks status: %ld\n", rc);
        return rc;
    }
    
    printf("Minimal syscall hooks: %s\n", rc ? "enabled" : "disabled");
    return rc;
}

long get_target_hooks_status(void)
{
    long rc = sc_target_hooks_status(key);
    if (rc < 0) {
        printf("Error getting target hooks status: %ld\n", rc);
        return rc;
    }
    
    printf("Target syscall hooks: %s\n", rc ? "enabled" : "disabled");
    return rc;
}

int kprehook_minimal_main(int argc, char **argv)
{
    if (argc != 1)
        minimal_usage(EXIT_FAILURE);

    if (!strcmp(argv[0], "help"))
        minimal_usage(EXIT_SUCCESS);

    int enable = atoi(argv[0]);

    return set_minimal_hooks(enable);
}

int kprehook_target_main(int argc, char **argv)
{
    if (argc != 1)
        target_usage(EXIT_FAILURE);

    if (!strcmp(argv[0], "help"))
        target_usage(EXIT_SUCCESS);

    int enable = atoi(argv[0]);

    return set_target_hooks(enable);
}

int kprehook_minimal_status_main(int argc, char **argv)
{
    if (argc > 0 && !strcmp(argv[0], "help"))
        minimal_status_usage(EXIT_SUCCESS);
    
    return get_minimal_hooks_status();
}

int kprehook_target_status_main(int argc, char **argv)
{
    if (argc > 0 && !strcmp(argv[0], "help"))
        target_status_usage(EXIT_SUCCESS);
    
    return get_target_hooks_status();
}