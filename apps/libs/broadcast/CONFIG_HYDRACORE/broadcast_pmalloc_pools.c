/* Copyright (c) 2018 Qualcomm Technologies International, Ltd. */
/* Part of 6.3.2 */
/*!
  @file broadcast_pmalloc_pools.c
  @brief Creation of pmalloc pools used in Hydracore BA configs only.
*/

#include "broadcast_private.h"

#include <pmalloc.h>

_Pragma ("unitsuppress Unused")

_Pragma("datasection apppool") static const pmalloc_pool_config app_pools[] =
{
    {2048, 1}
};

/* The linker will discard any files in an archive library that do not contain
 * an externally-referenced symbol. This function is used to guarantee that the 
 * architecture-specific app_pools structure is included in the final executable
 * when needed.
 */
void broadcast_pmalloc_init(void)
{
    /* Do nothing */
}
