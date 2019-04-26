/*!**********************************************************************
*
* @copyright Copyright (C) 2016 Siemens Aktiengesellschaft.\n
*            All rights reserved.
*
*************************************************************************
*
* @file     test_memory.c
* @date     Jul 29, 2016
* @brief    Unit test cases for testing mcl_mem module.
*
************************************************************************/

#include "mcl/mcl_common.h"
#include "time_series.h"
#include "memory.h"
#include "definitions.h"
#include "log_util.h"
#include "unity.h"
#include "data_types.h"

void setUp(void)
{
}

void tearDown(void)
{
}

/**
 * GIVEN : No initial condition
 * WHEN  : User requests allocation of a char * buffer (MCL_ALLOC).
 * THEN  : User expects the buffer is allocated and is not NULL.
 */
void test_malloc_001(void)
{
    long size = 10;
    char *buf = MCL_MALLOC(size);

    TEST_ASSERT_NOT_NULL_MESSAGE(buf, "New memory couldn't be allocated! Buffer is NULL.");

    MCL_FREE(buf);

    TEST_ASSERT_NULL_MESSAGE(buf, "Pointer is NOT NULL after freeing!");
}

/**
 * GIVEN : A buffer is allocated using MCL_ALLOC.
 * WHEN  : User puts a char to all elements of the created buffer.
 * THEN  : User expects program is not crushed and all elements contains the inserted char value.
 */
void test_malloc_002(void)
{
    long size = 10;
    char *buf = MCL_MALLOC(size);

    TEST_ASSERT_NOT_NULL_MESSAGE(buf, "New memory couldn't be allocated!");

    char test_char = 'a';

    // check if it is really size long
    for (long i = 0; i < size; i++)
    {
        buf[i] = 'a';
    }

    // expecting program not to crush here
    // as a second confirmation, check all chars are really test_char
    for (long i = 0; i < size; i++)
    {
        TEST_ASSERT_EQUAL_INT_MESSAGE(test_char, buf[i], "Char in buf is not as expected!");
    }

    MCL_FREE(buf);

    TEST_ASSERT_NULL_MESSAGE(buf, "Pointer is NOT NULL after freeing!");
}

/**
 * GIVEN : A large buffer is allocated using MCL_ALLOC.
 * WHEN  : User puts a char to all elements of the created buffer.
 * THEN  : User expects program is not crushed and all elements contains the inserted char value.
 */
void test_malloc_003(void)
{
    // test with large size
    long size = 10000;
    char *buf = MCL_MALLOC(size);

    TEST_ASSERT_NOT_NULL_MESSAGE(buf, "New memory couldn't be allocated!");

    char test_char = 'a';

    // check if it is really size long
    for (long i = 0; i < size; i++)
    {
        buf[i] = 'a';
    }

    // expecting program not to crush here
    // as a second confirmation, check all chars are really test_char
    for (long i = 0; i < size; i++)
    {
        TEST_ASSERT_EQUAL_INT_MESSAGE(test_char, buf[i], "Char in buf is not as expected!");
    }

    MCL_FREE(buf);

    TEST_ASSERT_NULL_MESSAGE(buf, "Pointer is NOT NULL after freeing!");
}

/**
 * GIVEN : No initial condition.
 * WHEN  : A buffer is allocated using MCL_NEW. No size is given. Size of given struct is used when allocating.
 * THEN  : User expects the allocated object is not NULL.
 */
void test_new_001(void)
{
    time_series_t *time_series;

    MCL_NEW(time_series);

    TEST_ASSERT_NOT_NULL_MESSAGE(time_series, "New memory couldn't be allocated!");

    MCL_FREE(time_series);

    TEST_ASSERT_NULL_MESSAGE(time_series, "Pointer is NOT NULL after freeing!");
}

/**
 * GIVEN : A new object is created using MCL_NEW.
 * WHEN  : User also allocates memory for a member struct of created object.
 * THEN  : User expects the allocated objects are not NULL and program is not crushed.
 */
