/*!**********************************************************************
 *
 * @copyright Copyright (C) 2016 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 *
 *************************************************************************
 *
 * @file     test_store.c
 * @date     Oct 3, 2016
 * @brief    Test module for testing functions wich operates on a MCL store.
 *
 ************************************************************************/

#include "unity.h"
#include "mock_mcl_time_series.h"
#include "mock_time_series.h"
#include "mock_mcl_custom_data.h"
#include "mock_custom_data.h"
#include "mock_stream_data.h"
#include "mock_event.h"
#include "mock_event_list.h"
#include "mock_file.h"
#include "store.h"
#include "definitions.h"
#include "memory.h"
#include "list.h"
#include "string_type.h"
#include "string_util.h"
#include "data_types.h"
#include "mcl/mcl_store.h"
#include "time_util.h"

char *type = "customType";
char *version = "1.0";
char *routing = "vnd.kuka.FingerprintAnalizer";
const char *event_payload_type = "thresholdViolation";
const char *event_payload_version = "1.0.1";
const char *timestamp = "2016-04-26T08:06:25.317Z";
E_MCL_EVENT_SEVERITY severity = MCL_EVENT_SEVERITY_ERROR;

void setUp(void)
{
}

void tearDown(void)
{
}

mcl_size_t stream_data_read_callback(void *destination, void *source, mcl_size_t size, void *user_context)
{
    return size;
}

/**
 * GIVEN : No special condition.
 * WHEN  : Initialization of store function called.
 * THEN  : Store must be created and MCL_OK must be returned.
 */
void test_initialize_001()
{
    // call test function
    mcl_store_t *store = MCL_NULL;
    E_MCL_ERROR_CODE code = mcl_store_initialize(MCL_FALSE, &store);

    TEST_ASSERT_MESSAGE(MCL_OK == code, "Expected code MCL_OK not returned from mcl_store_initialize()!");
    TEST_ASSERT_NOT_NULL_MESSAGE(store, "Store must point to allocated memory!");

    mcl_store_destroy(&store);
}

/**
 * GIVEN : Output parameter for store is given as NULL.
 * WHEN  : Initialization of store function called.
 * THEN  : MCL_TRIGGERED_WITH_NULL_HANDLE must be returned.
 */
void test_initialize_002()
{
    // call test function
    E_MCL_ERROR_CODE code = mcl_store_initialize(MCL_FALSE, MCL_NULL);

    TEST_ASSERT_MESSAGE(MCL_TRIGGERED_WITH_NULL == code, "Expected code not returned from mcl_store_initialize()!");
}

/**
 * GIVEN : Store is initialized.
 * WHEN  : Adding a new custom data to store is called.
 * THEN  : MCL_OK must be returned.
 */
void test_new_custom_data_001()
{
    mcl_custom_data_t *custom_data = MCL_NULL;
    MCL_NEW(custom_data);

    // Set Fill meta_payload_type.
    string_initialize_new(type, 0, &(custom_data->meta.payload.type));

    // Set meta.payload.version.
    string_initialize_new(version, 0, &(custom_data->meta.payload.version));

    // Set up mocks.
    custom_data_initialize_ExpectAnyArgsAndReturn(MCL_OK);
    custom_data_initialize_ReturnThruPtr_custom_data(&custom_data);

    // initialize store
    mcl_store_t *store = MCL_NULL;
    mcl_store_initialize(MCL_FALSE, &store);

    mcl_custom_data_t *new_custom_data = MCL_NULL;

    // call test function
    E_MCL_ERROR_CODE code = mcl_store_new_custom_data(store, version, type, routing, &new_custom_data);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Expected code not returned from mcl_store_new_custom_data()!");

    // Check if custom_data is added successfully or not.
    TEST_ASSERT_EQUAL_STRING_MESSAGE(version, ((mcl_custom_data_t *)((store_data_t *)store->high_priority_list->current->data)->data)->meta.payload.version->buffer,
                                     "version is wrong.");

    // Clean up.
    string_destroy(&((mcl_custom_data_t *)((store_data_t *)store->high_priority_list->current->data)->data)->meta.payload.version);
    string_destroy(&((mcl_custom_data_t *)((store_data_t *)store->high_priority_list->current->data)->data)->meta.payload.type);
    MCL_FREE(custom_data);
    custom_data_destroy_Ignore();
    mcl_store_destroy(&store);
}

/**
 * GIVEN : Store is initialized. Mock for creating a new custom data returns error code.
 * WHEN  : Adding a new custom data to store is called.
 * THEN  : MCL_FAIL must be returned.
 */
