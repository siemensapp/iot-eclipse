/*!**********************************************************************
*
* @copyright Copyright (C) 2016 Siemens Aktiengesellschaft.\n
*            All rights reserved.
*
*************************************************************************
*
* @file     test_list.c
* @date     Jul 27, 2016
* @brief    Unit test cases for testing mcl_list_t module.
*
************************************************************************/

#include "mcl/mcl_common.h"
#include "list.h"
#include "mcl/mcl_list.h"
#include "string_type.h"
#include "memory.h"
#include "data_types.h"
#include "definitions.h"
#include "unity.h"
#include "string_util.h"

mcl_list_t *list;

void setUp(void)
{
    mcl_list_initialize(&list);
}

void tearDown(void)
{
    mcl_list_destroy(&list);
}

/**
 * GIVEN : User requests initialization for mcl_list_t.
 * WHEN  : mcl_list_t initialized.
 * THEN  : mcl_list_t is initialized and is not NULL.
 */
void test_initialize_001(void)
{
    TEST_ASSERT_NOT_NULL_MESSAGE(list, "mcl_list_t couldn't be initialized!");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, list->count, "Count of list is not 0!");
    TEST_ASSERT_NULL_MESSAGE(list->head, "head node of list is not NULL!");
    TEST_ASSERT_NULL_MESSAGE(list->last, "last node of list is not NULL!");
    TEST_ASSERT_NULL_MESSAGE(list->current, "last node of list is not NULL!");
}

/**
 * GIVEN : User requests initialization for mcl_list_t.
 * WHEN  : mcl_list_t initialized. mcl_list_next() called on empty list.
 * THEN  : received node is NULL.
 */
void test_next_001(void)
{
    TEST_ASSERT_NOT_NULL_MESSAGE(list, "mcl_list_t couldn't be initialized!");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, list->count, "Count of list is not 0!");

    mcl_list_node_t *node;
    E_MCL_ERROR_CODE code = mcl_list_next(list, &node);

    TEST_ASSERT_EQUAL(MCL_FAIL, code);
    TEST_ASSERT_NULL_MESSAGE(node, "called mcl_list_next on empty list but received not NULL node!");

    // check everything is still NULL :
    TEST_ASSERT_NULL_MESSAGE(list->head, "head node of list is not NULL !");
    TEST_ASSERT_NULL_MESSAGE(list->last, "last node of list is not NULL !");
    TEST_ASSERT_NULL_MESSAGE(list->current, "last node of list is not NULL !");
}

/**
 * GIVEN : mcl_list_t is initialized.
 * WHEN  : user adds a new data to the list.
 * THEN  : user gets new added data with next and compares with the set data
 */
void test_add_001(void)
{
    TEST_ASSERT_NOT_NULL_MESSAGE(list, "mcl_list_t couldn't be initialized!");

    time_series_value_t value;

    string_initialize_static("1", 0, &value.data_point_id);
    string_initialize_static("2", 0, &value.quality_code);
    string_initialize_static("3", 0, &value.value);

    E_MCL_ERROR_CODE ret = mcl_list_add(list, &value);

    TEST_ASSERT_EQUAL_INT_MESSAGE(0, ret, "Add operation resulted with error!");
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, list->count, "Count of list is not 1!");

    mcl_list_node_t *next_node = MCL_NULL;
    ret = mcl_list_next(list, &next_node);
    TEST_ASSERT_EQUAL(MCL_OK, ret);

    TEST_ASSERT_NOT_NULL_MESSAGE(next_node, "next node received as NULL!");
    TEST_ASSERT_NOT_NULL_MESSAGE(next_node->data, "data in received next node is NULL!");

    time_series_value_t *value_from_list = (time_series_value_t *)next_node->data;

    TEST_ASSERT_EQUAL_STRING_MESSAGE("1", value_from_list->data_point_id->buffer, "Received data_point_id is not as expected!");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("2", value_from_list->quality_code->buffer, "Received quality_code is not as expected!");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("3", value_from_list->value->buffer, "Received value is not as expected!");

    string_destroy(&value.data_point_id);
    string_destroy(&value.quality_code);
    string_destroy(&value.value);
}

/**
 * GIVEN : mcl_list_t is initialized.
 * WHEN  : user adds multiple new data to the list.
 * THEN  : user gets the count of list and compares with the add count
 */
