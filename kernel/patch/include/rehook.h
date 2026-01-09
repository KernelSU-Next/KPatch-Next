/* SPDX-License-Identifier: GPL-2.0-or-later */
/* 
 * Copyright (C) 2024 bmax121. All Rights Reserved.
 */

#ifndef _KP_REHOOK_H_
#define _KP_REHOOK_H_

int minimal_hook_init();
int minimal_hook_exit();

int target_hook_init();
int target_hook_exit();

int minimal_hooks_status();
int target_hooks_status();

#endif