void test_new_custom_data_002()
{
    // setup mocks
    custom_data_initialize_IgnoreAndReturn(MCL_FAIL);

    // initialize store
    mcl_store_t *store = MCL_NULL;
    mcl_store_initialize(MCL_FALSE, &store);

    // call test function
    mcl_custom_data_t *custom_data = MCL_NULL;
    E_MCL_ERROR_CODE code = mcl_store_new_custom_data(store, version, type, routing, &custom_data);

    TEST_ASSERT_MESSAGE(MCL_FAIL == code, "Expected code not returned from mcl_store_new_custom_data()!");

    MCL_FREE(custom_data);
    custom_data_destroy_Ignore();
    mcl_store_destroy(&store);
}

/**
 * GIVEN : Store is initialized.
 * WHEN  : Adding a new custom data to a streamable store is called.
 * THEN  : MCL_OK must be returned.
 */
void test_new_custom_data_003()
{
#if MCL_STREAM_ENABLED
    mcl_custom_data_t *custom_data = MCL_NULL;
    MCL_NEW(custom_data);

    // Set Fill meta_payload_type.
    string_initialize_new(type, 0, &(custom_data->meta.payload.type));

    // Set meta.payload.version.
    string_initialize_new(version, 0, &(custom_data->meta.payload.version));

    // Set up mocks.
    custom_data_initialize_ExpectAnyArgsAndReturn(MCL_OK);
    custom_data_initialize_ReturnThruPtr_custom_data(&custom_data);

    // initialize store
    mcl_store_t *store = MCL_NULL;
    mcl_store_initialize(MCL_TRUE, &store);

    mcl_custom_data_t *new_custom_data = MCL_NULL;

    // call test function
    E_MCL_ERROR_CODE code = mcl_store_new_custom_data(store, version, type, routing, &new_custom_data);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Expected code not returned from mcl_store_new_custom_data()!");

    // Check if custom_data is added successfully or not.
    TEST_ASSERT_EQUAL_STRING_MESSAGE(version, ((mcl_custom_data_t *)((store_data_t *)store->high_priority_list->current->data)->data)->meta.payload.version->buffer,
                                     "version is wrong.");

    // Clean up.
    string_destroy(&((mcl_custom_data_t *)((store_data_t *)store->high_priority_list->current->data)->data)->meta.payload.version);
    string_destroy(&((mcl_custom_data_t *)((store_data_t *)store->high_priority_list->current->data)->data)->meta.payload.type);
    MCL_FREE(custom_data);
    custom_data_destroy_Ignore();
    mcl_store_destroy(&store);
#else
	tearDown();
	TEST_IGNORE_MESSAGE("Ignored test_new_custom_data_003.");
#endif
}

/**
 * GIVEN : One of mandatory parameters is given as MCL_NULL.
 * WHEN  : mcl_store_new_custom_data() is called.
 * THEN  : MCL_TRIGGERED_WITH_NULL must be returned.
 */
void test_new_custom_data_004()
{
    E_MCL_ERROR_CODE return_code;
    mcl_store_t dummy_store;
    const char dummy_string[] = "dummy";
    const char *dummy_routing = MCL_NULL; // this parameter is not mandatory.
    mcl_custom_data_t *dummy_custom_data;

    return_code = mcl_store_new_custom_data(MCL_NULL, dummy_string, dummy_string, dummy_routing, &dummy_custom_data);
    TEST_ASSERT_MESSAGE(MCL_TRIGGERED_WITH_NULL == return_code, "mcl_store_new_custom_data() does not return MCL_TRIGGERED_WITH_NULL.");

    return_code = mcl_store_new_custom_data(&dummy_store, MCL_NULL, dummy_string, dummy_routing, &dummy_custom_data);
    TEST_ASSERT_MESSAGE(MCL_TRIGGERED_WITH_NULL == return_code, "mcl_store_new_custom_data() does not return MCL_TRIGGERED_WITH_NULL.");

    return_code = mcl_store_new_custom_data(&dummy_store, dummy_string, MCL_NULL, dummy_routing, &dummy_custom_data);
    TEST_ASSERT_MESSAGE(MCL_TRIGGERED_WITH_NULL == return_code, "mcl_store_new_custom_data() does not return MCL_TRIGGERED_WITH_NULL.");

    return_code = mcl_store_new_custom_data(&dummy_store, dummy_string, dummy_string, dummy_routing, MCL_NULL);
    TEST_ASSERT_MESSAGE(MCL_TRIGGERED_WITH_NULL == return_code, "mcl_store_new_custom_data() does not return MCL_TRIGGERED_WITH_NULL.");
}

/**
 * GIVEN : Store is initialized.
 * WHEN  : Adding a new stream data to a non streamable store is called.
 * THEN  : MCL_FAIL returned.
 */
