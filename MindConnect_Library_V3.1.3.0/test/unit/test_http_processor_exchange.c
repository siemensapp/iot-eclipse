/*!**********************************************************************
*
* @copyright Copyright (C) 2016 Siemens Aktiengesellschaft.
*            All rights reserved.
*
*************************************************************************
*
* @file     test_http_processor_exchange.c
* @date     Oct 10, 2016
* @brief    Short description, usually one or two sentences
*
************************************************************************/

#include "string_type.h"
#include "string_array.h"
#include "http_processor.h"
#include "memory.h"
#include "string_util.h"
#include "mcl_time_series.h"
#include "unity.h"
#include "mock_http_request.h"
#include "mock_http_client.h"
#include "mock_security_handler.h"
#include "mock_http_response.h"
#include "mock_json.h"
#include "security.h"
#include "http_definitions.h"
#include "store.h"
#include "time_series.h"
#include "event.h"
#include "list.h"
#include "custom_data.h"
#include "random.h"
#include "mock_storage.h"
#include "mock_security.h"
#include "data_types.h"
#include "definitions.h"
#include "mcl/mcl_store.h"
#include "json_util.h"
#include "mock_event_list.h"
#include "mock_time_util.h"

configuration_t *configuration = MCL_NULL;
http_processor_t *http_processor = MCL_NULL;
http_response_t *success_response = MCL_NULL;
http_response_t *success_response_2 = MCL_NULL;
http_response_t *success_response_3 = MCL_NULL;
http_response_t *fail_response = MCL_NULL;
http_request_t *http_request = MCL_NULL;
http_request_t *http_request_2 = MCL_NULL;
http_request_t *http_request_3 = MCL_NULL;
char *timestamp = "NOW";
char *routing = "vnd.kuka.FingerprintAnalizer";
security_handler_t *security_handler = MCL_NULL;
event_list_t *event_list = MCL_NULL;

string_t *new_meta_json_string()
{
    string_t *meta_json_string;
    MCL_NEW(meta_json_string);
    meta_json_string->buffer = MCL_MALLOC(17);
    meta_json_string->buffer[0] = MCL_NULL_CHAR;
    string_util_strncat(meta_json_string->buffer, "meta_json_string", 16);

    // meta_json_string->buffer = "meta_json_string";
    meta_json_string->length = 16;
    meta_json_string->type = MCL_STRING_COPY_DESTROY;
    return meta_json_string;
}

string_t *new_time_series_payload_json_string()
{
    string_t *payload_json_string;
    MCL_NEW(payload_json_string);
    payload_json_string->buffer = MCL_MALLOC(20);
    payload_json_string->buffer[0] = MCL_NULL_CHAR;
    string_util_strncat(payload_json_string->buffer, "payload_json_string", 19);

    // meta_json_string->buffer = "meta_json_string";
    payload_json_string->length = 19;
    payload_json_string->type = MCL_STRING_COPY_DESTROY;
    return payload_json_string;
}

void setUp(void)
{
    MCL_NEW(configuration);
    MCL_NEW(configuration->mindsphere_hostname);
    configuration->mindsphere_hostname->buffer = "www.siemens.com";
    configuration->mindsphere_hostname->length = 15;
    configuration->mindsphere_hostname->type = MCL_STRING_NOT_COPY_NOT_DESTROY;
    configuration->security_profile = MCL_SECURITY_SHARED_SECRET;
    configuration->store_path = MCL_NULL;
    configuration->load_function.rsa = MCL_NULL;
    configuration->save_function.rsa = MCL_NULL;
    string_initialize_new("InitialAccessToken", 0, &configuration->initial_access_token);

    MCL_NEW(security_handler);
    security_handler->authentication_key = MCL_NULL;
    security_handler->onboarding_key = MCL_NULL;
    security_handler->hmac_key = MCL_NULL;
    security_handler->rsa.private_key = MCL_NULL;
    security_handler->rsa.public_key = MCL_NULL;
    security_handler->rsa.session_key = MCL_NULL;
    security_handler->authentication_key_size = 0;
    security_handler->access_token = MCL_NULL;
    security_handler->registration_access_token = MCL_NULL;
    security_handler->client_secret = MCL_NULL;
    security_handler->registration_client_uri = MCL_NULL;
    security_handler->client_id = MCL_NULL;
    string_initialize_new("dummy_access_token", 0, &(security_handler->access_token));

    MCL_NEW(http_request);
    http_request->payload = MCL_MALLOC(100);
    http_request->payload_size = 100;

    MCL_NEW(http_request_2);
    http_request_2->payload = MCL_MALLOC(100);
    http_request_2->payload_size = 100;

    MCL_NEW(http_request_3);
    http_request_3->payload = MCL_MALLOC(100);
    http_request_3->payload_size = 100;

    MCL_NEW(success_response);
    success_response->result_code = MCL_HTTP_RESULT_CODE_SUCCESS;
    success_response->payload = "{\"test_name\":\"test_value\"}";
    success_response->payload_size = 26;
    MCL_NEW(success_response_2);
    success_response_2->result_code = MCL_HTTP_RESULT_CODE_SUCCESS;
    MCL_NEW(success_response_3);
    success_response_3->result_code = MCL_HTTP_RESULT_CODE_SUCCESS;
    MCL_NEW(fail_response);
    fail_response->result_code = MCL_HTTP_RESULT_CODE_UNAUTHORIZED;

    // general mocks :
    http_client_initialize_IgnoreAndReturn(MCL_OK);
    http_client_destroy_Ignore();
    security_handler_initialize_IgnoreAndReturn(MCL_OK);
    security_handler_destroy_Ignore();
    security_initialize_Ignore();
}

void tearDown(void)
{
    string_destroy(&configuration->initial_access_token);
    MCL_FREE(configuration->mindsphere_hostname);
    MCL_FREE(configuration);
    MCL_FREE(success_response);
    MCL_FREE(success_response_2);
    MCL_FREE(success_response_3);
    MCL_FREE(fail_response);
    MCL_FREE(http_request->payload);
    MCL_FREE(http_request);
    MCL_FREE(http_request_2->payload);
    MCL_FREE(http_request_2);
    MCL_FREE(http_request_3->payload);
    MCL_FREE(http_request_3);
    string_destroy(&security_handler->access_token);
    MCL_FREE(security_handler);
}

// GIVEN : http_processor initialized - store initialized - no data added to store - no communication problem
// WHEN  : http_processor_exchange triggered with empty store.
// THEN  : Expect exchange operation to be failed.
void test_exchange_001(void)
{
    E_MCL_ERROR_CODE result = http_processor_initialize(configuration, &http_processor);
    TEST_ASSERT_EQUAL_INT_MESSAGE(MCL_OK, result, "http_processor_initialize failed!");

    // create a store
    mcl_store_t *store = MCL_NULL;
    result = mcl_store_initialize(MCL_FALSE, &store);
    TEST_ASSERT_EQUAL_INT_MESSAGE(MCL_OK, result, "mcl_store_initialize failed!");

    result = http_processor_exchange(http_processor, store, NULL);
    TEST_ASSERT_EQUAL_INT_MESSAGE(MCL_STORE_IS_EMPTY, result, "Exchange operation is failed!");

    http_processor_destroy(&http_processor);
    mcl_store_destroy(&store);
}

// GIVEN : http_processor initialized - store initialized - 1 custom data added to store - no communication problem
// WHEN  : Payload size of the 1 custom data is less than the MAX http payload size.
// THEN  : Exchange operation would successfully add the custom data to the request and send it
void test_exchange_002(void)
{
    E_MCL_ERROR_CODE result = http_processor_initialize(configuration, &http_processor);
    TEST_ASSERT_EQUAL_INT_MESSAGE(MCL_OK, result, "http_processor_initialize failed!");

    http_processor->security_handler = security_handler;

    // create a store
    mcl_store_t *store = MCL_NULL;
    result = mcl_store_initialize(MCL_FALSE, &store);
    TEST_ASSERT_EQUAL_INT_MESSAGE(MCL_OK, result, "mcl_store_initialize failed!");

    // add a custom data to the store :
    mcl_uint8_t payload[] =
    {
        '0', '1', '2', '3', '4', '5', '6', '7', '8', '9'
    };
    mcl_custom_data_t *custom_data = MCL_NULL;
    result = mcl_store_new_custom_data(store, "1.0", "custom_data_type_1", routing, &custom_data);
    TEST_ASSERT_EQUAL_INT_MESSAGE(MCL_OK, result, "New custom data from the store failed!");
    custom_data->payload.buffer = payload;
    custom_data->payload.size = sizeof(payload) / sizeof(payload[0]);

    // define mocks before calling exchange :
    // 1- json_from_item_meta will be called for custom_data->meta : 1 time :
    json_from_item_meta_ExpectAnyArgsAndReturn(MCL_OK);

    string_t *json_meta = new_meta_json_string();
    json_from_item_meta_ReturnThruPtr_json_string(&json_meta);

    // 2- An http_request will be created : 1 time :
    http_request_initialize_ExpectAnyArgsAndReturn(MCL_OK);
    http_request_initialize_ReturnThruPtr_http_request(&http_request);

    // 2.1- Add headers : Content-type, Authorization, Correlation-ID
    http_request_add_header_IgnoreAndReturn(MCL_OK);
    http_request_add_header_IgnoreAndReturn(MCL_OK);
	http_request_add_header_IgnoreAndReturn(MCL_OK);

    http_request_finalize_IgnoreAndReturn(MCL_OK);

	// Correlation-ID
	security_generate_random_bytes_ExpectAnyArgsAndReturn(MCL_OK);

    // 3- Custom data will be added to the http_request by add_tuple : 1 time :
    // ( here it returns MCL_OK, which means adding it successful. http_processor_exchange shouldn't call it again. )
    http_request_add_tuple_IgnoreAndReturn(MCL_OK);

    // 4- Prepared http_request will be sent using http_client_send : 1 time :
    success_response->payload = "";
    http_client_send_ExpectAnyArgsAndReturn(MCL_OK);
    http_client_send_ReturnThruPtr_http_response(&success_response);

    // 5- Request and response objects will be destroyed once each :
    http_response_destroy_Ignore();
    http_request_destroy_Ignore();

    // 6- event set object will be destroyed once each : 1 time.
    event_list_destroy_Ignore();

    // mock preperation is done. Calling the testing function :
    result = http_processor_exchange(http_processor, store, NULL);
    TEST_ASSERT_EQUAL_INT_MESSAGE(MCL_OK, result, "Exchange operation is failed!");

    // check the item count in the store :
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, store->high_priority_list->count, "There are still remaining high priority data in the store!");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, store->low_priority_list->count, "There are still remaining low priority data in the store!");

    http_processor_destroy(&http_processor);
    mcl_store_destroy(&store);
}

