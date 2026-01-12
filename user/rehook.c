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
        printf("Usage: %s rehook <enable|disable>\n\n", program_name);
        printf(
            "Rehook syscall command.\n\n"
            "help                 Print this help message.\n"
            "enable               Enable rehook syscall.\n"
            "disable              Disable rehook syscall.\n"
            "\n"
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
            "Check rehook syscall mode status.\n\n"
            "help                 Print this help message.\n"
        );
    }
    exit(status);
}

long set_rehook_mode(int enable)
{
    long rehook_status = sc_rehook_status();
    
    if (rehook_status < 0) {
        printf("Error getting rehook syscall status: %ld\n", rehook_status);
        return 1;
    }

    int current_enabled = (rehook_status == 1) ? 1 : 0;

    if (current_enabled == enable) {
        printf("Rehook syscall: already %s\n", enable ? "enabled" : "disabled");
        return 0;
    }

    long rc = sc_rehook_syscall(enable);
    if (rc < 0) {
        printf("Error %s rehook syscall: %ld\n", enable ? "enabling" : "disabling", rc);
        return 1;
    }

    printf("Rehook syscall: %s\n", enable ? "enabled" : "disabled");
    return 0;
}

long get_rehook_status(void)
{
    long rehook_status = sc_rehook_status();
    
    if (rehook_status < 0) {
        printf("Error getting rehook syscall status: %ld\n", rehook_status);
        return 1;
    }

    int enabled = (rehook_status == 1) ? 1 : 0;
    
    printf("Rehook syscall status: %s\n", enabled ? "enabled" : "disabled");
    
    return 0;
}

int kprehook_main(int argc, char **argv)
{
    if (argc != 1)
        rehook_usage(EXIT_FAILURE);

    if (!strcmp(argv[0], "help"))
        rehook_usage(EXIT_SUCCESS);

    int enable;
    if (!strcmp(argv[0], "enable")) {
        enable = 1;
    } else if (!strcmp(argv[0], "disable")) {
        enable = 0;
    } else {
        fprintf(stderr, "Invalid argument: %s\n", argv[0]);
        rehook_usage(EXIT_FAILURE);
    }

    return set_rehook_mode(enable);
}

int kprehook_status_main(int argc, char **argv)
{
    if (argc > 0 && !strcmp(argv[0], "help"))
        rehook_status_usage(EXIT_SUCCESS);
    
    return get_rehook_status();
}