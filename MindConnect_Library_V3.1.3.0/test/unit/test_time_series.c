/*!**********************************************************************
 *
 * @copyright Copyright (C) 2016 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 *
 *************************************************************************
 *
 * @file     test_time_series.c
 * @date     Jul 12, 2016
 * @brief    This file implements all test cases of mcl_time_series module.
 *
 ************************************************************************/

#include "time_series.h"
#include "mcl/mcl_time_series.h"
#include "mcl/mcl_common.h"
#include "log_util.h"
#include "data_types.h"
#include "memory.h"
#include "unity.h"
#include "list.h"
#include "random.h"
#include "string_util.h"
#include "string_type.h"
#include "definitions.h"
#include "security.h"
#include "security_libcrypto.h"
#include "time_util.h"

mcl_time_series_t *time_series;
mcl_time_series_value_set_t *value_set;

E_MCL_ERROR_CODE code;

char *timestamp = "2016-04-26T08:06:25.317Z";
char *data_point_id = "e50ab7ca-fd5d-11e5-8000-001b1bc14a1d";
char *value = "65";
char *quality_code = "00000000";
char *data_point_id_2 = "e50ab7ca-fd5d-11e5-8000-001b1bc14a1d";
char *value_2 = "65";
char *quality_code_2 = "00000000";
char *configuration_id = "e3217e2b-7036-49f2-9814-4c38542cd781";
char *payload_version = "1.0";
char *meta_type = "item";
char *payload_type = "standardTimeSeries";
char *routing = "vnd.kuka.FingerprintAnalizer";

void setUp(void)
{
    E_MCL_ERROR_CODE return_code = time_series_initialize(payload_version, configuration_id, routing, &time_series);
    TEST_ASSERT_MESSAGE(MCL_OK == return_code, "Initialization failed for time_series.");
}

void tearDown(void)
{
    time_series_destroy(&time_series);
}

/**
 * GIVEN : Created *time_series.
 * WHEN  : time_series is initialized.
 * THEN  : It gives the initialized time_series data struct.
 */
void test_initialize_001(void)
{
    TEST_ASSERT_NOT_NULL_RETURN(time_series);
    TEST_ASSERT_EQUAL_STRING_MESSAGE(meta_type, time_series->meta.type->buffer, "meta_type fail");
    TEST_ASSERT_EQUAL_STRING_MESSAGE(payload_version, time_series->meta.version->buffer, "meta_version fail");
    TEST_ASSERT_EQUAL_STRING_MESSAGE(payload_type, time_series->meta.payload.type->buffer, "meta_payload_type fail");
    TEST_ASSERT_EQUAL_STRING_MESSAGE(payload_version, time_series->meta.payload.version->buffer, "meta_payload_version fail");
    TEST_ASSERT_EQUAL_STRING_MESSAGE(configuration_id, time_series->meta.payload.details.time_series_details.configuration_id->buffer,
                                     "meta_payload_details_configuration_id fail");
}

/**
 * GIVEN : Initialized time_series.
 * WHEN  : User requests to add a timestamp into time_series.
 * THEN  : Timestamp of time_series is set with the new timestamp.
 */
void test_new_value_set_001(void)
{
    TEST_ASSERT_NOT_NULL_RETURN(time_series);

    code = mcl_time_series_new_value_set(time_series, timestamp, &value_set);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "New payload failed.");

    // Check whether we initialized payload successfully or not.
    TEST_ASSERT_EQUAL_STRING_MESSAGE(timestamp, ((mcl_time_series_value_set_t *)(time_series->payload.value_sets->current->data))->timestamp->buffer, "timestamp fail");
}

/**
 * GIVEN : Initialized time_series.
 * WHEN  : User requests to add a timestamp which has a wrong format into time series.
 * THEN  : MCL_INVALID_PARAMETER is returned.
 */
void test_new_value_set_002(void)
{
    // wrong format T -> X.
    char *timestamp_local = "2016-04-26X08:06:25.317Z";
    code = mcl_time_series_new_value_set(time_series, timestamp_local, &value_set);
    TEST_ASSERT_MESSAGE(MCL_INVALID_PARAMETER == code, "MCL_INVALID_PARAMETER didn't return for wrong format.");
}

/**
 * GIVEN : Initialized time_series.
 * WHEN  : User requests to add a timestamp whose length is wrong.
 * THEN  : MCL_INVALID_PARAMETER is returned.
 */
void test_new_value_set_003(void)
{
    // wrong length 24 -> 23.
    char *timestamp_local = "2016-04-26T08:06:25.31";
    code = mcl_time_series_new_value_set(time_series, timestamp_local, &value_set);
    TEST_ASSERT_MESSAGE(MCL_INVALID_PARAMETER == code, "MCL_INVALID_PARAMETER didn't return for wrong length.");
}

/**
 * GIVEN : Initialized time_series.
 * WHEN  : User requests to add a timestamp which has a wrong value into time_series.
 * THEN  : MCL_INVALID_PARAMETER is returned.
 */
