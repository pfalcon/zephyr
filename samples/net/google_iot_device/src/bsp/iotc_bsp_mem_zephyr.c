/* Copyright 2019 Google LLC
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <iotc_bsp_mem.h>
#include <kernel.h>

// https://docs.zephyrproject.org/1.10.0/kernel/memory/pools.html
K_MEM_POOL_DEFINE(iotc_mem_pool, 128, 32768, 16, 4);

void* iotc_bsp_mem_alloc(size_t byte_count) {
  void* ret = (void*)k_mem_pool_malloc(&iotc_mem_pool, byte_count);
  return ret;
}

void* iotc_bsp_mem_realloc(void* ptr, size_t byte_count) {
  (void)ptr;
  (void)byte_count;
  /* not implemented */
  return NULL;
}

void iotc_bsp_mem_free(void* ptr) { k_free(ptr); }