void test_new_stream_data_001()
{
    mcl_stream_data_t *stream_data = MCL_NULL;
    MCL_NEW(stream_data);

    // initialize store
    mcl_store_t *store = MCL_NULL;
    mcl_store_initialize(MCL_FALSE, &store);

    mcl_stream_data_t *new_stream_data = MCL_NULL;

    // call test function
    E_MCL_ERROR_CODE code = mcl_store_new_stream_data(store, version, type, routing, stream_data_read_callback, MCL_NULL, &new_stream_data);
    TEST_ASSERT_MESSAGE(MCL_INVALID_PARAMETER == code, "Expected code not returned from mcl_store_new_stream_data()!");

    MCL_FREE(stream_data);
    mcl_store_destroy(&store);
}

/**
 * GIVEN : Store is initialized.
 * WHEN  : Adding a new stream data to a streamable store is called.
 * THEN  : MCL_OK returned.
 */
void test_new_stream_data_002()
{
#if MCL_STREAM_ENABLED
    mcl_stream_data_t *stream_data = MCL_NULL;
    MCL_NEW(stream_data);
    MCL_NEW(stream_data->base);

    // Set Fill meta_payload_type.
    string_initialize_new(type, 0, &(stream_data->base->meta.payload.type));

    // Set meta.payload.version.
    string_initialize_new(version, 0, &(stream_data->base->meta.payload.version));

    // Set up mocks.
    stream_data_initialize_ExpectAnyArgsAndReturn(MCL_OK);
    stream_data_initialize_ReturnThruPtr_stream_data(&stream_data);

    // initialize store
    mcl_store_t *store = MCL_NULL;
    mcl_store_initialize(MCL_TRUE, &store);

    mcl_stream_data_t *new_stream_data = MCL_NULL;

    // call test function
    E_MCL_ERROR_CODE code = mcl_store_new_stream_data(store, version, type, routing, stream_data_read_callback, MCL_NULL, &new_stream_data);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Expected code not returned from mcl_store_new_stream_data()!");

    // Check if custom_data is added successfully or not.
    TEST_ASSERT_EQUAL_STRING_MESSAGE(version, ((mcl_stream_data_t *)((store_data_t *)store->high_priority_list->current->data)->data)->base->meta.payload.version->buffer,
                                     "version is wrong.");

    // Clean up.
    string_destroy(&((mcl_stream_data_t *)((store_data_t *)store->high_priority_list->current->data)->data)->base->meta.payload.version);
    string_destroy(&((mcl_stream_data_t *)((store_data_t *)store->high_priority_list->current->data)->data)->base->meta.payload.type);
    MCL_FREE(stream_data->base);
    MCL_FREE(stream_data);
    stream_data_destroy_Ignore();
    mcl_store_destroy(&store);
#else
	tearDown();
	TEST_IGNORE();
#endif
}

/**
 * GIVEN : Store is initialized.
 * WHEN  : Callback function is NULL.
 * THEN  : MCL_TRIGGERED_WITH_NULL is returned.
 */
void test_new_stream_data_003()
{
#if MCL_STREAM_ENABLED
    // initialize store
    mcl_store_t *store = MCL_NULL;
    mcl_store_initialize(MCL_TRUE, &store);
	
    mcl_stream_data_t *new_stream_data = MCL_NULL;

    // call test function
    E_MCL_ERROR_CODE code = mcl_store_new_stream_data(store, version, type, routing, MCL_NULL, MCL_NULL, &new_stream_data);
	
	mcl_store_destroy(&store);

    TEST_ASSERT_MESSAGE(MCL_TRIGGERED_WITH_NULL == code, "Expected code not returned from mcl_store_new_stream_data()!");
#else
	tearDown();
	TEST_IGNORE();
#endif
}

/**
 * GIVEN : Store is initialized.
 * WHEN  : Adding a new time_series to store is called.
 * THEN  : MCL_OK must be returned.
 */
void test_new_time_series_001()
{
    mcl_time_series_t *time_series = MCL_NULL;
    MCL_NEW(time_series);

    // Set Fill meta_payload_type.
    string_initialize_new(type, 0, &(time_series->meta.payload.type));

    // Set meta.payload.version.
    string_initialize_new(version, 0, &(time_series->meta.payload.version));

    // Set up mocks.
    time_series_initialize_ExpectAnyArgsAndReturn(MCL_OK);
    time_series_initialize_ReturnThruPtr_time_series(&time_series);

    // Initialize store.
    mcl_store_t *store = MCL_NULL;
    mcl_store_initialize(MCL_FALSE, &store);

    // Call test function.
    mcl_time_series_t *new_time_series = MCL_NULL;
    E_MCL_ERROR_CODE code = mcl_store_new_time_series(store, version, "e3217e2b-7036-49f2-9814-4c38542cd781", MCL_NULL, &new_time_series);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Expected code not returned from mcl_store_new_time_series()!");

    // Check if time_series is added successfully or not.
    TEST_ASSERT_EQUAL_STRING_MESSAGE(version, ((mcl_time_series_t *)((store_data_t *)store->high_priority_list->current->data)->data)->meta.payload.version->buffer,
                                     "version is wrong.");

    // Clean up.
    string_destroy(&((mcl_time_series_t *)((store_data_t *)store->high_priority_list->current->data)->data)->meta.payload.version);
    string_destroy(&((mcl_time_series_t *)((store_data_t *)store->high_priority_list->current->data)->data)->meta.payload.type);
    MCL_FREE(time_series);
    time_series_destroy_Ignore();
    mcl_store_destroy(&store);
}

