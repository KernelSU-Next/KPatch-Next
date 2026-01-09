/* SPDX-License-Identifier: GPL-2.0-or-later */
/* 
 * Copyright (C) 2026 rifsxd.
 * All Rights Reserved.
 */

#ifndef _KPU_REHOOK_H
#define _KPU_REHOOK_H

#include <unistd.h>

#ifdef __cplusplus
extern "C" {
#endif

long set_rehook_mode(int mode);
long get_rehook_status(void);

int kprehook_main(int argc, char **argv);
int kprehook_status_main(int argc, char **argv);

#ifdef __cplusplus
}
#endif

#endif