// GIVEN : http_processor initialized - store initialized - 2 custom data added to store - no communication problem
// WHEN  : Payload size of the 2 custom data is less than the MAX http payload size.
// THEN  : Exchange operation would successfully add the custom data to the request and send it
void test_exchange_003(void)
{
    E_MCL_ERROR_CODE result = http_processor_initialize(configuration, &http_processor);
    TEST_ASSERT_EQUAL_INT_MESSAGE(MCL_OK, result, "http_processor_initialize failed!");

    http_processor->security_handler = security_handler;

    // create a store
    mcl_store_t *store = MCL_NULL;
    result = mcl_store_initialize(MCL_FALSE, &store);
    TEST_ASSERT_EQUAL_INT_MESSAGE(MCL_OK, result, "mcl_store_initialize failed!");

    // add 2 custom data to the store :
    mcl_uint8_t payload[] =
    {
        '0', '1', '2', '3', '4', '5', '6', '7', '8', '9'
    };
    mcl_custom_data_t *custom_data = MCL_NULL;

    // first custom_data:
    result = mcl_store_new_custom_data(store, "1.0", "custom_data_type_1", routing, &custom_data);
    TEST_ASSERT_EQUAL_INT_MESSAGE(MCL_OK, result, "New custom data from the store failed!");
    custom_data->payload.buffer = payload;
    custom_data->payload.size = sizeof(payload) / sizeof(payload[0]);

    // second custom_data :
    result = mcl_store_new_custom_data(store, "1.0", "custom_data_type_2", routing, &custom_data);
    TEST_ASSERT_EQUAL_INT_MESSAGE(MCL_OK, result, "New custom data from the store failed!");
    custom_data->payload.buffer = payload;
    custom_data->payload.size = sizeof(payload) / sizeof(payload[0]);

    // define mocks before calling exchange :
    // 1- json_from_item_meta will be called for custom_data->meta : 2 times :
    json_from_item_meta_ExpectAnyArgsAndReturn(MCL_OK);

    string_t *json_meta = new_meta_json_string();
    json_from_item_meta_ReturnThruPtr_json_string(&json_meta);
    json_from_item_meta_ExpectAnyArgsAndReturn(MCL_OK);

    string_t *json_meta_2 = new_meta_json_string();
    json_from_item_meta_ReturnThruPtr_json_string(&json_meta_2);

    // 2- An http_request will be created : 1 time :
    http_request_initialize_ExpectAnyArgsAndReturn(MCL_OK);
    http_request_initialize_ReturnThruPtr_http_request(&http_request);

	// 2.1- Add headers : Content-type, Authorization, Correlation-ID
	http_request_add_header_IgnoreAndReturn(MCL_OK);
	http_request_add_header_IgnoreAndReturn(MCL_OK);
	http_request_add_header_IgnoreAndReturn(MCL_OK);

    http_request_finalize_IgnoreAndReturn(MCL_OK);

	// Correlation-ID
	security_generate_random_bytes_ExpectAnyArgsAndReturn(MCL_OK);

    // 3- Custom data will be added to the http_request by add_tuple : 2 times :
    http_request_add_tuple_IgnoreAndReturn(MCL_OK);
    http_request_add_tuple_IgnoreAndReturn(MCL_OK);

    // 4- Prepared http_request will be sent using http_client_send : 1 time :
	success_response->payload = "";
	http_client_send_ExpectAnyArgsAndReturn(MCL_OK);
    http_client_send_ReturnThruPtr_http_response(&success_response);

    // 5- Request and response objects will be destroyed once each :
    http_response_destroy_Ignore();
    http_request_destroy_Ignore();

    // 6- event set object will be destroyed once each : 1 time.
    event_list_destroy_Ignore();

    result = http_processor_exchange(http_processor, store, NULL);
    TEST_ASSERT_EQUAL_INT_MESSAGE(MCL_OK, result, "Exchange operation is failed!");

    // check the item count in the store :
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, store->high_priority_list->count, "There are still remaining high priority data in the store!");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, store->low_priority_list->count, "There are still remaining low priority data in the store!");

    http_processor_destroy(&http_processor);
    mcl_store_destroy(&store);
}

// GIVEN : http_processor initialized - store initialized - 2 custom data added to store - no communication problem
// WHEN  : Payload size of the 1st custom data is less than the MAX http payload size.
// AND no space left from the first http_request after adding the first custom_data.
// AND Second custom_data will be added to a second http_request successfully.
// THEN  : Exchange operation would successfully add the custom data to the request and send it
void test_exchange_004(void)
{
    E_MCL_ERROR_CODE result = http_processor_initialize(configuration, &http_processor);
    TEST_ASSERT_EQUAL_INT_MESSAGE(MCL_OK, result, "http_processor_initialize failed!");

    http_processor->security_handler = security_handler;

    // create a store
    mcl_store_t *store = MCL_NULL;
    result = mcl_store_initialize(MCL_FALSE, &store);
    TEST_ASSERT_EQUAL_INT_MESSAGE(MCL_OK, result, "mcl_store_initialize failed!");

    // add 2 custom data to the store :
    mcl_uint8_t payload[] =
    {
        '0', '1', '2', '3', '4', '5', '6', '7', '8', '9'
    };
    mcl_custom_data_t *custom_data = MCL_NULL;
    mcl_custom_data_t *custom_data_2 = MCL_NULL;

    // first custom_data:
    result = mcl_store_new_custom_data(store, "1.0", "custom_data_type_1", routing, &custom_data);
    TEST_ASSERT_EQUAL_INT_MESSAGE(MCL_OK, result, "New custom data from the store failed!");
    custom_data->payload.buffer = payload;
    custom_data->payload.size = sizeof(payload) / sizeof(payload[0]);

    // second custom_data :
    result = mcl_store_new_custom_data(store, "1.0", "custom_data_type_2", routing, &custom_data_2);
    TEST_ASSERT_EQUAL_INT_MESSAGE(MCL_OK, result, "New custom data from the store failed!");
    custom_data_2->payload.buffer = payload;
    custom_data_2->payload.size = sizeof(payload) / sizeof(payload[0]);

    // define mocks before calling exchange :
    // 1- json_from_item_meta will be called for custom_data->meta : 2 times :
    json_from_item_meta_ExpectAnyArgsAndReturn(MCL_OK);

    string_t *json_meta = new_meta_json_string();
    json_from_item_meta_ReturnThruPtr_json_string(&json_meta);
    json_from_item_meta_ExpectAnyArgsAndReturn(MCL_OK);

    string_t *json_meta_2 = new_meta_json_string();
    json_from_item_meta_ReturnThruPtr_json_string(&json_meta_2);

    // 2- An http_request will be created : 1 time :
    http_request_initialize_ExpectAnyArgsAndReturn(MCL_OK);
    http_request_initialize_ReturnThruPtr_http_request(&http_request);

    // 2.1- Add headers : Content-type, Authorization
    http_request_add_header_IgnoreAndReturn(MCL_OK);
    http_request_add_header_IgnoreAndReturn(MCL_OK);

    http_request_finalize_IgnoreAndReturn(MCL_OK);

	// Correlation-ID
	security_generate_random_bytes_ExpectAnyArgsAndReturn(MCL_OK);
	security_generate_random_bytes_ExpectAnyArgsAndReturn(MCL_OK);

    // 3- Custom data will be added to the http_request by add_tuple : 2 times :
    // Second one will return error telling no more space left in the request
    http_request_add_tuple_IgnoreAndReturn(MCL_OK);
    http_request_add_tuple_IgnoreAndReturn(MCL_HTTP_REQUEST_NO_MORE_SPACE);

    // chunking will be tried. this is for it to be failed :
    http_request_get_available_space_for_tuple_IgnoreAndReturn(0);

    // http_processor_exchange mechanism will start over and try to add again and that should return error too. With that it will decide we can't add anything anymore :
    http_request_add_tuple_IgnoreAndReturn(MCL_HTTP_REQUEST_NO_MORE_SPACE);

    // chunking will be tried. this is for it to be failed :
    http_request_get_available_space_for_tuple_IgnoreAndReturn(0);

    // 4- Prepared http_request will be sent using http_client_send : 1 time :
    http_client_send_ExpectAnyArgsAndReturn(MCL_OK);
    http_client_send_ReturnThruPtr_http_response(&success_response);

    // 5- Request and response objects will be destroyed once each :
    http_response_destroy_Ignore();
    http_request_destroy_Ignore();

    // 6- Another http_request will be created for the second custom_data: 1 time :
    http_request_initialize_ExpectAnyArgsAndReturn(MCL_OK);
    http_request_initialize_ReturnThruPtr_http_request(&http_request);

    // 6.1- Add content-type header:
    http_request_add_header_IgnoreAndReturn(MCL_OK);
    http_request_add_header_IgnoreAndReturn(MCL_OK);
    http_request_finalize_IgnoreAndReturn(MCL_OK);

    // 7- // 3- Custom data will be added to the http_request by add_tuple : 1 times : Successful this time :
    http_request_add_tuple_IgnoreAndReturn(MCL_OK);

    // 8- Prepared http_request will be sent using http_client_send : 1 time :
    http_client_send_ExpectAnyArgsAndReturn(MCL_OK);
    http_client_send_ReturnThruPtr_http_response(&success_response);

    // 9- Request and response objects will be destroyed once each :
    http_response_destroy_Ignore();
    http_request_destroy_Ignore();

    result = http_processor_exchange(http_processor, store, NULL);
    TEST_ASSERT_EQUAL_INT_MESSAGE(MCL_OK, result, "Exchange operation is failed!");

    // check the item count in the store :
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, store->high_priority_list->count, "There are still remaining high priority data in the store!");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, store->low_priority_list->count, "There are still remaining low priority data in the store!");

    http_processor_destroy(&http_processor);
    mcl_store_destroy(&store);
}