void test_new_002(void)
{
    item_meta_t *item_meta;

    MCL_NEW(item_meta);

    TEST_ASSERT_NOT_NULL_MESSAGE(item_meta, "New memory couldn't be allocated!");

    MCL_NEW(item_meta->content_id);

    TEST_ASSERT_NOT_NULL_MESSAGE(item_meta->content_id, "New memory couldn't be allocated for content_id!");

    MCL_FREE(item_meta->content_id);

    TEST_ASSERT_NULL_MESSAGE(item_meta->content_id, "content_id Pointer is NOT NULL after freeing!");

    MCL_FREE(item_meta);

    TEST_ASSERT_NULL_MESSAGE(item_meta, "Time_series Pointer is NOT NULL after freeing!");
}

/**
 * GIVEN : No initial condition.
 * WHEN  : A new object is created using MCL_CALLOC with a given size.
 * THEN  : User expects created object pointer is not NULL and all bytes set to 0.
 */
void test_calloc_001(void)
{
    long size = 10;
    char *buf = MCL_CALLOC(1, size);

    TEST_ASSERT_NOT_NULL_MESSAGE(buf, "New memory couldn't be allocated! Buffer is NULL.");

    for (int i = 0; i < size; i++)
    {
        TEST_ASSERT_EQUAL_INT32_MESSAGE(0, buf[i], "Initialized buffer with MCL_CALLOC content is not zero(0)!");
    }

    MCL_FREE(buf);

    TEST_ASSERT_NULL_MESSAGE(buf, "Pointer is NOT NULL after freeing!");
}

/**
 * GIVEN : A new buffer is allocated using MCL_ALLOC with size 'first_size'. Content set.
 * WHEN  : User requests MCL_RESIZE() with 'second_size' to change the size of the allocated block. 'first_size' < 'second_size'
 * THEN  : User expects all content in the new block is same and able to add more chars to the buffer.
 */
void test_realloc_001(void)
{
    int first_size = 10;
    int second_size = 20;
    char base_char = 'a';
    char first_char = base_char;
    char second_char = base_char;

    char *buf = MCL_MALLOC(first_size);

    TEST_ASSERT_NOT_NULL_MESSAGE(buf, "New memory couldn't be allocated!");

    for (int i = 0; i < first_size; i++)
    {
        buf[i] = first_char++;
    }

    buf = MCL_RESIZE(buf, second_size);

    TEST_ASSERT_NOT_NULL_MESSAGE(buf, "Buffer is NULL after re-allocation operation!");

    // check if first 'first_size' char values are still the same :
    for (int i = 0; i < first_size; i++)
    {
        TEST_ASSERT_EQUAL_INT32_MESSAGE(buf[i], second_char++, "Buffers content is not as expected after resize operation!");
    }

    // try to add more chars in buffer :
    for (int i = first_size; i < second_size; i++)
    {
        buf[i] = first_char++;
    }

    // if we are still here it's good but to be sure check the other chars :
    for (int i = first_size; i < second_size; i++)
    {
        TEST_ASSERT_EQUAL_INT32_MESSAGE(buf[i], second_char++, "Buffers content is not as expected after resize operation!");
    }

    MCL_FREE(buf);
    TEST_ASSERT_NULL_MESSAGE(buf, "Buffer is NOT NULL after freeing!");
}

/**
 * GIVEN : No initial condition.
 * WHEN  : A new object is created using MCL_NEW_WITH_ZERO.
 * THEN  : User expects created object pointer is not NULL and all bytes set to 0.
 */
void test_new_with_zero_001(void)
{
    typedef struct demo_t
    {
        int i1;
        int i2;
        int i3;
    } demo_t;

    demo_t *demo;

    MCL_NEW_WITH_ZERO(demo);

    TEST_ASSERT_NOT_NULL_MESSAGE(demo, "New memory couldn't be allocated!");

    TEST_ASSERT_EQUAL_INT32_MESSAGE(0, demo->i1, "1. field of created struct is not 0!");
    TEST_ASSERT_EQUAL_INT32_MESSAGE(0, demo->i2, "2. field of created struct is not 0!");
    TEST_ASSERT_EQUAL_INT32_MESSAGE(0, demo->i3, "3. field of created struct is not 0!");

    MCL_FREE(demo);

    TEST_ASSERT_NULL_MESSAGE(demo, "Pointer is NOT NULL after freeing!");
}