void test_new_value_set_004(void)
{
    // wrong hour value up to 24 -> 25.
    char *timestamp_local = "2016-04-26T25:06:25.317Z";
    code = mcl_time_series_new_value_set(time_series, timestamp_local, &value_set);
    TEST_ASSERT_MESSAGE(MCL_INVALID_PARAMETER == code, "MCL_INVALID_PARAMETER didn't return for wrong time value.");
}

/**
 * GIVEN : Initialized time_series.
 * WHEN  : User requests to add a data_point_id, value and quality_code into time_series.
 * THEN  : time_series is set with the new three parameters.
 */
void test_add_value_001(void)
{
    TEST_ASSERT_NOT_NULL_RETURN(time_series);

    code = mcl_time_series_new_value_set(time_series, timestamp, &value_set);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "New payload failed.");

    // Add value.
    code = mcl_time_series_add_value(value_set, data_point_id, value, quality_code);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Adding value failed.");

    time_series_value_t *value_local = (time_series_value_t *)((time_series_value_set_t *)time_series->payload.value_sets->current->data)->values->current->data;

    TEST_ASSERT_EQUAL_STRING_MESSAGE(data_point_id, value_local->data_point_id->buffer, "data_point_id fail");
    TEST_ASSERT_EQUAL_STRING_MESSAGE(value, value_local->value->buffer, "value fail");
    TEST_ASSERT_EQUAL_STRING_MESSAGE(quality_code, value_local->quality_code->buffer, "quality_code fail");
}

/**
 * GIVEN : Initialized time_series.
 * WHEN  : The value parameter is NULL.
 * THEN  : It warns that value parameter is NULL.
 */
void test_add_value_002(void)
{
    TEST_ASSERT_NOT_NULL_RETURN(time_series);

    MCL_DEBUG("Create new payload.");

    code = mcl_time_series_new_value_set(time_series, timestamp, &value_set);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "New payload failed.");

    code = mcl_time_series_add_value(value_set, data_point_id, MCL_NULL, quality_code);
    TEST_ASSERT_EQUAL_INT32_MESSAGE(MCL_TRIGGERED_WITH_NULL, code, "value should have been NULL but related error code is not received.");
}

/**
 * GIVEN : Initialized time_series.
 * WHEN  : Two value added.
 * THEN  : User expect to see two different value in the value list.
 */
void test_add_value_003(void)
{
    TEST_ASSERT_NOT_NULL_RETURN(time_series);

    // Initialize value_set.
    code = mcl_time_series_new_value_set(time_series, timestamp, &value_set);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "New payload failed.");

    // Add first value.
    code = mcl_time_series_add_value(value_set, data_point_id, value, quality_code);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Adding value failed.");

    // Add second value.
    code = mcl_time_series_add_value(value_set, data_point_id_2, value_2, quality_code_2);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Adding value failed.");

    list_t *values = ((time_series_value_set_t *)time_series->payload.value_sets->current->data)->values;
    list_reset(values);

    // Get the first node.
    time_series_value_t *value_local = (time_series_value_t *)(values->current->data);

    // First value node.
    TEST_ASSERT_EQUAL_STRING_MESSAGE(data_point_id, value_local->data_point_id->buffer, "data_point_id fail");
    TEST_ASSERT_EQUAL_STRING_MESSAGE(value, value_local->value->buffer, "value fail");
    TEST_ASSERT_EQUAL_STRING_MESSAGE(quality_code, value_local->quality_code->buffer, "quality_code fail");

    // Get the second node.
    value_local = ((time_series_value_t *)(list_next(values)->data));

    // Second value node.
    TEST_ASSERT_EQUAL_STRING_MESSAGE(data_point_id_2, value_local->data_point_id->buffer, "data_point_id fail");
    TEST_ASSERT_EQUAL_STRING_MESSAGE(value_2, value_local->value->buffer, "value fail");
    TEST_ASSERT_EQUAL_STRING_MESSAGE(quality_code_2, value_local->quality_code->buffer, "quality_code fail");
}

/**
 * GIVEN : Initialized and set time_series.
 * WHEN  : User requests to destroy time_series.
 * THEN  : User expects the time_series and everything in it have been freed and pointer is NULL
 */
void test_destroy_001(void)
{
    TEST_ASSERT_NOT_NULL_RETURN(time_series);

    // Initialize value_set.
    code = mcl_time_series_new_value_set(time_series, timestamp, &value_set);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "New payload failed.");

    // Add first value.
    code = mcl_time_series_add_value(value_set, data_point_id, value, quality_code);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Adding value failed.");

    // Add second value.
    code = mcl_time_series_add_value(value_set, data_point_id_2, value_2, quality_code_2);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Adding value failed.");

    list_t *values = ((time_series_value_set_t *)time_series->payload.value_sets->current->data)->values;

    TEST_ASSERT_EQUAL_INT_MESSAGE(2, values->count, "Count of values list is not as expected!");

    time_series_destroy(&time_series);

    TEST_ASSERT_NULL_MESSAGE(time_series, "After destroy operation, time_series is not NULL!");
}