void test_add_002(void)
{
    TEST_ASSERT_NOT_NULL_MESSAGE(list, "mcl_list_t couldn't be initialized!");

    time_series_value_t value;

    string_initialize_static("1", 0, &value.data_point_id);
    string_initialize_static("2", 0, &value.quality_code);
    string_initialize_static("3", 0, &value.value);

    int count = 10;

    for (int i = 0; i < count; i++)
    {
        E_MCL_ERROR_CODE ret = mcl_list_add(list, &value);

        TEST_ASSERT_EQUAL_INT_MESSAGE(0, ret, "Add operation resulted with error!");
    }

    TEST_ASSERT_EQUAL_INT_MESSAGE(count, list->count, "Count of list is not as expected!");

    string_destroy(&value.data_point_id);
    string_destroy(&value.quality_code);
    string_destroy(&value.value);
}

/**
 * GIVEN : mcl_list_t is initialized.
 * WHEN  : user adds a new data to the list. then removes  the added data.
 * THEN  : user checks the remove operation result and list count
 */
void test_remove_001(void)
{
    TEST_ASSERT_NOT_NULL_MESSAGE(list, "mcl_list_t couldn't be initialized!");

    time_series_value_t value;

    string_initialize_static("1", 0, &value.data_point_id);
    string_initialize_static("2", 0, &value.quality_code);
    string_initialize_static("3", 0, &value.value);

    E_MCL_ERROR_CODE ret = mcl_list_add(list, &value);

    TEST_ASSERT_EQUAL_INT_MESSAGE(0, ret, "Add operation resulted with error!");
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, list->count, "Count of list is not 1!");

    mcl_list_node_t *next_node;
    ret = mcl_list_next(list, &next_node);

    TEST_ASSERT_EQUAL(MCL_OK, ret);
    TEST_ASSERT_NOT_NULL_MESSAGE(next_node, "next node received as NULL!");
    TEST_ASSERT_NOT_NULL_MESSAGE(next_node->data, "data in received next node is NULL!");

    ret = mcl_list_remove(list, next_node);
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, ret, "Remove operation resulted with error!");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, list->count, "Count of list is not 0!");

    string_destroy(&value.data_point_id);
    string_destroy(&value.quality_code);
    string_destroy(&value.value);
}

/**
 * GIVEN : mcl_list_t is initialized.
 * WHEN  : user adds a new data to the list. then removes  the added data.
 * THEN  : user cheks the remove operation result and list count and checks removed node's data still accessible
 */
void test_remove_002(void)
{
    TEST_ASSERT_NOT_NULL_MESSAGE(list, "mcl_list_t couldn't be initialized!");

    time_series_value_t value;

    string_initialize_static("1", 0, &value.data_point_id);
    string_initialize_static("2", 0, &value.quality_code);
    string_initialize_static("3", 0, &value.value);

    E_MCL_ERROR_CODE ret = mcl_list_add(list, &value);

    TEST_ASSERT_EQUAL_INT_MESSAGE(0, ret, "Add operation resulted with error!");
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, list->count, "Count of list is not 1!");

    mcl_list_node_t *next_node = MCL_NULL;
    mcl_list_next(list, &next_node);

    TEST_ASSERT_NOT_NULL_MESSAGE(next_node, "next node received as NULL!");
    TEST_ASSERT_NOT_NULL_MESSAGE(next_node->data, "data in received next node is NULL!");

    ret = mcl_list_remove(list, next_node);
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, ret, "Remove operation resulted with error!");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, list->count, "Count of list is not 1!");

    // TEST_ASSERT_EQUAL_STRING_MESSAGE("1", )
    TEST_ASSERT_EQUAL_STRING_MESSAGE("1", value.data_point_id->buffer, "Received data_point_id is not as expected!");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("2", value.quality_code->buffer, "Received quality_code is not as expected!");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("3", value.value->buffer, "Received value is not as expected!");

    string_destroy(&value.data_point_id);
    string_destroy(&value.quality_code);
    string_destroy(&value.value);
}

/**
 * GIVEN : mcl_list_t is initialized.
 * WHEN  : user adds multiple new data to the list.
 * THEN  : user removes each item and checks the result. also checks removed item count to be same as added item count.
 */