/**
 * GIVEN : Store is initialized. Mock for creating a new time_series returns error code.
 * WHEN  : Adding a new time_series to store is called.
 * THEN  : MCL_FAIL must be returned.
 */
void test_new_time_series_002()
{
    // Setup mocks.
    time_series_initialize_IgnoreAndReturn(MCL_FAIL);

    // Initialize store.
    mcl_store_t *store = MCL_NULL;
    mcl_store_initialize(MCL_FALSE, &store);

    // Call test function.
    mcl_time_series_t *time_series = MCL_NULL;
    E_MCL_ERROR_CODE code = mcl_store_new_time_series(store, version, "e3217e2b-7036-49f2-9814-4c38542cd781", MCL_NULL, &time_series);

    TEST_ASSERT_MESSAGE(MCL_FAIL == code, "Expected code not returned from mcl_store_new_time_series()!");

    MCL_FREE(time_series);
    time_series_destroy_Ignore();
    mcl_store_destroy(&store);
}

/**
 * GIVEN : Store is initialized.
 * WHEN  : Adding a new time_series to a streamable store is called.
 * THEN  : MCL_OK must be returned.
 */
void test_new_time_series_003()
{
#if MCL_STREAM_ENABLED
    mcl_time_series_t *time_series = MCL_NULL;
    MCL_NEW(time_series);

    // Set Fill meta_payload_type.
    string_initialize_new(type, 0, &(time_series->meta.payload.type));

    // Set meta.payload.version.
    string_initialize_new(version, 0, &(time_series->meta.payload.version));

    // Set up mocks.
    time_series_initialize_ExpectAnyArgsAndReturn(MCL_OK);
    time_series_initialize_ReturnThruPtr_time_series(&time_series);

    // Initialize store.
    mcl_store_t *store = MCL_NULL;
    mcl_store_initialize(MCL_TRUE, &store);

    // Call test function.
    mcl_time_series_t *new_time_series = MCL_NULL;
    E_MCL_ERROR_CODE code = mcl_store_new_time_series(store, version, "e3217e2b-7036-49f2-9814-4c38542cd781", MCL_NULL, &new_time_series);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Expected code not returned from mcl_store_new_time_series()!");

    // Check if time_series is added successfully or not.
    TEST_ASSERT_EQUAL_STRING_MESSAGE(version, ((mcl_time_series_t *)((store_data_t *)store->high_priority_list->current->data)->data)->meta.payload.version->buffer,
                                     "version is wrong.");

    // Clean up.
    string_destroy(&((mcl_time_series_t *)((store_data_t *)store->high_priority_list->current->data)->data)->meta.payload.version);
    string_destroy(&((mcl_time_series_t *)((store_data_t *)store->high_priority_list->current->data)->data)->meta.payload.type);
    MCL_FREE(time_series);
    time_series_destroy_Ignore();
    mcl_store_destroy(&store);
#else
	tearDown();
	TEST_IGNORE();
#endif
}

/**
 * GIVEN : One of mandatory parameters is given as MCL_NULL.
 * WHEN  : mcl_store_new_time_series() is called.
 * THEN  : MCL_TRIGGERED_WITH_NULL must be returned.
 */
void test_new_time_series_004()
{
    E_MCL_ERROR_CODE return_code;
    mcl_store_t dummy_store;
    const char dummy_string[] = "dummy";
    mcl_time_series_t *dummy_time_series;
    const char * dummy_router = MCL_NULL; // this parameter is not mandatory.

    return_code = mcl_store_new_time_series(MCL_NULL, dummy_string, dummy_string, dummy_router, &dummy_time_series);
    TEST_ASSERT_MESSAGE(MCL_TRIGGERED_WITH_NULL == return_code, "mcl_store_new_time_series() does not return MCL_TRIGGERED_WITH_NULL.");

    return_code = mcl_store_new_time_series(&dummy_store, MCL_NULL, dummy_string, dummy_router, &dummy_time_series);
    TEST_ASSERT_MESSAGE(MCL_TRIGGERED_WITH_NULL == return_code, "mcl_store_new_time_series() does not return MCL_TRIGGERED_WITH_NULL.");

    return_code = mcl_store_new_time_series(&dummy_store, dummy_string, MCL_NULL, dummy_router, &dummy_time_series);
    TEST_ASSERT_MESSAGE(MCL_TRIGGERED_WITH_NULL == return_code, "mcl_store_new_time_series() does not return MCL_TRIGGERED_WITH_NULL.");

    return_code = mcl_store_new_time_series(&dummy_store, dummy_string, dummy_string, dummy_router, MCL_NULL);
    TEST_ASSERT_MESSAGE(MCL_TRIGGERED_WITH_NULL == return_code, "mcl_store_new_time_series() does not return MCL_TRIGGERED_WITH_NULL.");
}