// GIVEN : http_processor initialized - store initialized - 1 custom data added to store - no communication problem
// WHEN  : Payload size of the 1 custom data is less than the MAX http payload size. AND Error response is received from MindSphere.
// THEN  : Exchange operation is failed. All data in the store is remained.
void test_exchange_005(void)
{
    E_MCL_ERROR_CODE result = http_processor_initialize(configuration, &http_processor);
    TEST_ASSERT_EQUAL_INT_MESSAGE(MCL_OK, result, "http_processor_initialize failed!");

    http_processor->security_handler = security_handler;

    // create a store
    mcl_store_t *store = MCL_NULL;
    result = mcl_store_initialize(MCL_FALSE, &store);
    TEST_ASSERT_EQUAL_INT_MESSAGE(MCL_OK, result, "mcl_store_initialize failed!");

    // add a custom data to the store :
    mcl_uint8_t payload[] =
    {
        '0', '1', '2', '3', '4', '5', '6', '7', '8', '9'
    };
    mcl_custom_data_t *custom_data = MCL_NULL;
    result = mcl_store_new_custom_data(store, "1.0", "custom_data_type_1", routing, &custom_data);
    TEST_ASSERT_EQUAL_INT_MESSAGE(MCL_OK, result, "New custom data from the store failed!");
    custom_data->payload.buffer = payload;
    custom_data->payload.size = sizeof(payload) / sizeof(payload[0]);

    // define mocks before calling exchange :
    // 1- json_from_item_meta will be called for custom_data->meta : 1 time :
    json_from_item_meta_ExpectAnyArgsAndReturn(MCL_OK);

    string_t *json_meta = new_meta_json_string();
    json_from_item_meta_ReturnThruPtr_json_string(&json_meta);

    // 2- An http_request will be created : 1 time :
    http_request_initialize_ExpectAnyArgsAndReturn(MCL_OK);
    http_request_initialize_ReturnThruPtr_http_request(&http_request);

	// 2.1- Add headers : Content-type, Authorization, Correlation-ID
	http_request_add_header_IgnoreAndReturn(MCL_OK);
	http_request_add_header_IgnoreAndReturn(MCL_OK);
	http_request_add_header_IgnoreAndReturn(MCL_OK);

    http_request_finalize_IgnoreAndReturn(MCL_OK);

	// Correlation-ID
	security_generate_random_bytes_ExpectAnyArgsAndReturn(MCL_OK);

    // 3- Custom data will be added to the http_request by add_tuple : 1 time :
    // ( here it returns MCL_OK, which means adding it successful. http_processor_exchange shouldn't call it again. )
    http_request_add_tuple_IgnoreAndReturn(MCL_OK);

    // 4- Prepared http_request will be sent using http_client_send : 1 time :
    // MCL_HTTP_RESULT_CODE_UNAUTHORIZED returning :
    http_client_send_ExpectAnyArgsAndReturn(MCL_OK);
    http_client_send_ReturnThruPtr_http_response(&fail_response);

    // 5- Request and response objects will be destroyed once each :
    http_response_destroy_Ignore();
    http_request_destroy_Ignore();

    // 6- event set will be destroyed: 1 time :
    event_list_destroy_Ignore();

    // mock preperation is done. Calling the testing function :
    result = http_processor_exchange(http_processor, store, NULL);
    TEST_ASSERT_EQUAL_INT_MESSAGE(MCL_UNAUTHORIZED, result, "Exchange operation is failed!");

    // check the item count in the store :
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, store->high_priority_list->count, "Data is removed from high priority list eventhough opertation is failed!");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, store->low_priority_list->count, "There are still remaining low priority data in the store!");

    http_processor_destroy(&http_processor);
    mcl_store_destroy(&store);
}

// GIVEN : http_processor initialized - store initialized - 2 custom data added to store - no communication problem
// WHEN  : Payload size of the 2 custom data is less than the MAX http payload size. AND MindSphere will return NOT 200 OK.
// THEN  : Exchange operation is failed. All data in the store is remained.
void test_exchange_006(void)
{
    E_MCL_ERROR_CODE result = http_processor_initialize(configuration, &http_processor);
    TEST_ASSERT_EQUAL_INT_MESSAGE(MCL_OK, result, "http_processor_initialize failed!");

    http_processor->security_handler = security_handler;

    // create a store
    mcl_store_t *store = MCL_NULL;
    result = mcl_store_initialize(MCL_FALSE, &store);
    TEST_ASSERT_EQUAL_INT_MESSAGE(MCL_OK, result, "mcl_store_initialize failed!");

    // add 2 custom data to the store :
    mcl_uint8_t payload[] =
    {
        '0', '1', '2', '3', '4', '5', '6', '7', '8', '9'
    };
    mcl_custom_data_t *custom_data = MCL_NULL;

    // first custom_data:
    result = mcl_store_new_custom_data(store, "1.0", "custom_data_type_1", routing, &custom_data);
    TEST_ASSERT_EQUAL_INT_MESSAGE(MCL_OK, result, "New custom data from the store failed!");
    custom_data->payload.buffer = payload;
    custom_data->payload.size = sizeof(payload) / sizeof(payload[0]);

    // second custom_data :
    result = mcl_store_new_custom_data(store, "1.0", "custom_data_type_2", routing, &custom_data);
    TEST_ASSERT_EQUAL_INT_MESSAGE(MCL_OK, result, "New custom data from the store failed!");
    custom_data->payload.buffer = payload;
    custom_data->payload.size = sizeof(payload) / sizeof(payload[0]);

    // define mocks before calling exchange :
    // 1- json_from_item_meta will be called for custom_data->meta : 2 times :
    json_from_item_meta_ExpectAnyArgsAndReturn(MCL_OK);

    string_t *json_meta = new_meta_json_string();
    json_from_item_meta_ReturnThruPtr_json_string(&json_meta);
    json_from_item_meta_ExpectAnyArgsAndReturn(MCL_OK);

    string_t *json_meta_2 = new_meta_json_string();
    json_from_item_meta_ReturnThruPtr_json_string(&json_meta_2);

    // 2- An http_request will be created : 1 time :
    http_request_initialize_ExpectAnyArgsAndReturn(MCL_OK);
    http_request_initialize_ReturnThruPtr_http_request(&http_request);

	// 2.1- Add headers : Content-type, Authorization, Correlation-ID
	http_request_add_header_IgnoreAndReturn(MCL_OK);
	http_request_add_header_IgnoreAndReturn(MCL_OK);
	http_request_add_header_IgnoreAndReturn(MCL_OK);

    http_request_finalize_IgnoreAndReturn(MCL_OK);

	// Correlation-ID
	security_generate_random_bytes_ExpectAnyArgsAndReturn(MCL_OK);

    // 3- Custom data will be added to the http_request by add_tuple : 2 times :
    http_request_add_tuple_IgnoreAndReturn(MCL_OK);
    http_request_add_tuple_IgnoreAndReturn(MCL_OK);

    // 4- Prepared http_request will be sent using http_client_send : 1 time :
    // MCL_HTTP_RESULT_CODE_UNAUTHORIZED returning
    http_client_send_ExpectAnyArgsAndReturn(MCL_OK);
    http_client_send_ReturnThruPtr_http_response(&fail_response);

    // 5- Request and response objects will be destroyed once each :
    http_response_destroy_Ignore();
    http_request_destroy_Ignore();

    // 6- event set will be destroyed: 1 time :
    event_list_destroy_Ignore();

    // mock preparation is done. Calling the testing function :
    result = http_processor_exchange(http_processor, store, NULL);
    TEST_ASSERT_EQUAL_INT_MESSAGE(MCL_UNAUTHORIZED, result, "Exchange operation is failed!");

    // check the item count in the store :
    TEST_ASSERT_EQUAL_INT_MESSAGE(2, store->high_priority_list->count, "Data is removed from high priority list even though operation is failed!");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, store->low_priority_list->count, "There are still remaining low priority data in the store!");

    http_processor_destroy(&http_processor);
    mcl_store_destroy(&store);
}

