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

long set_minimal_hooks(int enable);
long set_target_hooks(int enable);

int kprehook_minimal_main(int argc, char **argv);
int kprehook_target_main(int argc, char **argv);

int kprehook_minimal_status_main(int argc, char **argv);
int kprehook_target_status_main(int argc, char **argv);

#ifdef __cplusplus
}
#endif

#endif