/**
 * GIVEN : Version with invalid format like "1-2".
 * WHEN  : mcl_store_new_time_series() is called.
 * THEN  : MCL_INVALID_PARAMETER must be returned.
 */
void test_new_time_series_005()
{
    E_MCL_ERROR_CODE return_code;
    mcl_store_t dummy_store;
    const char *dummy_string = "dummy";
    mcl_time_series_t *dummy_time_series;
    const char * dummy_router = MCL_NULL;
    const char *invalid_version = "1-2";

    return_code = mcl_store_new_time_series(&dummy_store, invalid_version, dummy_string, dummy_router, &dummy_time_series);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_INVALID_PARAMETER, return_code, "mcl_store_new_time_series() does not return MCL_INVALID_PARAMETER for wrong version format.");
}

/**
 * GIVEN : Version with invalid format like "12.".
 * WHEN  : mcl_store_new_time_series() is called.
 * THEN  : MCL_INVALID_PARAMETER must be returned.
 */
void test_new_time_series_006()
{
    E_MCL_ERROR_CODE return_code;
    mcl_store_t dummy_store;
    const char *dummy_string = "dummy";
    mcl_time_series_t *dummy_time_series;
    const char * dummy_router = MCL_NULL;
    const char *invalid_version = "12.";

    return_code = mcl_store_new_time_series(&dummy_store, invalid_version, dummy_string, dummy_router, &dummy_time_series);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_INVALID_PARAMETER, return_code, "mcl_store_new_time_series() does not return MCL_INVALID_PARAMETER for wrong version format.");
}

/**
 * GIVEN : Version with invalid format like "a.2".
 * WHEN  : mcl_store_new_time_series() is called.
 * THEN  : MCL_INVALID_PARAMETER must be returned.
 */
void test_new_time_series_007()
{
    E_MCL_ERROR_CODE return_code;
    mcl_store_t dummy_store;
    const char *dummy_string = "dummy";
    mcl_time_series_t *dummy_time_series;
    const char * dummy_router = MCL_NULL;
    const char *invalid_version = "a.2";

    return_code = mcl_store_new_time_series(&dummy_store, invalid_version, dummy_string, dummy_router, &dummy_time_series);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_INVALID_PARAMETER, return_code, "mcl_store_new_time_series() does not return MCL_INVALID_PARAMETER for wrong version format.");
}

/**
 * GIVEN : Version with invalid format like ".12".
 * WHEN  : mcl_store_new_time_series() is called.
 * THEN  : MCL_INVALID_PARAMETER must be returned.
 */
void test_new_time_series_008()
{
    E_MCL_ERROR_CODE return_code;
    mcl_store_t dummy_store;
    const char *dummy_string = "dummy";
    mcl_time_series_t *dummy_time_series;
    const char * dummy_router = MCL_NULL;
    const char *invalid_version = ".12";

    return_code = mcl_store_new_time_series(&dummy_store, invalid_version, dummy_string, dummy_router, &dummy_time_series);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_INVALID_PARAMETER, return_code, "mcl_store_new_time_series() does not return MCL_INVALID_PARAMETER for wrong version format.");
}

/**
 * GIVEN : Store is initialized.
 * WHEN  : Adding a new event to store is called.
 * THEN  : MCL_OK must be returned.
 */