// GIVEN : http_processor initialized - store initialized - 2 custom data added to store - no communication problem
// WHEN  : Payload size of the 1st custom data is less than the MAX http payload size.
// AND no space left from the first http_request after adding the first custom_data.
// AND Second custom_data will be added to a second http_request successfully.
// AND For the second send operation MindSphere returns error response.
// THEN  : Exchange operation is failed. All data in the store is remained.
void test_exchange_007(void)
{
    E_MCL_ERROR_CODE result = http_processor_initialize(configuration, &http_processor);
    TEST_ASSERT_EQUAL_INT_MESSAGE(MCL_OK, result, "http_processor_initialize failed!");

    http_processor->security_handler = security_handler;

    // create a store
    mcl_store_t *store = MCL_NULL;
    result = mcl_store_initialize(MCL_FALSE, &store);
    TEST_ASSERT_EQUAL_INT_MESSAGE(MCL_OK, result, "mcl_store_initialize failed!");

    // add 2 custom data to the store :
    mcl_uint8_t payload[] =
    {
        '0', '1', '2', '3', '4', '5', '6', '7', '8', '9'
    };
    mcl_custom_data_t *custom_data = MCL_NULL;
    mcl_custom_data_t *custom_data_2 = MCL_NULL;

    // first custom_data:
    result = mcl_store_new_custom_data(store, "1.0", "custom_data_type_1", routing, &custom_data);
    TEST_ASSERT_EQUAL_INT_MESSAGE(MCL_OK, result, "New custom data from the store failed!");
    custom_data->payload.buffer = payload;
    custom_data->payload.size = sizeof(payload) / sizeof(payload[0]);

    // second custom_data :
    result = mcl_store_new_custom_data(store, "1.0", "custom_data_type_2", routing, &custom_data_2);
    TEST_ASSERT_EQUAL_INT_MESSAGE(MCL_OK, result, "New custom data from the store failed!");
    custom_data_2->payload.buffer = payload;
    custom_data_2->payload.size = sizeof(payload) / sizeof(payload[0]);

    // define mocks before calling exchange :
    // 1- json_from_item_meta will be called for custom_data->meta : 2 times :
    json_from_item_meta_ExpectAnyArgsAndReturn(MCL_OK);

    string_t *json_meta = new_meta_json_string();
    json_from_item_meta_ReturnThruPtr_json_string(&json_meta);

    json_from_item_meta_ExpectAnyArgsAndReturn(MCL_OK);

    string_t *json_meta_2 = new_meta_json_string();
    json_from_item_meta_ReturnThruPtr_json_string(&json_meta_2);

    // 2- An http_request will be created : 1 time :
    http_request_initialize_ExpectAnyArgsAndReturn(MCL_OK);
    http_request_initialize_ReturnThruPtr_http_request(&http_request);

    // 2.1- Add headers : Content-type, Authorization
    http_request_add_header_IgnoreAndReturn(MCL_OK);
    http_request_add_header_IgnoreAndReturn(MCL_OK);

    http_request_finalize_IgnoreAndReturn(MCL_OK);

	// Correlation-ID
	security_generate_random_bytes_ExpectAnyArgsAndReturn(MCL_OK);
	security_generate_random_bytes_ExpectAnyArgsAndReturn(MCL_OK);

    // 3- Custom data will be added to the http_request by add_tuple : 2 times :
    // Second one will return error telling no more space left in the request
    http_request_add_tuple_IgnoreAndReturn(MCL_OK);
    http_request_add_tuple_IgnoreAndReturn(MCL_HTTP_REQUEST_NO_MORE_SPACE);

    // chunking will be tried. this is for it to be failed :
    http_request_get_available_space_for_tuple_IgnoreAndReturn(0);

    // http_processor_exchange mechanism will start over and try to add again and that should return error too. With that it will decide we can't add anything anymore :
    http_request_add_tuple_IgnoreAndReturn(MCL_HTTP_REQUEST_NO_MORE_SPACE);

    // chunking will be tried. this is for it to be failed :
    http_request_get_available_space_for_tuple_IgnoreAndReturn(0);

    // 4- Prepared http_request will be sent using http_client_send : 1 time :
    http_client_send_ExpectAnyArgsAndReturn(MCL_OK);
    http_client_send_ReturnThruPtr_http_response(&success_response);

    // 5- Request and response objects will be destroyed once each :
    http_response_destroy_Ignore();
    http_request_destroy_Ignore();

    // 6- Another http_request will be created for the second custom_data: 1 time :
    http_request_initialize_ExpectAnyArgsAndReturn(MCL_OK);
    http_request_initialize_ReturnThruPtr_http_request(&http_request_2);

    // 6.1- Add content-type header:
    http_request_add_header_IgnoreAndReturn(MCL_OK);
    http_request_add_header_IgnoreAndReturn(MCL_OK);

    http_request_finalize_IgnoreAndReturn(MCL_OK);

    // 7- // 3- Custom data will be added to the http_request by add_tuple : 1 times : Successful this time :
    http_request_add_tuple_IgnoreAndReturn(MCL_OK);

    // 8- Prepared http_request will be sent using http_client_send : 1 time :

    http_client_send_ExpectAnyArgsAndReturn(MCL_OK);

    http_client_send_ReturnThruPtr_http_response(&fail_response);

    // 9- Request and response objects will be destroyed once each :
    http_response_destroy_Ignore();
    http_request_destroy_Ignore();

    // 10- event set will be destroyed: 1 time :
    event_list_destroy_Ignore();

    result = http_processor_exchange(http_processor, store, NULL);
    TEST_ASSERT_EQUAL_INT_MESSAGE(MCL_UNAUTHORIZED, result, "Exchange operation is failed!");

    // check the item count in the store :
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, store->high_priority_list->count, "There are still remaining high priority data in the store!");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, store->low_priority_list->count, "There are still remaining low priority data in the store!");

    http_processor_destroy(&http_processor);
    mcl_store_destroy(&store);
}

// GIVEN : http_processor initialized - store initialized - 1 time series added to store - no communication problem
// WHEN  : Payload size of the 1 time series is less than the MAX http payload size.
// THEN  : Exchange operation would successfully add the time series to the request and send it
void test_exchange_008(void)
{
    E_MCL_ERROR_CODE result = http_processor_initialize(configuration, &http_processor);
    TEST_ASSERT_EQUAL_INT_MESSAGE(MCL_OK, result, "http_processor_initialize failed!");

    http_processor->security_handler = security_handler;

    // create a store
    mcl_store_t *store = MCL_NULL;
    result = mcl_store_initialize(MCL_FALSE, &store);
    TEST_ASSERT_EQUAL_INT_MESSAGE(MCL_OK, result, "mcl_store_initialize failed!");

    // add a time series to the store :
    mcl_time_series_t *time_series = MCL_NULL;
    result = mcl_store_new_time_series(store, "1.0", "time_series_type_1", MCL_NULL, &time_series);
    TEST_ASSERT_EQUAL_INT_MESSAGE(MCL_OK, result, "New time series from the store failed!");

    time_util_validate_timestamp_IgnoreAndReturn(MCL_TRUE);

    mcl_time_series_value_set_t *ts_value_set = MCL_NULL;
    result = mcl_time_series_new_value_set(time_series, timestamp, &ts_value_set);
    TEST_ASSERT_EQUAL_INT_MESSAGE(MCL_OK, result, "New payload from time series failed!");

    result = mcl_time_series_add_value(ts_value_set, "data_point_id_1", "value_1", "quality_code_1");
    TEST_ASSERT_EQUAL_INT_MESSAGE(MCL_OK, result, "Add value to payload failed!");

    // define mocks before calling exchange :
    // 1- json_from_item_meta will be called for time_series->meta : 1 time :
    json_from_item_meta_ExpectAnyArgsAndReturn(MCL_OK);

    string_t *json_meta = new_meta_json_string();
    json_from_item_meta_ReturnThruPtr_json_string(&json_meta);
    json_from_time_series_payload_ExpectAnyArgsAndReturn(MCL_OK);

    string_t *json_payload = new_time_series_payload_json_string();
    json_from_time_series_payload_ReturnThruPtr_json_string(&json_payload);

    // 2- An http_request will be created : 1 time :
    http_request_initialize_ExpectAnyArgsAndReturn(MCL_OK);
    http_request_initialize_ReturnThruPtr_http_request(&http_request);

	// 2.1- Add headers : Content-type, Authorization, Correlation-ID
	http_request_add_header_IgnoreAndReturn(MCL_OK);
	http_request_add_header_IgnoreAndReturn(MCL_OK);
	http_request_add_header_IgnoreAndReturn(MCL_OK);

    http_request_finalize_IgnoreAndReturn(MCL_OK);

	// Correlation-ID
	security_generate_random_bytes_ExpectAnyArgsAndReturn(MCL_OK);

    // 3- time series will be added to the http_request by add_tuple : 1 time :
    // ( here it returns MCL_OK, which means adding it successful. http_processor_exchange shouldn't call it again. )
    http_request_add_tuple_IgnoreAndReturn(MCL_OK);

    // 4- Prepared http_request will be sent using http_client_send : 1 time :
    success_response->payload = "";
    http_client_send_ExpectAnyArgsAndReturn(MCL_OK);
    http_client_send_ReturnThruPtr_http_response(&success_response);

    // 5- Request and response objects will be destroyed once each :
    http_response_destroy_Ignore();
    http_request_destroy_Ignore();

    // 6- event set object will be destroyed once each : 1 time.
    event_list_destroy_Ignore();

    result = http_processor_exchange(http_processor, store, NULL);
    TEST_ASSERT_EQUAL_INT_MESSAGE(MCL_OK, result, "Exchange operation is failed!");

    // check the item count in the store :
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, store->high_priority_list->count, "There are still remaining high priority data in the store!");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, store->low_priority_list->count, "There are still remaining low priority data in the store!");

    http_processor_destroy(&http_processor);
    mcl_store_destroy(&store);
}

