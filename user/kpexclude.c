#include "kpexclude.h"

#include <stdio.h>
#include <stdlib.h>
#include <error.h>
#include <string.h>
#include "supercall.h"

extern const char *key;
extern const char program_name[];

static void set_usage(int status)
{
    if (status != EXIT_SUCCESS)
        fprintf(stderr, "Try `%s exclude help' for more information.\n", program_name);
    else {
        printf("Usage: %s exclude <UID> <0|1>\n\n", program_name);
        printf(
            "Exclude command.\n\n"
            "help                 Print this help message.\n"
            "<UID> 1              Add UID to exclude list.\n"
            "<UID> 0              Remove UID from exclude list.\n"
        );
    }
    exit(status);
}

static void get_usage(int status)
{
    if (status != EXIT_SUCCESS)
        fprintf(stderr, "Try `%s exclude_get help' for more information.\n", program_name);
    else {
        printf("Usage: %s exclude_get <UID>\n\n", program_name);
        printf(
            "Get exclude command.\n\n"
            "help                 Print this help message.\n"
            "<UID>                Check if UID is in exclude list.\n"
        );
    }
    exit(status);
}

long set_uid_exclude(uid_t uid, int exclude)
{
    if (!key || !key[0])
        error(-EINVAL, 0, "superkey not set");

    if (exclude != 0 && exclude != 1)
        error(-EINVAL, 0, "exclude must be 0 or 1");

    long rc = sc_set_ap_mod_exclude(key, uid, exclude);
    if (rc < 0)
        return rc;

    printf("UID %d %s exclude list\n",
           uid, exclude ? "added to" : "removed from");

    return rc;
}

long get_uid_exclude(uid_t uid)
{
    if (!key || !key[0])
        error(-EINVAL, 0, "superkey not set");

    long rc = sc_get_ap_mod_exclude(key, uid);
    if (rc < 0)
        return rc;

    printf("UID %d %s in exclude list\n",
           uid, rc ? "is" : "is not");

    return rc;
}

int kpexclude_set_main(int argc, char **argv)
{
    if (argc != 2)
        set_usage(EXIT_FAILURE);

    if (!strcmp(argv[0], "help"))
        set_usage(EXIT_SUCCESS);

    uid_t uid = (uid_t)atoi(argv[0]);
    int exclude = atoi(argv[1]);

    return set_uid_exclude(uid, exclude);
}

int kpexclude_get_main(int argc, char **argv)
{
    if (argc != 1)
        get_usage(EXIT_FAILURE);

    if (!strcmp(argv[0], "help"))
        get_usage(EXIT_SUCCESS);

    uid_t uid = (uid_t)atoi(argv[0]);

    return get_uid_exclude(uid);
}