void test_remove_003(void)
{
    TEST_ASSERT_NOT_NULL_MESSAGE(list, "mcl_list_t couldn't be initialized!");

    time_series_value_t value;

    string_initialize_static("1", 0, &value.data_point_id);
    string_initialize_static("2", 0, &value.quality_code);
    string_initialize_static("3", 0, &value.value);

    int count = 10;
    E_MCL_ERROR_CODE ret;

    for (int i = 0; i < count; i++)
    {
        ret = mcl_list_add(list, &value);

        TEST_ASSERT_EQUAL_INT_MESSAGE(0, ret, "Add operation resulted with error!");
    }
    TEST_ASSERT_EQUAL_INT_MESSAGE(count, list->count, "Count of list is not as expected!");

    mcl_list_node_t *next_node = MCL_NULL;
    int index = 0;

    mcl_list_next(list, &next_node);
    while (MCL_NULL != next_node)
    {
        TEST_ASSERT_NOT_NULL_MESSAGE(next_node, "next node received as NULL!");
        TEST_ASSERT_NOT_NULL_MESSAGE(next_node->data, "data in received next node is NULL!");

        ret = mcl_list_remove(list, next_node);
        TEST_ASSERT_EQUAL_INT_MESSAGE(0, ret, "Remove operation resulted with error!");

        index++;
        TEST_ASSERT_TRUE_MESSAGE(index <= count, "Removed more item than added!");
        mcl_list_next(list, &next_node);
    }

    TEST_ASSERT_EQUAL_INT_MESSAGE(0, list->count, "After all remove operations, count of list is not 0!");

    string_destroy(&value.data_point_id);
    string_destroy(&value.quality_code);
    string_destroy(&value.value);
}

/**
 * GIVEN : mcl_list_t is initialized.
 * WHEN  : user adds a new data to the list. then removes  the added data.
 * THEN  : user checks the remove operation result and list count
 */
void test_remove_with_content_001(void)
{
    TEST_ASSERT_NOT_NULL_MESSAGE(list, "mcl_list_t couldn't be initialized!");

    mcl_list_t *list_as_node;
    E_MCL_ERROR_CODE ret = mcl_list_initialize(&list_as_node);

    TEST_ASSERT_EQUAL(MCL_OK, ret);

    ret = mcl_list_add(list, list_as_node);

    TEST_ASSERT_EQUAL(MCL_OK, ret);
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, ret, "Add operation resulted with error!");
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, list->count, "Count of list is not 1!");

    mcl_list_node_t *next_node;
    ret = mcl_list_next(list, &next_node);

    TEST_ASSERT_EQUAL(MCL_OK, ret);
    TEST_ASSERT_NOT_NULL_MESSAGE(next_node, "next node received as NULL!");
    TEST_ASSERT_NOT_NULL_MESSAGE(next_node->data, "data in received next node is NULL!");

    ret = mcl_list_remove_with_content(list, next_node, (mcl_list_item_destroy_callback)mcl_list_destroy);
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, ret, "Remove operation resulted with error!");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, list->count, "Count of list is not 0!");
}

/**
 * GIVEN : mcl_list_t is initialized.
 * WHEN  : User resets the list.
 * THEN  : Current node becomes head node.
 */
void test_reset_001(void)
{
    TEST_ASSERT_NOT_NULL_MESSAGE(list, "mcl_list_t couldn't be initialized!");

    time_series_value_t value;

    string_initialize_static("1", 0, &value.data_point_id);
    string_initialize_static("2", 0, &value.quality_code);
    string_initialize_static("3", 0, &value.value);

    int count = 10;

    for (int i = 0; i < count; i++)
    {
        E_MCL_ERROR_CODE ret = mcl_list_add(list, &value);

        TEST_ASSERT_EQUAL_INT_MESSAGE(0, ret, "Add operation resulted with error!");
    }

    // Before reset the list, verify current node is not the head node.
    TEST_ASSERT_MESSAGE(list->head == list->current, "Current node shouldn't have been head node.");
    mcl_list_reset(list);

    // After reset the list, verify current node is the head node.
    TEST_ASSERT_EQUAL_PTR_MESSAGE(list->head, list->current, "List's current node couldn't be reset to head node.");

    string_destroy(&value.data_point_id);
    string_destroy(&value.quality_code);
    string_destroy(&value.value);
}

/**
 * GIVEN : mcl_list_t is initialized.
 * WHEN  : The item is in the list.
 * THEN  : mcl_list_exist() returns MCL_OK and node_data_of_item is the node data where the item exists.
 */