// GIVEN : http_processor initialized - store initialized - 2 time series added to store - no communication problem
// WHEN  : Payload size of the 2 time series is less than the MAX http payload size.
// THEN  : Exchange operation would successfully add the time series to the request and send it
void test_exchange_009(void)
{
    E_MCL_ERROR_CODE result = http_processor_initialize(configuration, &http_processor);
    TEST_ASSERT_EQUAL_INT_MESSAGE(MCL_OK, result, "http_processor_initialize failed!");

    http_processor->security_handler = security_handler;

    // create a store
    mcl_store_t *store = MCL_NULL;
    result = mcl_store_initialize(MCL_FALSE, &store);
    TEST_ASSERT_EQUAL_INT_MESSAGE(MCL_OK, result, "mcl_store_initialize failed!");

    // add 2 time series to the store :
    mcl_time_series_t *time_series = MCL_NULL;

    // first time_series:
    result = mcl_store_new_time_series(store, "1.0", "time_series_type_1", MCL_NULL, &time_series);
    TEST_ASSERT_EQUAL_INT_MESSAGE(MCL_OK, result, "New time series from the store failed!");

    time_util_validate_timestamp_IgnoreAndReturn(MCL_TRUE);

    mcl_time_series_value_set_t *ts_value_set = MCL_NULL;
    result = mcl_time_series_new_value_set(time_series, timestamp, &ts_value_set);
    TEST_ASSERT_EQUAL_INT_MESSAGE(MCL_OK, result, "New payload from time series failed!");

    result = mcl_time_series_add_value(ts_value_set, "data_point_id_1", "value_1", "quality_code_1");
    TEST_ASSERT_EQUAL_INT_MESSAGE(MCL_OK, result, "Add value to payload failed!");

    // second time_series :
    result = mcl_store_new_time_series(store, "1.0", "time_series_type_2", MCL_NULL, &time_series);
    TEST_ASSERT_EQUAL_INT_MESSAGE(MCL_OK, result, "New time series from the store failed!");

    result = mcl_time_series_new_value_set(time_series, timestamp, &ts_value_set);
    TEST_ASSERT_EQUAL_INT_MESSAGE(MCL_OK, result, "New payload from time series failed!");

    result = mcl_time_series_add_value(ts_value_set, "data_point_id_2", "value_2", "quality_code_2");
    TEST_ASSERT_EQUAL_INT_MESSAGE(MCL_OK, result, "Add value to payload failed!");

    // define mocks before calling exchange :
    // 1- json_from_item_meta will be called for time_series->meta : 2 times :
    json_from_item_meta_ExpectAnyArgsAndReturn(MCL_OK);

    string_t *json_meta = new_meta_json_string();
    json_from_item_meta_ReturnThruPtr_json_string(&json_meta);
    json_from_item_meta_ExpectAnyArgsAndReturn(MCL_OK);

    string_t *json_meta_2 = new_meta_json_string();
    json_from_item_meta_ReturnThruPtr_json_string(&json_meta_2);

    json_from_time_series_payload_ExpectAnyArgsAndReturn(MCL_OK);

    string_t *json_payload = new_time_series_payload_json_string();
    json_from_time_series_payload_ReturnThruPtr_json_string(&json_payload);
    json_from_time_series_payload_ExpectAnyArgsAndReturn(MCL_OK);

    string_t *json_payload_2 = new_time_series_payload_json_string();
    json_from_time_series_payload_ReturnThruPtr_json_string(&json_payload_2);

    // 2- An http_request will be created : 1 time :
    http_request_initialize_ExpectAnyArgsAndReturn(MCL_OK);
    http_request_initialize_ReturnThruPtr_http_request(&http_request);

	// 2.1- Add headers : Content-type, Authorization, Correlation-ID
	http_request_add_header_IgnoreAndReturn(MCL_OK);
	http_request_add_header_IgnoreAndReturn(MCL_OK);
	http_request_add_header_IgnoreAndReturn(MCL_OK);

    http_request_finalize_IgnoreAndReturn(MCL_OK);

	// Correlation-ID
	security_generate_random_bytes_ExpectAnyArgsAndReturn(MCL_OK);

    // 3- time series will be added to the http_request by add_tuple : 2 times :
    http_request_add_tuple_IgnoreAndReturn(MCL_OK);
    http_request_add_tuple_IgnoreAndReturn(MCL_OK);

    // 4- Prepared http_request will be sent using http_client_send : 1 time :
    success_response->payload = "";
    http_client_send_ExpectAnyArgsAndReturn(MCL_OK);
    http_client_send_ReturnThruPtr_http_response(&success_response);

    // 5- Request and response objects will be destroyed once each :
    http_response_destroy_Ignore();
    http_request_destroy_Ignore();

    // 6- event set object will be destroyed once each : 1 time.
    event_list_destroy_Ignore();

    result = http_processor_exchange(http_processor, store, NULL);
    TEST_ASSERT_EQUAL_INT_MESSAGE(MCL_OK, result, "Exchange operation is failed!");

    // check the item count in the store :
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, store->high_priority_list->count, "There are still remaining high priority data in the store!");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, store->low_priority_list->count, "There are still remaining low priority data in the store!");

    http_processor_destroy(&http_processor);
    mcl_store_destroy(&store);
}

// GIVEN : http_processor initialized - store initialized - 2 time series added to store - no communication problem
// WHEN  : Payload size of the 1st time series is less than the MAX http payload size.
// AND no space left from the first http_request after adding the first time_series.
// AND Second time_series will be added to a second http_request successfully.
// THEN  : Exchange operation would successfully add the time series to the request and send it
void test_exchange_010(void)
{
    E_MCL_ERROR_CODE result = http_processor_initialize(configuration, &http_processor);
    TEST_ASSERT_EQUAL_INT_MESSAGE(MCL_OK, result, "http_processor_initialize failed!");

    http_processor->security_handler = security_handler;

    // create a store
    mcl_store_t *store = MCL_NULL;
    result = mcl_store_initialize(MCL_FALSE, &store);
    TEST_ASSERT_EQUAL_INT_MESSAGE(MCL_OK, result, "mcl_store_initialize failed!");

    // add 2 time series to the store :
    mcl_time_series_t *time_series = MCL_NULL;
    mcl_time_series_t *time_series_2 = MCL_NULL;

    // first time_series:
    result = mcl_store_new_time_series(store, "1.0", "time_series_type_1", MCL_NULL, &time_series);
    TEST_ASSERT_EQUAL_INT_MESSAGE(MCL_OK, result, "New time series from the store failed!");

    time_util_validate_timestamp_IgnoreAndReturn(MCL_TRUE);

    mcl_time_series_value_set_t *ts_value_set = MCL_NULL;
    result = mcl_time_series_new_value_set(time_series, timestamp, &ts_value_set);
    TEST_ASSERT_EQUAL_INT_MESSAGE(MCL_OK, result, "New payload from time series failed!");

    result = mcl_time_series_add_value(ts_value_set, "data_point_id_1", "value_1", "quality_code_1");
    TEST_ASSERT_EQUAL_INT_MESSAGE(MCL_OK, result, "Add value to payload failed!");

    // second time_series :
    result = mcl_store_new_time_series(store, "1.0", "time_series_type_2", MCL_NULL, &time_series_2);
    TEST_ASSERT_EQUAL_INT_MESSAGE(MCL_OK, result, "New time series from the store failed!");

    mcl_time_series_value_set_t *ts_value_set_2 = MCL_NULL;
    result = mcl_time_series_new_value_set(time_series_2, timestamp, &ts_value_set_2);
    TEST_ASSERT_EQUAL_INT_MESSAGE(MCL_OK, result, "New payload from time series failed!");

    result = mcl_time_series_add_value(ts_value_set_2, "data_point_id_1", "value_1", "quality_code_1");
    TEST_ASSERT_EQUAL_INT_MESSAGE(MCL_OK, result, "Add value to payload failed!");

    // define mocks before calling exchange :
    // 1- json_from_item_meta will be called for time_series->meta : 2 times :
    json_from_item_meta_ExpectAnyArgsAndReturn(MCL_OK);

    string_t *json_meta = new_meta_json_string();
    json_from_item_meta_ReturnThruPtr_json_string(&json_meta);
    json_from_item_meta_ExpectAnyArgsAndReturn(MCL_OK);

    string_t *json_meta_2 = new_meta_json_string();
    json_from_item_meta_ReturnThruPtr_json_string(&json_meta_2);

    json_from_time_series_payload_ExpectAnyArgsAndReturn(MCL_OK);

    string_t *json_payload = new_time_series_payload_json_string();
    json_from_time_series_payload_ReturnThruPtr_json_string(&json_payload);
    json_from_time_series_payload_ExpectAnyArgsAndReturn(MCL_OK);

    string_t *json_payload_2 = new_time_series_payload_json_string();
    json_from_time_series_payload_ReturnThruPtr_json_string(&json_payload_2);

    // 2- An http_request will be created : 1 time :
    http_request_initialize_ExpectAnyArgsAndReturn(MCL_OK);
    http_request_initialize_ReturnThruPtr_http_request(&http_request);

	// 2.1- Add headers : Content-type, Authorization, Correlation-ID
	http_request_add_header_IgnoreAndReturn(MCL_OK);
	http_request_add_header_IgnoreAndReturn(MCL_OK);
	http_request_add_header_IgnoreAndReturn(MCL_OK);

    http_request_finalize_IgnoreAndReturn(MCL_OK);

	// Correlation-ID
	security_generate_random_bytes_ExpectAnyArgsAndReturn(MCL_OK);

    // 3- time series will be added to the http_request by add_tuple : 2 times :
    // Second one will return error telling no more space left in the request
    http_request_add_tuple_IgnoreAndReturn(MCL_OK);
    http_request_add_tuple_IgnoreAndReturn(MCL_HTTP_REQUEST_NO_MORE_SPACE);

    // http_processor_exchange mechanism will start over and try to add again and that should return error too. With that it will decide we can't add anything anymore :
    http_request_add_tuple_IgnoreAndReturn(MCL_HTTP_REQUEST_NO_MORE_SPACE);

    // 4- Prepared http_request will be sent using http_client_send : 1 time :
    success_response->payload = "";
    http_client_send_ExpectAnyArgsAndReturn(MCL_OK);
    http_client_send_ReturnThruPtr_http_response(&success_response);

    // 5- Request and response objects will be destroyed once each :
    http_response_destroy_Ignore();
    http_request_destroy_Ignore();

    // 6- Another http_request will be created for the second time_series: 1 time :
    http_request_initialize_ExpectAnyArgsAndReturn(MCL_OK);
    http_request_initialize_ReturnThruPtr_http_request(&http_request);

    // 6.1- Add content-type header:
    http_request_add_header_IgnoreAndReturn(MCL_OK);
    http_request_add_header_IgnoreAndReturn(MCL_OK);
	http_request_add_header_IgnoreAndReturn(MCL_OK);

    http_request_finalize_IgnoreAndReturn(MCL_OK);

	// Correlation-ID
	security_generate_random_bytes_ExpectAnyArgsAndReturn(MCL_OK);

    // 7- // 3- time series will be added to the http_request by add_tuple : 1 times : Successful this time :
    http_request_add_tuple_IgnoreAndReturn(MCL_OK);

    // 8- Prepared http_request will be sent using http_client_send : 1 time :
    http_client_send_ExpectAnyArgsAndReturn(MCL_OK);
    http_client_send_ReturnThruPtr_http_response(&success_response);

    // 9- Request and response objects will be destroyed once each :
    http_response_destroy_Ignore();
    http_request_destroy_Ignore();

    // 10- event set object will be destroyed once each : 1 time.
    event_list_destroy_Ignore();

    result = http_processor_exchange(http_processor, store, NULL);
    TEST_ASSERT_EQUAL_INT_MESSAGE(MCL_OK, result, "Exchange operation is failed!");

    // check the item count in the store :
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, store->high_priority_list->count, "There are still remaining high priority data in the store!");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, store->low_priority_list->count, "There are still remaining low priority data in the store!");

    http_processor_destroy(&http_processor);
    mcl_store_destroy(&store);
}

