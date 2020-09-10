/* Copyright (c) 2018 Qualcomm Technologies International, Ltd. */
/* Part of 6.3.2 */
/*!
  @file ama_pmalloc_pools.c
  @brief Creation of pmalloc pools used in AMA configs only.
*/

#include "ama_private.h"

#include <pmalloc.h>

_Pragma ("unitsuppress Unused")

_Pragma("datasection apppool") static const pmalloc_pool_config app_pools[] =
{
    {512, 4}
};

/* The linker will discard any files in an archive library that do not contain
 * an externally-referenced symbol. This function is used to guarantee that the 
 * architecture-specific app_pools structure is included in the final executable
 * when needed.
 */
void ama_pmalloc_init(void)
{
    /* Do nothing */
}