void test_new_event_001()
{
    // Setup mocks.
    mcl_event_t *event = MCL_NULL;
    MCL_NEW(event);
	MCL_NEW(event->meta);
	MCL_NEW(event->payload);

    char *event_type= "businessEvent";

    // Set meta.payload.version.
    string_initialize_new(event_type, 0, &(event->meta->payload.type));
    string_initialize_new(version, 0, &(event->meta->payload.version));

	event_list_t *event_list = MCL_NULL;
    MCL_NEW(event_list);
    MCL_NEW(event_list->meta);
    list_initialize(&event_list->events);

    string_initialize_new(event_type, 0, &(event_list->meta->payload.type));
    string_initialize_new(version, 0, &(event_list->meta->payload.version));

    // Set up mocks.
    event_initialize_ExpectAnyArgsAndReturn(MCL_OK);
    event_initialize_ReturnThruPtr_event(&event);

    event_list_initialize_ExpectAnyArgsAndReturn(MCL_OK);
    event_list_initialize_ReturnThruPtr_event_list(&event_list);

    event_list_add_event_ExpectAnyArgsAndReturn(MCL_OK);

    // Add event to event set.
    list_add(event_list->events, event);
    list_add(event_list->events, event);

    // Initialize store.
    mcl_store_t *store = MCL_NULL;
    mcl_store_initialize(MCL_FALSE, &store);

    // Call test function.
    mcl_event_t *new_event = MCL_NULL;
    E_MCL_ERROR_CODE code = mcl_store_new_event(store, version, event_payload_type, event_payload_version, severity, timestamp, &new_event);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Expected code not returned from mcl_store_new_event()!");

    // Check if event is added successfully or not.
    TEST_ASSERT_EQUAL_STRING_MESSAGE(version, ((event_list_t *)((store_data_t *)store->high_priority_list->current->data)->data)->meta->payload.version->buffer,
                                     "version is wrong.");

    TEST_ASSERT_EQUAL_STRING_MESSAGE(event_type, ((event_list_t *)((store_data_t *)store->high_priority_list->current->data)->data)->meta->payload.type->buffer,
                                     "type is wrong.");

    // Clean up.
    string_destroy(&event->meta->payload.type);
    string_destroy(&event->meta->payload.version);

    string_destroy(&((event_list_t *)((store_data_t *)store->high_priority_list->current->data)->data)->meta->payload.version);
    string_destroy(&((event_list_t *)((store_data_t *)store->high_priority_list->current->data)->data)->meta->payload.type);
    MCL_FREE(event->meta);
    MCL_FREE(event->payload);
    MCL_FREE(event);
    event_list_destroy_Ignore();
    mcl_store_destroy(&store);
    MCL_FREE(event_list->meta);
    list_destroy(&event_list->events);
    MCL_FREE(event_list);
}

/**
 * GIVEN : Store is initialized. Mock for creating a new event returns error code.
 * WHEN  : Adding a new event to store is called.
 * THEN  : MCL_FAIL must be returned.
 */
void test_new_event_002()
{
    // Setup mocks.
    event_initialize_IgnoreAndReturn(MCL_FAIL);

    event_list_t *event_list = MCL_NULL;
    MCL_NEW(event_list);
    MCL_NEW(event_list->meta);
    list_initialize(&event_list->events);

    char *event_type= "businessEvent";

    string_initialize_new(event_type, 0, &(event_list->meta->payload.type));
    string_initialize_new(version, 0, &(event_list->meta->payload.version));

    event_list_initialize_ExpectAnyArgsAndReturn(MCL_OK);
    event_list_initialize_ReturnThruPtr_event_list(&event_list);

    // Initialize store.
    mcl_store_t *store = MCL_NULL;
    mcl_store_initialize(MCL_FALSE, &store);

    // Call test function.
    mcl_event_t *event = MCL_NULL;
    E_MCL_ERROR_CODE code = mcl_store_new_event(store, version, event_payload_type, event_payload_version, severity, timestamp, &event);
    TEST_ASSERT_MESSAGE(MCL_FAIL == code, "Expected code not returned from mcl_store_new_event()!");

    // Clean up.
    string_destroy(&((event_list_t *)((store_data_t *)store->high_priority_list->current->data)->data)->meta->payload.version);
    string_destroy(&((event_list_t *)((store_data_t *)store->high_priority_list->current->data)->data)->meta->payload.type);
    event_list_destroy_Ignore();
    mcl_store_destroy(&store);
    MCL_FREE(event_list->meta);
    list_destroy(&event_list->events);
    MCL_FREE(event_list);
}

/**
 * GIVEN : Store is initialized.
 * WHEN  : Adding a new event to store is called.
 * THEN  : MCL_OK must be returned.
 */