// GIVEN : http_processor initialized - store initialized - 1 time series added to store - no communication problem
// WHEN  : Payload size of the 1 time series is less than the MAX http payload size. AND Error response is received from MindSphere.
// THEN  : Exchange operation is failed. All data in the store is remained.
void test_exchange_011(void)
{
    E_MCL_ERROR_CODE result = http_processor_initialize(configuration, &http_processor);
    TEST_ASSERT_EQUAL_INT_MESSAGE(MCL_OK, result, "http_processor_initialize failed!");

    http_processor->security_handler = security_handler;

    // create a store
    mcl_store_t *store = MCL_NULL;
    result = mcl_store_initialize(MCL_FALSE, &store);
    TEST_ASSERT_EQUAL_INT_MESSAGE(MCL_OK, result, "mcl_store_initialize failed!");

    // add a time series to the store :
    mcl_time_series_t *time_series = MCL_NULL;
    result = mcl_store_new_time_series(store, "1.0", "time_series_type_1", MCL_NULL, &time_series);
    TEST_ASSERT_EQUAL_INT_MESSAGE(MCL_OK, result, "New time series from the store failed!");

    time_util_validate_timestamp_IgnoreAndReturn(MCL_TRUE);

    mcl_time_series_value_set_t *ts_value_set = MCL_NULL;
    result = mcl_time_series_new_value_set(time_series, timestamp, &ts_value_set);
    TEST_ASSERT_EQUAL_INT_MESSAGE(MCL_OK, result, "New payload from time series failed!");

    result = mcl_time_series_add_value(ts_value_set, "data_point_id_1", "value_1", "quality_code_1");
    TEST_ASSERT_EQUAL_INT_MESSAGE(MCL_OK, result, "Add value to payload failed!");

    // define mocks before calling exchange :
    // 1- json_from_item_meta will be called for time_series->meta : 1 time :
    json_from_item_meta_ExpectAnyArgsAndReturn(MCL_OK);

    string_t *json_meta = new_meta_json_string();
    json_from_item_meta_ReturnThruPtr_json_string(&json_meta);
    json_from_time_series_payload_ExpectAnyArgsAndReturn(MCL_OK);

    string_t *json_payload = new_time_series_payload_json_string();
    json_from_time_series_payload_ReturnThruPtr_json_string(&json_payload);

    // 2- An http_request will be created : 1 time :
    http_request_initialize_ExpectAnyArgsAndReturn(MCL_OK);
    http_request_initialize_ReturnThruPtr_http_request(&http_request);

	// 2.1- Add headers : Content-type, Authorization, Correlation-ID
	http_request_add_header_IgnoreAndReturn(MCL_OK);
	http_request_add_header_IgnoreAndReturn(MCL_OK);
	http_request_add_header_IgnoreAndReturn(MCL_OK);

    http_request_finalize_IgnoreAndReturn(MCL_OK);

	// Correlation-ID
	security_generate_random_bytes_ExpectAnyArgsAndReturn(MCL_OK);

    // 3- time series will be added to the http_request by add_tuple : 1 time :
    // ( here it returns MCL_OK, which means adding it successful. http_processor_exchange shouldn't call it again. )
    http_request_add_tuple_IgnoreAndReturn(MCL_OK);

    // 4- Prepared http_request will be sent using http_client_send : 1 time :
    // MCL_HTTP_RESULT_CODE_UNAUTHORIZED returning :
    http_client_send_ExpectAnyArgsAndReturn(MCL_OK);
    http_client_send_ReturnThruPtr_http_response(&fail_response);

    // 5- Request and response objects will be destroyed once each :
    http_response_destroy_Ignore();
    http_request_destroy_Ignore();

    // 6- event set will be destroyed: 1 time :
    event_list_destroy_Ignore();

    result = http_processor_exchange(http_processor, store, NULL);
    TEST_ASSERT_EQUAL_INT_MESSAGE(MCL_UNAUTHORIZED, result, "Exchange operation is failed!");

    // check the item count in the store :
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, store->high_priority_list->count, "Data is removed from high priority list eventhough opertation is failed!");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, store->low_priority_list->count, "There are still remaining low priority data in the store!");

    http_processor_destroy(&http_processor);
    mcl_store_destroy(&store);
}

// GIVEN : http_processor initialized - store initialized - 2 time series added to store - no communication problem
// WHEN  : Payload size of the 2 time series is less than the MAX http payload size. AND MindSphere will return NOT 200 OK.
// THEN  : Exchange operation is failed. All data in the store is remained.
void test_exchange_012(void)
{
    E_MCL_ERROR_CODE result = http_processor_initialize(configuration, &http_processor);
    TEST_ASSERT_EQUAL_INT_MESSAGE(MCL_OK, result, "http_processor_initialize failed!");

    http_processor->security_handler = security_handler;

    // create a store
    mcl_store_t *store = MCL_NULL;
    result = mcl_store_initialize(MCL_FALSE, &store);
    TEST_ASSERT_EQUAL_INT_MESSAGE(MCL_OK, result, "mcl_store_initialize failed!");

    // add 2 time series to the store :
    mcl_time_series_t *time_series = MCL_NULL;

    // first time_series:
    result = mcl_store_new_time_series(store, "1.0", "time_series_type_1", MCL_NULL, &time_series);
    TEST_ASSERT_EQUAL_INT_MESSAGE(MCL_OK, result, "New time series from the store failed!");

    time_util_validate_timestamp_IgnoreAndReturn(MCL_TRUE);

    mcl_time_series_value_set_t *ts_value_set = MCL_NULL;
    result = mcl_time_series_new_value_set(time_series, timestamp, &ts_value_set);
    TEST_ASSERT_EQUAL_INT_MESSAGE(MCL_OK, result, "New payload from time series failed!");

    result = mcl_time_series_add_value(ts_value_set, "data_point_id_1", "value_1", "quality_code_1");
    TEST_ASSERT_EQUAL_INT_MESSAGE(MCL_OK, result, "Add value to payload failed!");

    // second time_series :
    result = mcl_store_new_time_series(store, "1.0", "time_series_type_2", MCL_NULL, &time_series);
    TEST_ASSERT_EQUAL_INT_MESSAGE(MCL_OK, result, "New time series from the store failed!");
    ts_value_set = MCL_NULL;
    result = mcl_time_series_new_value_set(time_series, timestamp, &ts_value_set);
    TEST_ASSERT_EQUAL_INT_MESSAGE(MCL_OK, result, "New payload from time series failed!");

    result = mcl_time_series_add_value(ts_value_set, "data_point_id_1", "value_1", "quality_code_1");
    TEST_ASSERT_EQUAL_INT_MESSAGE(MCL_OK, result, "Add value to payload failed!");

    // define mocks before calling exchange :
    // 1- json_from_item_meta will be called for time_series->meta : 2 times :
    json_from_item_meta_ExpectAnyArgsAndReturn(MCL_OK);

    string_t *json_meta = new_meta_json_string();
    json_from_item_meta_ReturnThruPtr_json_string(&json_meta);
    json_from_item_meta_ExpectAnyArgsAndReturn(MCL_OK);

    string_t *json_meta_2 = new_meta_json_string();
    json_from_item_meta_ReturnThruPtr_json_string(&json_meta_2);

    json_from_time_series_payload_ExpectAnyArgsAndReturn(MCL_OK);

    string_t *json_payload = new_time_series_payload_json_string();
    json_from_time_series_payload_ReturnThruPtr_json_string(&json_payload);
    json_from_time_series_payload_ExpectAnyArgsAndReturn(MCL_OK);

    string_t *json_payload_2 = new_time_series_payload_json_string();
    json_from_time_series_payload_ReturnThruPtr_json_string(&json_payload_2);

    // 2- An http_request will be created : 1 time :
    http_request_initialize_ExpectAnyArgsAndReturn(MCL_OK);
    http_request_initialize_ReturnThruPtr_http_request(&http_request);

	// 2.1- Add headers : Content-type, Authorization, Correlation-ID
	http_request_add_header_IgnoreAndReturn(MCL_OK);
	http_request_add_header_IgnoreAndReturn(MCL_OK);
	http_request_add_header_IgnoreAndReturn(MCL_OK);

    http_request_finalize_IgnoreAndReturn(MCL_OK);
	
	// Correlation-ID
	security_generate_random_bytes_ExpectAnyArgsAndReturn(MCL_OK);

    // 3- time series will be added to the http_request by add_tuple : 2 times :
    http_request_add_tuple_IgnoreAndReturn(MCL_OK);
    http_request_add_tuple_IgnoreAndReturn(MCL_OK);

    // 4- Prepared http_request will be sent using http_client_send : 1 time :
    // MCL_HTTP_RESULT_CODE_UNAUTHORIZED returning
    http_client_send_ExpectAnyArgsAndReturn(MCL_OK);
    http_client_send_ReturnThruPtr_http_response(&fail_response);

    // 5- Request and response objects will be destroyed once each :
    http_response_destroy_Ignore();
    http_request_destroy_Ignore();

    // 6- event set will be destroyed: 1 time :
    event_list_destroy_Ignore();

    result = http_processor_exchange(http_processor, store, NULL);
    TEST_ASSERT_EQUAL_INT_MESSAGE(MCL_UNAUTHORIZED, result, "Exchange operation is failed!");

    // check the item count in the store :
    TEST_ASSERT_EQUAL_INT_MESSAGE(2, store->high_priority_list->count, "Data is removed from high priority list eventhough opertation is failed!");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, store->low_priority_list->count, "There are still remaining low priority data in the store!");

    http_processor_destroy(&http_processor);
    mcl_store_destroy(&store);
}

