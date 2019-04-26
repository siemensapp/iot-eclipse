/*!**********************************************************************
*
* @copyright Copyright (C) 2016 Siemens Aktiengesellschaft.\n
*            All rights reserved.
*
*************************************************************************
*
* @file     memory.h
* @date     Jul 28, 2016
* @brief    Memory module header file.
*
* This module contains functions and macro definitions for memory allocation/deallocation operations.
*
************************************************************************/

#ifndef MEMORY_H_
#define MEMORY_H_

#include "mcl/mcl_common.h"

/**
 * This function is called to allocate memory. @see #memory_malloc() for more details.
 *
 * @param bytes Size of the space to be allocated.
 * @param function The name of the calling function. Gathered with __FUNCTION__ macro.
 * @param line The line of the calling function. Gathered with __LINE__ macro.
 * @return Pointer to the allocated memory space.
 */
void *memory_allocate(mcl_size_t bytes, const char *function, unsigned line);

/**
 * This function is called to allocate memory and initialize it with zero. @see #memory_calloc() for more details.
 *
 * @param count Count of the object to be created. Total memory space will be (@p count*@p bytes)
 * @param bytes Size of the space to be allocated.
 * @param function The name of the calling function. Gathered with __FUNCTION__ macro.
 * @param line The line of the calling function. Gathered with __LINE__ macro.
 * @return Pointer to the allocated memory space.
 */
void *memory_allocate_with_zero(mcl_size_t count, mcl_size_t bytes, const char *function, unsigned line);

/**
 * This function is called to reallocate memory. @see #memory_realloc() for more details.
 *
 * @param p The pointer to be reallocated.
 * @param bytes Size of the space to be allocated.
 * @param function The name of the calling function. Gathered with __FUNCTION__ macro.
 * @param line The line of the calling function. Gathered with __LINE__ macro.
 * @return Pointer to the reallocated memory space.
 */
void *memory_reallocate(void *p, mcl_size_t bytes, const char *function, unsigned line);

/**
 * This function is called to free memory. @see #memory_free() for more details.
 *
 * @param p The pointer to be freed.
 * @param function The name of the calling function. Gathered with __FUNCTION__ macro.
 * @param line The line of the calling function. Gathered with __LINE__ macro.
 */
void memory_release(void *p, const char *function, unsigned line);

/**
 * @brief malloc wrapper
 *
 * If stddef.h is exists it uses the malloc function. If another implementation will be needed it can be added.
 *
 * @param size Size of the space to be allocated.
 * @return Pointer to the allocated memory space.
 */
void *memory_malloc(mcl_size_t size);

/**
 * @brief calloc wrapper
 *
 * If stddef.h is exists in the system it uses the calloc function. If another implementation will be needed it can be added.
 *
 * @param count Count of the object to be created. Total memory space will be (@p count*@p bytes)
 * @param bytes Size of the space to be allocated.
 * @return Pointer to the allocated memory space.
 */
void *memory_calloc(mcl_size_t count, mcl_size_t bytes);

/**
 * @brief realloc wrapper
 *
 * If stddef.h is exists in the system it uses the realloc function. If another implementation will be needed it can be added.
 *
 * @param p The pointer to be reallocated.
 * @param bytes Size of the space to be allocated.
 * @return Pointer to the reallocated memory space.
 */
void *memory_realloc(void *p, mcl_size_t bytes);

/**
 * @brief free wrapper
 *
 * If stddef.h is exists in the system it uses the free function. If another implementation will be needed it can be added.
 *
 * @param p The pointer to be freed.
 */
void memory_free(void *p);

/*
 * Usage examples for memory macros :
 * char *buf = MCL_MALLOC(100);
 *
 * --
 *
 * struct demo_t demo;
 * MCL_NEW(demo);
 *
 * --
 *
 * buf = MCL_RESIZE(buf, new_size);
 *
 * --
 *
 * MCL_FREE(demo);
 */
#define MCL_MALLOC(bytes) memory_allocate(bytes,  __FUNCTION__, __LINE__)
#define MCL_NEW(p) ((p) = MCL_MALLOC((long)sizeof (*p)))
#define MCL_CALLOC(count, bytes) memory_allocate_with_zero(count, bytes,  __FUNCTION__, __LINE__)
#define MCL_NEW_WITH_ZERO(p) ((p) = MCL_CALLOC(1, (long)sizeof *(p)))
#define MCL_RESIZE(p, bytes) ((p) = memory_reallocate(p, bytes, __FUNCTION__, __LINE__))
#define MCL_FREE(p) ((void)(memory_release((p), __FUNCTION__, (mcl_uint32_t)__LINE__), (p) = MCL_NULL))

#endif //MCL_MEMORY_H_
