/*!**********************************************************************
 *
 * @copyright Copyright (C) 2016 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 *
 *************************************************************************
 *
 * @file     test_data_types.c
 * @date     Sep 19, 2016
 * @brief    This file implements all test cases of data types module.
 *
 ************************************************************************/

#include "data_types.h"
#include "unity.h"
#include "log_util.h"
#include "string_util.h"

void setUp(void)
{
}

void tearDown(void)
{
}

// Private Function Prototypes:
static void _check_string_length(string_t *string_array, mcl_size_t count_of_array_elements);

/**
 * GIVEN : #string_t array.
 * WHEN  : The length of array.buffer is equal to array.length for each string.
 * THEN  : It succeeds.
 */
void test_data_types_001(void)
{
    //Contains element counts of each string arrays we have in data_types.c
    mcl_size_t element_counts_of_string_arrays[] = { META_FIELD_NAMES_END, META_FIELD_VALUES_END, PAYLOAD_FIELD_NAMES_END, PAYLOAD_FIELD_VALUES_END, CONTENT_TYPE_VALUES_END};

    //To get the string arrays in for loop, keep their address in this array.
    string_t *string_arrays[] = {meta_field_names, meta_field_values, payload_field_names, payload_field_values, content_type_values};

    //When new string arrays created, total count of string arrays is updated automatically here.
    mcl_size_t total_count_of_string_arrays = sizeof(element_counts_of_string_arrays) / sizeof(element_counts_of_string_arrays[0]);

    //Index for string arrays.
    mcl_size_t index_of_string_array;
    for (index_of_string_array = 0; index_of_string_array < total_count_of_string_arrays; index_of_string_array++)
    {
        _check_string_length(string_arrays[index_of_string_array], element_counts_of_string_arrays[index_of_string_array]);
    }
}

static void _check_string_length(string_t *string_array, mcl_size_t count_of_array_elements)
{
    mcl_size_t index;

    // Compare calculated length of each string_array.buffer with string_array.length.
    for (index = 0; index < count_of_array_elements; index++)
    {
        mcl_size_t string_length;
        string_length = string_util_strlen(string_array[index].buffer);

        // This check is printing the string which fails.
        if (string_array[index].length != string_length)
        {
            MCL_DEBUG("String length failed for <%s> in the string array whose element count = <%d>.", string_array[index].buffer, count_of_array_elements);
            TEST_ASSERT_EQUAL_INT32_MESSAGE(string_array[index].length, string_length, "String length failed.");
        }
    }
}