// GIVEN : http_processor initialized - store initialized - 2 time series added to store - no communication problem
// WHEN  : Payload size of the 1st time series is less than the MAX http payload size.
// AND no space left from the first http_request after adding the first time_series.
// AND Second time_series will be added to a second http_request successfully.
// AND For the second send operation MindSphere returns error response.
// THEN  : Exchange operation is failed. All data in the store is remained.
void test_exchange_013(void)
{
    E_MCL_ERROR_CODE result = http_processor_initialize(configuration, &http_processor);
    TEST_ASSERT_EQUAL_INT_MESSAGE(MCL_OK, result, "http_processor_initialize failed!");

    http_processor->security_handler = security_handler;

    // create a store
    mcl_store_t *store = MCL_NULL;
    result = mcl_store_initialize(MCL_FALSE, &store);
    TEST_ASSERT_EQUAL_INT_MESSAGE(MCL_OK, result, "mcl_store_initialize failed!");

    // add 2 time series to the store :
    mcl_time_series_t *time_series = MCL_NULL;
    mcl_time_series_t *time_series_2 = MCL_NULL;

    // first time_series:
    result = mcl_store_new_time_series(store, "1.0", "time_series_type_1", MCL_NULL, &time_series);
    TEST_ASSERT_EQUAL_INT_MESSAGE(MCL_OK, result, "New time series from the store failed!");

    time_util_validate_timestamp_IgnoreAndReturn(MCL_TRUE);

    mcl_time_series_value_set_t *ts_value_set = MCL_NULL;
    result = mcl_time_series_new_value_set(time_series, timestamp, &ts_value_set);
    TEST_ASSERT_EQUAL_INT_MESSAGE(MCL_OK, result, "New payload from time series failed!");

    result = mcl_time_series_add_value(ts_value_set, "data_point_id_1", "value_1", "quality_code_1");
    TEST_ASSERT_EQUAL_INT_MESSAGE(MCL_OK, result, "Add value to payload failed!");

    // second time_series :
    result = mcl_store_new_time_series(store, "1.0", "time_series_type_2", MCL_NULL, &time_series_2);
    TEST_ASSERT_EQUAL_INT_MESSAGE(MCL_OK, result, "New time series from the store failed!");

    mcl_time_series_value_set_t *ts_value_set_2 = MCL_NULL;
    result = mcl_time_series_new_value_set(time_series_2, timestamp, &ts_value_set_2);
    TEST_ASSERT_EQUAL_INT_MESSAGE(MCL_OK, result, "New payload from time series failed!");

    result = mcl_time_series_add_value(ts_value_set_2, "data_point_id_1", "value_1", "quality_code_1");
    TEST_ASSERT_EQUAL_INT_MESSAGE(MCL_OK, result, "Add value to payload failed!");

    // define mocks before calling exchange :
    // 1- json_from_item_meta will be called for time_series->meta : 2 times :
    json_from_item_meta_ExpectAnyArgsAndReturn(MCL_OK);

    string_t *json_meta = new_meta_json_string();
    json_from_item_meta_ReturnThruPtr_json_string(&json_meta);
    json_from_item_meta_ExpectAnyArgsAndReturn(MCL_OK);

    string_t *json_meta_2 = new_meta_json_string();
    json_from_item_meta_ReturnThruPtr_json_string(&json_meta_2);

    json_from_time_series_payload_ExpectAnyArgsAndReturn(MCL_OK);

    string_t *json_payload = new_time_series_payload_json_string();
    json_from_time_series_payload_ReturnThruPtr_json_string(&json_payload);
    json_from_time_series_payload_ExpectAnyArgsAndReturn(MCL_OK);

    string_t *json_payload_2 = new_time_series_payload_json_string();
    json_from_time_series_payload_ReturnThruPtr_json_string(&json_payload_2);

    // 2- An http_request will be created : 1 time :
    http_request_initialize_ExpectAnyArgsAndReturn(MCL_OK);
    http_request_initialize_ReturnThruPtr_http_request(&http_request);

	// 2.1- Add headers : Content-type, Authorization, Correlation-ID
	http_request_add_header_IgnoreAndReturn(MCL_OK);
	http_request_add_header_IgnoreAndReturn(MCL_OK);
	http_request_add_header_IgnoreAndReturn(MCL_OK);

    http_request_finalize_IgnoreAndReturn(MCL_OK);

	// Correlation-ID
	security_generate_random_bytes_ExpectAnyArgsAndReturn(MCL_OK);

    // 3- time series will be added to the http_request by add_tuple : 2 times :
    // Second one will return error telling no more space left in the request
    http_request_add_tuple_IgnoreAndReturn(MCL_OK);
    http_request_add_tuple_IgnoreAndReturn(MCL_HTTP_REQUEST_NO_MORE_SPACE);

    // http_processor_exchange mechanism will start over and try to add again and that should return error too. With that it will decide we can't add anything anymore :
    http_request_add_tuple_IgnoreAndReturn(MCL_HTTP_REQUEST_NO_MORE_SPACE);

    // 4- Prepared http_request will be sent using http_client_send : 1 time :
    http_client_send_ExpectAnyArgsAndReturn(MCL_OK);
    http_client_send_ReturnThruPtr_http_response(&success_response);

    // 5- Request and response objects will be destroyed once each :
    http_response_destroy_Ignore();
    http_request_destroy_Ignore();

    // 6- Another http_request will be created for the second time_series: 1 time :
    http_request_initialize_ExpectAnyArgsAndReturn(MCL_OK);
    http_request_initialize_ReturnThruPtr_http_request(&http_request_2);

    // 6.1- Add content-type header:
    http_request_add_header_IgnoreAndReturn(MCL_OK);
    http_request_add_header_IgnoreAndReturn(MCL_OK);
	http_request_add_header_IgnoreAndReturn(MCL_OK);

    http_request_finalize_IgnoreAndReturn(MCL_OK);

	// Correlation-ID
	security_generate_random_bytes_ExpectAnyArgsAndReturn(MCL_OK);

    // 7- // 3- time series will be added to the http_request by add_tuple : 1 times : Successful this time :
    http_request_add_tuple_IgnoreAndReturn(MCL_OK);

    // 8- Prepared http_request will be sent using http_client_send : 1 time :

    http_client_send_ExpectAnyArgsAndReturn(MCL_OK);

    http_client_send_ReturnThruPtr_http_response(&fail_response);

    // 9- Request and response objects will be destroyed once each :
    http_response_destroy_Ignore();
    http_request_destroy_Ignore();

    // 10- event set will be destroyed: 1 time :
    event_list_destroy_Ignore();

    result = http_processor_exchange(http_processor, store, NULL);
    TEST_ASSERT_EQUAL_INT_MESSAGE(MCL_UNAUTHORIZED, result, "Exchange operation is failed!");

    // check the item count in the store :
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, store->high_priority_list->count, "There are still remaining high priority data in the store!");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, store->low_priority_list->count, "There are still remaining low priority data in the store!");

    http_processor_destroy(&http_processor);
    mcl_store_destroy(&store);
}