void test_new_event_003()
{
    // Setup mocks.
    mcl_event_t *event = MCL_NULL;
    MCL_NEW(event);
    MCL_NEW(event->meta);
    MCL_NEW(event->payload);

    char *event_type= "businessEvent";

    // Set meta.payload.version.
    string_initialize_new(event_type, 0, &(event->meta->payload.type));
    string_initialize_new(version, 0, &(event->meta->payload.version));

    event_list_t *event_list = MCL_NULL;
    MCL_NEW(event_list);
    MCL_NEW(event_list->meta);
    list_initialize(&event_list->events);

    string_initialize_new(event_type, 0, &(event_list->meta->payload.type));
    string_initialize_new(version, 0, &(event_list->meta->payload.version));

    // Set up mocks.
    event_initialize_ExpectAnyArgsAndReturn(MCL_OK);
    event_initialize_ReturnThruPtr_event(&event);

    event_list_initialize_ExpectAnyArgsAndReturn(MCL_OK);
    event_list_initialize_ReturnThruPtr_event_list(&event_list);

    event_list_add_event_ExpectAnyArgsAndReturn(MCL_OK);

    // Add event to event set.
    list_add(event_list->events, event);
    list_add(event_list->events, event);

    // Initialize store.
    mcl_store_t *store = MCL_NULL;
    mcl_store_initialize(MCL_FALSE, &store);

    // Call test function.
    mcl_event_t *new_event = MCL_NULL;
    E_MCL_ERROR_CODE code = mcl_store_new_event(store, version, event_payload_type, event_payload_version, severity, timestamp, &new_event);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Expected code not returned from mcl_store_new_event()!");

    // Check if event is added successfully or not.
    TEST_ASSERT_EQUAL_STRING_MESSAGE(version, ((event_list_t *)((store_data_t *)store->high_priority_list->current->data)->data)->meta->payload.version->buffer,
                                     "version is wrong.");

    TEST_ASSERT_EQUAL_STRING_MESSAGE(event_type, ((event_list_t *)((store_data_t *)store->high_priority_list->current->data)->data)->meta->payload.type->buffer,
                                     "type is wrong.");

    // Clean up.
    string_destroy(&event->meta->payload.type);
    string_destroy(&event->meta->payload.version);
    string_destroy(&((event_list_t *)((store_data_t *)store->high_priority_list->current->data)->data)->meta->payload.version);
    string_destroy(&((event_list_t *)((store_data_t *)store->high_priority_list->current->data)->data)->meta->payload.type);
    MCL_FREE(event->meta);
    MCL_FREE(event->payload);
    MCL_FREE(event);
    event_list_destroy_Ignore();
    mcl_store_destroy(&store);
    MCL_FREE(event_list->meta);
    list_destroy(&event_list->events);
    MCL_FREE(event_list);
}

/**
 * GIVEN : One of mandatory parameters is given as MCL_NULL.
 * WHEN  : mcl_store_new_event() is called.
 * THEN  : MCL_TRIGGERED_WITH_NULL must be returned.
 */
void test_new_event_004()
{
    E_MCL_ERROR_CODE code;
    mcl_store_t store;
    mcl_event_t *event;

    code = mcl_store_new_event(MCL_NULL, version, event_payload_type, event_payload_version, severity, timestamp, &event);
    TEST_ASSERT_MESSAGE(MCL_TRIGGERED_WITH_NULL == code, "mcl_store_new_event() does not return MCL_TRIGGERED_WITH_NULL for store.");

    code = mcl_store_new_event(&store, MCL_NULL, event_payload_type, event_payload_version, severity, timestamp, &event);
    TEST_ASSERT_MESSAGE(MCL_TRIGGERED_WITH_NULL == code, "mcl_store_new_event() does not return MCL_TRIGGERED_WITH_NULL for version.");

    code = mcl_store_new_event(&store, version, MCL_NULL, event_payload_version, severity, timestamp, &event);
    TEST_ASSERT_MESSAGE(MCL_TRIGGERED_WITH_NULL == code, "mcl_store_new_event() does not return MCL_TRIGGERED_WITH_NULL for event type.");

    code = mcl_store_new_event(&store, version, event_payload_type, MCL_NULL, severity, timestamp, &event);
    TEST_ASSERT_MESSAGE(MCL_TRIGGERED_WITH_NULL == code, "mcl_store_new_event() does not return MCL_TRIGGERED_WITH_NULL for event payload version.");

    code = mcl_store_new_event(&store, version, event_payload_type, event_payload_version, severity, MCL_NULL, &event);
    TEST_ASSERT_MESSAGE(MCL_TRIGGERED_WITH_NULL == code, "mcl_store_new_event() does not return MCL_TRIGGERED_WITH_NULL for timestamp.");

    code = mcl_store_new_event(&store, version, event_payload_type, event_payload_version, severity, timestamp, MCL_NULL);
    TEST_ASSERT_MESSAGE(MCL_TRIGGERED_WITH_NULL == code, "mcl_store_new_event() does not return MCL_TRIGGERED_WITH_NULL for timestamp.");
}

/**
* GIVEN : Invalid version is given.
* WHEN  : mcl_store_new_event() is called.
* THEN  : MCL_INVALID_PARAMETER must be returned.
*/
void test_new_event_005()
{
    E_MCL_ERROR_CODE code;
    mcl_store_t store;
    mcl_event_t *event;

    code = mcl_store_new_event(&store, "1.01", event_payload_type, event_payload_version, severity, timestamp, &event);
    TEST_ASSERT_MESSAGE(MCL_INVALID_PARAMETER == code, "mcl_store_new_event() does not return MCL_INVALID_PARAMETER for store.");
}