void test_exist_001(void)
{
    time_series_value_t value;

    string_initialize_static("1", 0, &value.data_point_id);
    string_initialize_static("2", 0, &value.quality_code);
    string_initialize_static("3", 0, &value.value);

    mcl_list_add(list, value.data_point_id);
    mcl_list_add(list, value.quality_code);
    mcl_list_add(list, value.value);

    void *node_data_of_item = MCL_NULL;
    E_MCL_ERROR_CODE group_id_exists = mcl_list_exist(list, value.data_point_id, (mcl_list_compare_callback)string_compare, &node_data_of_item);
    TEST_ASSERT_EQUAL_INT_MESSAGE(MCL_OK, group_id_exists, "mcl_list_exist operation resulted with error!");

    TEST_ASSERT_EQUAL_STRING_MESSAGE(value.data_point_id->buffer, ((string_t *)node_data_of_item)->buffer, "Received data_point_id is not as expected!");

    string_destroy(&value.data_point_id);
    string_destroy(&value.quality_code);
    string_destroy(&value.value);
}

/**
 * GIVEN : mcl_list_t is initialized.
 * WHEN  : The item is not in the list.
 * THEN  : mcl_list_exist() returns MCL_FAIL and node_data_of_item becomes null.
 */
void test_exist_002(void)
{
    time_series_value_t value;

    string_initialize_static("1", 0, &value.data_point_id);
    string_initialize_static("2", 0, &value.quality_code);
    string_initialize_static("3", 0, &value.value);

    mcl_list_add(list, value.data_point_id);
    mcl_list_add(list, value.quality_code);

    void *node_data_of_item = MCL_NULL;
    E_MCL_ERROR_CODE group_id_exists = mcl_list_exist(list, value.value, (mcl_list_compare_callback)string_compare, &node_data_of_item);
    TEST_ASSERT_EQUAL_INT_MESSAGE(MCL_FAIL, group_id_exists, "mcl_list_exist operation didn't return MCL_FAIL!");

    TEST_ASSERT_NULL_MESSAGE(node_data_of_item, "node_data_of_item is not null.");

    string_destroy(&value.data_point_id);
    string_destroy(&value.quality_code);
    string_destroy(&value.value);
}

/**
 * GIVEN : mcl_list_t is initialized.
 * WHEN  : user adds multiple new data to the list.
 * THEN  : user calls destroy operation of list and expects the list has been freed and pointer is NULL.
 */
void test_destroy_001(void)
{
    TEST_ASSERT_NOT_NULL_MESSAGE(list, "mcl_list_t couldn't be initialized!");

    time_series_value_t value;

    string_initialize_static("1", 0, &value.data_point_id);
    string_initialize_static("2", 0, &value.quality_code);
    string_initialize_static("3", 0, &value.value);

    int count = 10;

    for (int i = 0; i < count; i++)
    {
        E_MCL_ERROR_CODE ret = mcl_list_add(list, &value);

        TEST_ASSERT_EQUAL_INT_MESSAGE(0, ret, "Add operation resulted with error!");
    }
    TEST_ASSERT_EQUAL_INT_MESSAGE(count, list->count, "Count of list is not as expected!");

    mcl_list_destroy(&list);
    TEST_ASSERT_NULL_MESSAGE(list, "After destroy operation, list is not NULL!");

    string_destroy(&value.data_point_id);
    string_destroy(&value.quality_code);
    string_destroy(&value.value);
}

/**
 * GIVEN : mcl_list_t is initialized.
 * WHEN  : user adds multiple list to the list.
 * THEN  : user calls destroy operation of list and expects the list has been freed and pointer is NULL.
 */
void test_destroy_with_content_001(void)
{
    mcl_list_t *list_1 = MCL_NULL;
    mcl_list_t *list_2 = MCL_NULL;
    mcl_list_t *list_of_lists = MCL_NULL;

    E_MCL_ERROR_CODE code = mcl_list_initialize(&list_1);
    TEST_ASSERT_EQUAL(MCL_OK, code);
    TEST_ASSERT_NOT_NULL(list_1);

    code = mcl_list_initialize(&list_2);
    TEST_ASSERT_EQUAL(MCL_OK, code);
    TEST_ASSERT_NOT_NULL(list_2);

    code = mcl_list_initialize(&list_of_lists);
    TEST_ASSERT_EQUAL(MCL_OK, code);
    TEST_ASSERT_NOT_NULL(list_of_lists);

    code = mcl_list_add(list_of_lists, list_1);
    TEST_ASSERT_EQUAL(MCL_OK, code);

    code = mcl_list_add(list_of_lists, list_2);
    TEST_ASSERT_EQUAL(MCL_OK, code);
    TEST_ASSERT_EQUAL(2, list_of_lists->count);

    mcl_list_destroy_with_content(&list_of_lists, (mcl_list_item_destroy_callback)mcl_list_destroy);
    TEST_ASSERT_NULL(list_of_lists);
}