// GIVEN : http_processor initialized - store initialized - 1 time series and 1 custom data added to store - no communication problem
// WHEN  : Payload size of the store items are less than the MAX http payload size.
// THEN  : Exchange operation is success. All data in the store is sent and removed.
void test_exchange_014(void)
{
    E_MCL_ERROR_CODE result = http_processor_initialize(configuration, &http_processor);
    TEST_ASSERT_EQUAL_INT_MESSAGE(MCL_OK, result, "http_processor_initialize failed!");

    http_processor->security_handler = security_handler;

    // create a store
    mcl_store_t *store = MCL_NULL;
    result = mcl_store_initialize(MCL_FALSE, &store);
    TEST_ASSERT_EQUAL_INT_MESSAGE(MCL_OK, result, "mcl_store_initialize failed!");

    // add 1 time series to the store :
    mcl_time_series_t *time_series = MCL_NULL;

    result = mcl_store_new_time_series(store, "1.0", "time_series_type_1", MCL_NULL, &time_series);
    TEST_ASSERT_EQUAL_INT_MESSAGE(MCL_OK, result, "New time series from the store failed!");

    time_util_validate_timestamp_IgnoreAndReturn(MCL_TRUE);

    mcl_time_series_value_set_t *ts_payload = MCL_NULL;
    result = mcl_time_series_new_value_set(time_series, timestamp, &ts_payload);
    TEST_ASSERT_EQUAL_INT_MESSAGE(MCL_OK, result, "New payload from time series failed!");

    result = mcl_time_series_add_value(ts_payload, "data_point_id_1", "value_1", "quality_code_1");
    TEST_ASSERT_EQUAL_INT_MESSAGE(MCL_OK, result, "Add value to payload failed!");

    // add 1 custom data to the store :
    mcl_uint8_t payload[] =
    {
        '0', '1', '2', '3', '4', '5', '6', '7', '8', '9'
    };
    mcl_custom_data_t *custom_data = MCL_NULL;

    result = mcl_store_new_custom_data(store, "1.0", "custom_data_type_1", routing, &custom_data);
    TEST_ASSERT_EQUAL_INT_MESSAGE(MCL_OK, result, "New custom data from the store failed!");
    custom_data->payload.buffer = payload;
    custom_data->payload.size = sizeof(payload) / sizeof(payload[0]);

    // define mocks before calling exchange :
    // 1- json_from_item_meta will be called for time_series->meta and custom_data->meta: 1 time each:
    json_from_item_meta_ExpectAnyArgsAndReturn(MCL_OK);

    string_t *json_meta = new_meta_json_string();
    json_from_item_meta_ReturnThruPtr_json_string(&json_meta);
    json_from_item_meta_ExpectAnyArgsAndReturn(MCL_OK);

    string_t *json_meta_2 = new_meta_json_string();
    json_from_item_meta_ReturnThruPtr_json_string(&json_meta_2);

    // json_from_time_series_payload will be called only for time_series data :
    json_from_time_series_payload_ExpectAnyArgsAndReturn(MCL_OK);

    string_t *json_payload = new_time_series_payload_json_string();
    json_from_time_series_payload_ReturnThruPtr_json_string(&json_payload);

    // 2- An http_request will be created : 1 time :
    http_request_initialize_ExpectAnyArgsAndReturn(MCL_OK);
    http_request_initialize_ReturnThruPtr_http_request(&http_request);

	// 2.1- Add headers : Content-type, Authorization, Correlation-ID
	http_request_add_header_IgnoreAndReturn(MCL_OK);
	http_request_add_header_IgnoreAndReturn(MCL_OK);
	http_request_add_header_IgnoreAndReturn(MCL_OK);

    http_request_finalize_IgnoreAndReturn(MCL_OK);

	// Correlation-ID
	security_generate_random_bytes_ExpectAnyArgsAndReturn(MCL_OK);

    // 3- time_series and custom_data will be added to the http_request by add_tuple : 2 times :
    // Second one will return error telling no more space left in the request
    http_request_add_tuple_IgnoreAndReturn(MCL_OK);
    http_request_add_tuple_IgnoreAndReturn(MCL_HTTP_REQUEST_NO_MORE_SPACE);
    http_request_add_tuple_IgnoreAndReturn(MCL_HTTP_REQUEST_NO_MORE_SPACE);

    // 4- Prepared http_request will be sent using http_client_send : 1 time :
    http_client_send_ExpectAnyArgsAndReturn(MCL_OK);
    http_client_send_ReturnThruPtr_http_response(&success_response);

    // 5- Request and response objects will be destroyed once each :
    http_response_destroy_Ignore();
    http_request_destroy_Ignore();

    // 6- Prepared http_request will be sent using http_client_send : 1 time :
    http_request_initialize_ExpectAnyArgsAndReturn(MCL_OK);
    http_request_initialize_ReturnThruPtr_http_request(&http_request_2);
    http_request_add_tuple_IgnoreAndReturn(MCL_OK);
    http_request_add_header_IgnoreAndReturn(MCL_OK);
    http_request_add_header_IgnoreAndReturn(MCL_OK);
	http_request_add_header_IgnoreAndReturn(MCL_OK);

    http_request_finalize_IgnoreAndReturn(MCL_OK);

	// Correlation-ID
	security_generate_random_bytes_ExpectAnyArgsAndReturn(MCL_OK);

    http_client_send_ExpectAnyArgsAndReturn(MCL_OK);
    http_client_send_ReturnThruPtr_http_response(&fail_response);

    // 7- event set will be destroyed: 1 time :
    event_list_destroy_Ignore();

    result = http_processor_exchange(http_processor, store, NULL);
    TEST_ASSERT_EQUAL_INT_MESSAGE(MCL_UNAUTHORIZED, result, "Exchange operation is failed!");

    // check the item count in the store :
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, store->high_priority_list->count, "There are still remaining high priority data in the store!");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, store->low_priority_list->count, "There are still remaining low priority data in the store!");

    http_processor_destroy(&http_processor);
    mcl_store_destroy(&store);
}

// GIVEN : http_processor initialized - store initialized - 1 custom data added to store - no communication problem, provided event_list is null
// WHEN  : Payload size of the 1 custom data is less than the MAX http payload size.
// THEN  : Exchange operation would successfully add the custom data to the request and send it
void test_exchange_015(void)
{
	E_MCL_ERROR_CODE result = http_processor_initialize(configuration, &http_processor);
	TEST_ASSERT_EQUAL_INT_MESSAGE(MCL_OK, result, "http_processor_initialize failed!");

	http_processor->security_handler = security_handler;

	// create a store
	mcl_store_t *store = MCL_NULL;
	result = mcl_store_initialize(MCL_FALSE, &store);
	TEST_ASSERT_EQUAL_INT_MESSAGE(MCL_OK, result, "mcl_store_initialize failed!");

	// add a custom data to the store :
	mcl_uint8_t payload[] =
	{
		'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'
	};
	mcl_custom_data_t *custom_data = MCL_NULL;
	result = mcl_store_new_custom_data(store, "1.0", "custom_data_type_1", routing, &custom_data);
	TEST_ASSERT_EQUAL_INT_MESSAGE(MCL_OK, result, "New custom data from the store failed!");
	custom_data->payload.buffer = payload;
	custom_data->payload.size = sizeof(payload) / sizeof(payload[0]);

	// define mocks before calling exchange :
	// 1- json_from_item_meta will be called for custom_data->meta : 1 time :
	json_from_item_meta_ExpectAnyArgsAndReturn(MCL_OK);

	string_t *json_meta = new_meta_json_string();
	json_from_item_meta_ReturnThruPtr_json_string(&json_meta);

	// 2- An http_request will be created : 1 time :
	http_request_initialize_ExpectAnyArgsAndReturn(MCL_OK);
	http_request_initialize_ReturnThruPtr_http_request(&http_request);

	// 2.1- Add headers : Content-type, Authorization, Correlation-ID
	http_request_add_header_IgnoreAndReturn(MCL_OK);
	http_request_add_header_IgnoreAndReturn(MCL_OK);
	http_request_add_header_IgnoreAndReturn(MCL_OK);

	http_request_finalize_IgnoreAndReturn(MCL_OK);

	// Correlation-ID
	security_generate_random_bytes_ExpectAnyArgsAndReturn(MCL_OK);

	// 3- Custom data will be added to the http_request by add_tuple : 1 time :
	// ( here it returns MCL_OK, which means adding it successful. http_processor_exchange shouldn't call it again. )
	http_request_add_tuple_IgnoreAndReturn(MCL_OK);

	// 4- Prepared http_request will be sent using http_client_send : 1 time :
	success_response->payload = "";
	http_client_send_ExpectAnyArgsAndReturn(MCL_OK);
	http_client_send_ReturnThruPtr_http_response(&success_response);

	// 5- Request and response objects will be destroyed once each :
	http_response_destroy_Ignore();
	http_request_destroy_Ignore();

	// 6- event set object will be destroyed once each : 1 time.
	event_list_destroy_Ignore();

	// mock preperation is done. Calling the testing function :
	result = http_processor_exchange(http_processor, store,MCL_NULL);
	TEST_ASSERT_EQUAL_INT_MESSAGE(MCL_OK, result, "Exchange operation is failed!");

	// check the item count in the store :
	TEST_ASSERT_EQUAL_INT_MESSAGE(0, store->high_priority_list->count, "There are still remaining high priority data in the store!");
	TEST_ASSERT_EQUAL_INT_MESSAGE(0, store->low_priority_list->count, "There are still remaining low priority data in the store!");

	http_processor_destroy(&http_processor);
	mcl_store_destroy(&store);
}