/**
* GIVEN : Store is initialized.
* WHEN  : mcl_store_new_event is called with version "2.0".
* THEN  : MCL_OK must be returned.
*/
void test_new_event_006()
{
    // Setup mocks.
    mcl_event_t *event = MCL_NULL;
    MCL_NEW(event);
    MCL_NEW(event->meta);
    MCL_NEW(event->payload);

    char *event_category = "businessEvent";

    // Set meta.payload.version.
    string_initialize_new(event_category, 0, &(event->meta->payload.type));
    string_initialize_new(version, 0, &(event->meta->payload.version));

    event_list_t *event_list = MCL_NULL;
    MCL_NEW(event_list);
    MCL_NEW(event_list->meta);
    list_initialize(&event_list->events);

    string_initialize_new(event_category, 0, &(event_list->meta->payload.type));
    string_initialize_new(version, 0, &(event_list->meta->payload.version));

    // Set up mocks.
    event_initialize_ExpectAnyArgsAndReturn(MCL_OK);
    event_initialize_ReturnThruPtr_event(&event);

    event_list_initialize_ExpectAnyArgsAndReturn(MCL_OK);
    event_list_initialize_ReturnThruPtr_event_list(&event_list);

    event_list_add_event_ExpectAnyArgsAndReturn(MCL_OK);

    // Add event to event set.
    list_add(event_list->events, event);
    list_add(event_list->events, event);

    // Initialize store.
    mcl_store_t *store = MCL_NULL;
    mcl_store_initialize(MCL_FALSE, &store);

    // Call test function.
    mcl_event_t *new_event = MCL_NULL;
    E_MCL_ERROR_CODE code = mcl_store_new_event(store, "2.0", event_payload_type, event_payload_version, severity, timestamp, &new_event);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Expected code not returned from mcl_store_new_event()!");

    // Check if event is added successfully or not.
    TEST_ASSERT_EQUAL_STRING_MESSAGE(version, ((event_list_t *)((store_data_t *)store->high_priority_list->current->data)->data)->meta->payload.version->buffer,
        "version is wrong.");

    TEST_ASSERT_EQUAL_STRING_MESSAGE(event_category, ((event_list_t *)((store_data_t *)store->high_priority_list->current->data)->data)->meta->payload.type->buffer,
        "type is wrong.");

    // Clean up.
    string_destroy(&event->meta->payload.type);
    string_destroy(&event->meta->payload.version);

    string_destroy(&((event_list_t *)((store_data_t *)store->high_priority_list->current->data)->data)->meta->payload.version);
    string_destroy(&((event_list_t *)((store_data_t *)store->high_priority_list->current->data)->data)->meta->payload.type);
    MCL_FREE(event->meta);
    MCL_FREE(event->payload);
    MCL_FREE(event);
    event_list_destroy_Ignore();
    mcl_store_destroy(&store);
    MCL_FREE(event_list->meta);
    list_destroy(&event_list->events);
    MCL_FREE(event_list);
}

/**
 * GIVEN : One of mandatory parameters is given as MCL_NULL.
 * WHEN  : mcl_store_new_file() is called.
 * THEN  : MCL_TRIGGERED_WITH_NULL must be returned.
 */
void test_new_file_001()
{
    E_MCL_ERROR_CODE return_code;
    mcl_store_t dummy_store;
    const char dummy_string[] = "dummy";
	mcl_file_t *dummy_file;

    return_code = mcl_store_new_file(MCL_NULL, dummy_string, dummy_string, dummy_string, dummy_string, dummy_string, &dummy_file);
    TEST_ASSERT_MESSAGE(MCL_TRIGGERED_WITH_NULL == return_code, "mcl_store_new_file() does not return MCL_TRIGGERED_WITH_NULL.");

    return_code = mcl_store_new_file(&dummy_store, MCL_NULL, dummy_string, dummy_string, dummy_string, dummy_string, &dummy_file);
    TEST_ASSERT_MESSAGE(MCL_TRIGGERED_WITH_NULL == return_code, "mcl_store_new_file() does not return MCL_TRIGGERED_WITH_NULL.");

    return_code = mcl_store_new_file(&dummy_store, dummy_string, MCL_NULL, dummy_string, dummy_string, dummy_string, &dummy_file);
    TEST_ASSERT_MESSAGE(MCL_TRIGGERED_WITH_NULL == return_code, "mcl_store_new_file() does not return MCL_TRIGGERED_WITH_NULL.");

    return_code = mcl_store_new_file(&dummy_store, dummy_string, dummy_string, MCL_NULL, dummy_string, dummy_string, &dummy_file);
    TEST_ASSERT_MESSAGE(MCL_TRIGGERED_WITH_NULL == return_code, "mcl_store_new_file() does not return MCL_TRIGGERED_WITH_NULL.");

    return_code = mcl_store_new_file(&dummy_store, dummy_string, dummy_string, dummy_string, dummy_string, dummy_string, MCL_NULL);
    TEST_ASSERT_MESSAGE(MCL_TRIGGERED_WITH_NULL == return_code, "mcl_store_new_file() does not return MCL_TRIGGERED_WITH_NULL.");
}

