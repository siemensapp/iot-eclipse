/*!**********************************************************************
*
* @copyright Copyright (C) 2016 Siemens Aktiengesellschaft.\n
*            All rights reserved.
*
*************************************************************************
*
* @file     test_random.c
* @date     Sep 2, 2016
* @brief    This file contains test case functions to test random module.
*
************************************************************************/

#include "mcl_random.h"
#include "random.h"
#include "memory.h"
#include "unity.h"
#include "string_type.h"
#include "definitions.h"
#include "security.h"
#include "security_libcrypto.h"
#include "string_util.h"

void setUp(void)
{
}

void tearDown(void)
{
}

/**
 * GIVEN : No initial condition
 * WHEN  : Random number generation requested from mcl_random module.
 * THEN  : MCL_OK is returned.
 */
void test_generate_number_001()
{
    mcl_uint32_t random_number;
    E_MCL_ERROR_CODE code = random_generate_number(&random_number);

    TEST_ASSERT_EQUAL(MCL_OK, code);
}

/**
 * GIVEN : No initial condition
 * WHEN  : 10 random number generation requested from mcl_random module.
 * THEN  : Expecting each random number is different than the previous one.
 */
void test_generate_number_002()
{
    mcl_uint32_t random_number_old = 0;
    mcl_size_t iterate_count = 10;

    for (mcl_size_t i = 0; i < iterate_count; i++)
    {
        mcl_uint32_t random_number;
        E_MCL_ERROR_CODE code = random_generate_number(&random_number);
        TEST_ASSERT_EQUAL(MCL_OK, code);
        TEST_ASSERT_NOT_EQUAL(random_number_old, random_number);
        random_number_old = random_number;
    }
}

/**
 * GIVEN : No initial condition
 * WHEN  : 10000 random number generation requested from mcl_random module.
 * THEN  : Expecting each random number is different than the previous one.
 */
void test_generate_number_003()
{
    mcl_uint32_t random_number_old = 0;
    mcl_size_t iterate_count = 10000;
    int retry_count = 2;

    for (mcl_size_t i = 0; i < iterate_count; i++)
    {
        mcl_uint32_t random_number;
        random_generate_number(&random_number);
        if (random_number_old == random_number)
        {
            TEST_ASSERT_NOT_EQUAL_MESSAGE(0, retry_count--, "To much consecutive same number is generated!");
        }
        random_number_old = random_number;
    }
}

/**
 * GIVEN : No initial condition
 * WHEN  : A random array generation is requested from mcl_random module with size of 1000.
 * THEN  : Expecting same items in array is under acceptable limit which is %0.03.
 */
void test_generate_array_001()
{
    mcl_uint8_t *random_array = 0;
    const mcl_size_t array_size = 1000;
    mcl_uint8_t duplicate_array[1000];
    mcl_size_t duplicate_count = 0;

    random_array = random_generate_array(array_size);
    for (mcl_size_t i = 0; i < array_size; i++)
    {
        for (mcl_size_t j = i + 1; j < array_size; j++)
        {
            if (random_array[i] == random_array[j])
            {
                // if not already added to the list, add :
                mcl_bool_t already_added = MCL_FALSE;
                for (int k = 0; k < duplicate_count; k++)
                {
                    if (duplicate_array[k] == random_array[i])
                    {
                        already_added = MCL_TRUE;
                        break;
                    }
                }
                if (!already_added)
                {
                    duplicate_array[duplicate_count] = random_array[i];
                    duplicate_count++;
                }
            }
        }
    }

    // check the duplication ratio:
    if (duplicate_count != 0)
    {
        double duplicate_ratio = 1.0 * duplicate_count / array_size;
		double duplicate_limit = 0.3;

        if (duplicate_ratio > duplicate_limit)
        {
            TEST_FAIL_MESSAGE("Too much duplication in random array!");
        }
    }

    MCL_FREE(random_array);
}

/**
 * GIVEN : No initial condition
 * WHEN  : Two random array generation is requested from mcl_random module with size of 128.
 * THEN  : Expecting two array has different values.
 */
void test_generate_array_002()
{
    mcl_uint8_t *random_array_1 = 0;
    mcl_uint8_t *random_array_2 = 0;
    mcl_size_t array_size = 128;

    random_array_1 = random_generate_array(array_size);
    random_array_2 = random_generate_array(array_size);

    mcl_bool_t same = MCL_TRUE;
    for (mcl_size_t i = 0; i < array_size; i++)
    {
        if (random_array_1[i] != random_array_2[i])
        {
            same = MCL_FALSE;

            break;
        }
    }

    TEST_ASSERT_EQUAL_MESSAGE(MCL_FALSE, same, "Two random array has the same content!");

    MCL_FREE(random_array_1);
    MCL_FREE(random_array_2);
}

/**
 * GIVEN : No initial condition
 * WHEN  : Random number generation requested by calling mcl_random_generate_guid().
 * THEN  : MCL_OK is returned.
 */
void test_generate_guid_001()
{
    char *guid = MCL_NULL;
    E_MCL_ERROR_CODE code = mcl_random_generate_guid(&guid);

    MCL_FREE(guid);

    TEST_ASSERT_EQUAL(MCL_OK, code);
}
