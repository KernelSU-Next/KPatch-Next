/* SPDX-License-Identifier: GPL-2.0-or-later */
/* 
 * Copyright (C) 2023 bmax121. All Rights Reserved.
 */

#ifndef _KPU_KPATCH_H_
#define _KPU_KPATCH_H_

#include <stdint.h>
#include <unistd.h>
#include "../version"

#ifdef __cplusplus
extern "C"
{
#endif

    uint32_t version();

    void hello();
    void kpv();
    void kv();

    void bootlog();
    void panic();

#ifdef __cplusplus
}
#endif

#endif
