/*!**********************************************************************
 *
 * @copyright Copyright (C) 2016 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 *
 *************************************************************************
 *
 * @file     test_json.c
 * @date     Jul 19, 2016
 * @brief    This file implements all test cases of json module.
 *
 ************************************************************************/

#include "custom_data.h"
#include "file.h"
#include "time_series.h"
#include "json.h"
#include "data_types.h"
#include "event_list.h"
#include "event.h"
#include "data_source_configuration.h"
#include "unity.h"
#include "memory.h"
#include "log_util.h"
#include "list.h"
#include "string_type.h"
#include "string_util.h"
#include "mock_random.h"
#include "definitions.h"
#include "mcl/mcl_time_series.h"
#include "mcl/mcl_custom_data.h"
#include "json_util.h"
#include "mcl/mcl_json_util.h"
#include "event_list.h"
#include "time_util.h"
#include "mcl/mcl_event.h"
#include "mcl_data_source_configuration.h"

char *timestamp = "2016-04-26T08:06:25.317Z";
char *timestamp_2 = "2016-04-26T08:06:25.317Z";
char *data_point_id = "e50ab7ca-fd5d-11e5-8000-001b1bc14a1d";
char *value = "65";
char *quality_code = "00000000";
char *data_point_id_2 = "e50ab7ca-fd5d-11e5-8000-001b1bc14a1d";
char *value_2 = "65";
char *quality_code_2 = "00000000";
char *configuration_id = "e3217e2b-7036-49f2-9814-4c38542cd781";
char *payload_version = "1.0";
char *routing = "vnd.kuka.FingerprintAnalizer";
string_t *json_string = MCL_NULL;
mcl_size_t duration = 42;
char *description = "Test Rack in ERL M with MindConnectNano";

// Event
const char *correlation_id = "6ba7b810-9dad-11d1-80b4-00c04fd430c8";
const char *event_payload_type = "thresholdViolation";
const char *event_payload_version = "1.0";
const char *details_name = "message";
const char *details_value = "Critical oscillation level";
const char *details_name_2 = "message";
const char *details_value_2 = "Critical oscillation level";
char *event_list_json = "[{\"meta\":{\"type\":\"item\",\"version\":\"1.0\",\"payload\":{\"type\":\"businessEvent\",\"version\":\"1.0\"}},\"payload\":{\"id\":\"12\",\"correlationId\":\"6ba7b810-9dad-11d1-80b4-00c04fd430c8\",\"sourceType\":\"application\",\"sourceId\":\"FirmwareUpgradeService\",\"timestamp\":\"2016-04-26T08:06:25.317Z\",\"severity\":3,\"description\":\"nice event\",\"status\":\"good status\",\"type\":\"fw upgrade\",\"version\":\"1.0\",\"details\":{\"download_link\":\"https://agentcom.apps.mindsphere.io/ api/v1/swrepository/FWUpdates/SAL01.00.00.02b004/download\",\"name\":\"Linux Agent\",\"version\":\"SAL01.00.00.02b004\",\"description\":\"Fix for the following security issues...\"}}},{\"meta\":{\"type\":\"item\",\"version\":\"1.0\",\"payload\":{\"type\":\"businessEvent\",\"version\":\"1.0\"}},\"payload\":{\"id\":\"153e4567-e89b-12d3-a456-426655440000\",\"correlationId\":\"6ba7b810-9dad-11d1-80b4-00c04fd430c8\",\"sourceType\":\"asset\",\"sourceId\":\"30849f78-10a4-11e6-a148-3e1d05defe78\",\"timestamp\":\"2016-04-26T08:06:25.317Z\",\"severity\":1,\"description\":\"nice event\",\"status\": \"good status\",\"type\":\"thresholdViolation\",\"version\":\"1.2\",\"details\":{\"message\":\"Critical oscillation level\",\"errorCode\":\"54745\",\"details\":\"Resonance vibrations reached a dangerous level\",\"value\":\"50HZ\"}}}]";

/*
 * Note: example_json_string is in the format shown below.
 *
 * {
 *      "type": "item",
 *      "version": "1.0",
 *      "payload": {
 *          "type": "standardTimeSeries",
 *          "version": "1.0",
 *          "details": {
 *              "configurationId": "e3217e2b-7036-49f2-9814-4c38542cd781"
 *          }
 *      }
 * }
 */
char *example_json_string =
    "{\"type\":\"item\",\"version\":\"1.0\",\"payload\":{\"type\":\"standardTimeSeries\",\"version\":\"v2\",\"details\":{\"configurationId\":\"e3217e2b-7036-49f2-9814-4c38542cd781\"}}}";

void setUp(void)
{
}

void tearDown(void)
{
    string_destroy(&json_string);
}

/**
 * GIVEN : Initialized time_series.
 * WHEN  : json_from_item_meta is called with time_series whose mandatory fields are set.
 * THEN  : It returns the json string for the meta part of time series.
 */
void test_json_from_item_meta_001(void)
{
    time_series_t *time_series;
    time_series_initialize(payload_version, configuration_id, MCL_NULL, &time_series);
    TEST_ASSERT_NOT_NULL_RETURN(time_series);

    MCL_DEBUG("Call json_from_item_meta function");
    json_from_item_meta(&(time_series->meta), &json_string);

    char *expected_json_string =
        "{\"type\":\"item\",\"version\":\"1.0\",\"payload\":{\"type\":\"standardTimeSeries\",\"version\":\"1.0\",\"details\":{\"configurationId\":\"e3217e2b-7036-49f2-9814-4c38542cd781\"}}}";
    TEST_ASSERT_EQUAL_STRING_MESSAGE(expected_json_string, json_string->buffer, "json string fail");
    time_series_destroy(&time_series);
}

/**
 * GIVEN : Initialized file with mandatory and optional fields.
 * WHEN  : json_from_item_meta is called with filled file struct.
 * THEN  : It returns full meta part of file json string.
 */
void test_json_from_item_meta_002(void)
{
    // TODO: After completing mcl_file.c, update below with initialize function of mcl_file.c
    file_t *file;
    MCL_NEW_WITH_ZERO(file);
    TEST_ASSERT_NOT_NULL_RETURN(file);

    E_MCL_ERROR_CODE code = string_initialize_static(meta_field_values[META_FIELD_TYPE_ITEM].buffer, meta_field_values[META_FIELD_TYPE_ITEM].length, &file->meta.type);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "String initialize fail for meta_type.");

    code = string_initialize_static(meta_field_values[META_FIELD_VERSION_CURRENT].buffer, meta_field_values[META_FIELD_VERSION_CURRENT].length, &file->meta.version);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "String initialize fail for meta_version.");

    // meta_details
    code = string_initialize_new("vnd.kuka.FingerprintAnalizer", 0, &file->meta.details.routing);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "String initialize fail for meta.details.routing.");

    // meta_payload
    code = string_initialize_static(meta_field_values[META_FIELD_PAYLOAD_TYPE_FILE].buffer, meta_field_values[META_FIELD_PAYLOAD_TYPE_FILE].length, &file->meta.payload.type);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "String initialize fail for meta.payload.type.");

    code = string_initialize_static(meta_field_values[META_FIELD_PAYLOAD_VERSION_CURRENT].buffer, meta_field_values[META_FIELD_PAYLOAD_VERSION_CURRENT].length,
                              &file->meta.payload.version);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "String initialize fail for meta.payload.version.");

    // meta_payload_details
    code = string_initialize_new("data_collector.log.old", 0, &file->meta.payload.details.file_details.file_name);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "String initialize fail for file_name.");

    code = string_initialize_new("2017-12-13T11:04:37.000Z", 0, &file->meta.payload.details.file_details.creation_date);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "String initialize fail for creation_date.");

    code = string_initialize_new("log", 0, &file->meta.payload.details.file_details.file_type);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "String initialize fail for file_type.");

    json_from_item_meta(&(file->meta), &json_string);

    char *expected_json_string =
        "{\"type\":\"item\",\"version\":\"1.0\",\"details\":{\"routing\":\"vnd.kuka.FingerprintAnalizer\"},\"payload\":{\"type\":\"file\",\"version\":\"1.0\",\"details\":{\"fileName\":\"data_collector.log.old\",\"creationDate\":\"2017-12-13T11:04:37.000Z\",\"fileType\":\"log\"}}}";
    TEST_ASSERT_EQUAL_STRING_MESSAGE(expected_json_string, json_string->buffer, "json string fail");

    // TODO: After completing mcl_file.c, update below with just mcl_file_destroy();
    string_destroy(&file->meta.type);
    string_destroy(&file->meta.version);
    string_destroy(&file->meta.details.routing);
    string_destroy(&file->meta.payload.type);
    string_destroy(&file->meta.payload.version);
    string_destroy(&file->meta.payload.details.file_details.file_name);
    string_destroy(&file->meta.payload.details.file_details.creation_date);
    string_destroy(&file->meta.payload.details.file_details.file_type);
    MCL_FREE(file);
}

/**
 * GIVEN : Initialized custom_data.
 * WHEN  : json_from_item_meta is called with filled custom-type struct without optional routing parameter.
 * THEN  : It returns full meta part of custom-type json string.
 */
void test_json_from_item_meta_003(void)
{
    custom_data_t *custom_data;
    char *type = "helloType";

    E_MCL_ERROR_CODE code = custom_data_initialize(payload_version, type, MCL_NULL, &custom_data);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Initialization failed for custom_data.");

    // Create details json object of custom data.
    mcl_json_t *details = MCL_NULL;
    mcl_json_util_initialize(MCL_JSON_OBJECT, &details);

    // Add string to details json object.
    mcl_json_util_add_string(details, "name_1", "value_1");
    mcl_json_util_add_string(details, "name_2", "value_2");

    // Set details of custom data.
    mcl_custom_data_set_meta_details(custom_data, details);

    MCL_DEBUG("call function tested.");

    custom_data_t *custom_data_local = (custom_data_t *)custom_data;
    json_from_item_meta(&(custom_data_local->meta), &json_string);

    char *expected_json_string =
        "{\"type\":\"item\",\"version\":\"1.0\",\"payload\":{\"type\":\"helloType\",\"version\":\"1.0\",\"details\":{\"name_1\":\"value_1\",\"name_2\":\"value_2\"}}}";
    TEST_ASSERT_EQUAL_STRING_MESSAGE(expected_json_string, json_string->buffer, "json string fail");

    custom_data_destroy(&custom_data);
    json_util_destroy(&details);
}

/**
 * GIVEN : File struct is filled with only mandatory fields.
 * WHEN  : json_from_item_meta is called with filled file struct.
 * THEN  : It returns the json string for meta part of file.
 */
void test_json_from_item_meta_004(void)
{
    // TODO: After completing mcl_file.c, update below with initialize function of mcl_file.c
    file_t *file;
    MCL_NEW_WITH_ZERO(file);
    TEST_ASSERT_NOT_NULL_RETURN(file);

    E_MCL_ERROR_CODE code = string_initialize_static(meta_field_values[META_FIELD_TYPE_ITEM].buffer, meta_field_values[META_FIELD_TYPE_ITEM].length, &file->meta.type);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "String initialize fail for meta_type.");

    code = string_initialize_static(meta_field_values[META_FIELD_VERSION_CURRENT].buffer, meta_field_values[META_FIELD_VERSION_CURRENT].length, &file->meta.version);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "String initialize fail for meta_version.");

    // meta_payload
    code = string_initialize_static(meta_field_values[META_FIELD_PAYLOAD_TYPE_FILE].buffer, meta_field_values[META_FIELD_PAYLOAD_TYPE_FILE].length, &file->meta.payload.type);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "String initialize fail for meta.payload.type.");

    code = string_initialize_static(meta_field_values[META_FIELD_PAYLOAD_VERSION_CURRENT].buffer, meta_field_values[META_FIELD_PAYLOAD_VERSION_CURRENT].length,
                                  &file->meta.payload.version);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "String initialize fail for meta.payload.version.");

    // meta_payload_details
    code = string_initialize_new("data_collector.log.old", 0, &file->meta.payload.details.file_details.file_name);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "String initialize fail for file_name.");

    code = string_initialize_new("2017-12-13T11:04:37.000Z", 0, &file->meta.payload.details.file_details.creation_date);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "String initialize fail for creation_date.");

    json_from_item_meta(&(file->meta), &json_string);

    char *expected_json_string =
        "{\"type\":\"item\",\"version\":\"1.0\",\"payload\":{\"type\":\"file\",\"version\":\"1.0\",\"details\":{\"fileName\":\"data_collector.log.old\",\"creationDate\":\"2017-12-13T11:04:37.000Z\"}}}";
    TEST_ASSERT_EQUAL_STRING_MESSAGE(expected_json_string, json_string->buffer, "json string fail");

    // TODO: After completing mcl_file.c, update below with just mcl_file_destroy();
    string_destroy(&file->meta.type);
    string_destroy(&file->meta.version);
    string_destroy(&file->meta.details.routing);
    string_destroy(&file->meta.payload.type);
    string_destroy(&file->meta.payload.version);
    string_destroy(&file->meta.payload.details.file_details.file_name);
    string_destroy(&file->meta.payload.details.file_details.creation_date);
    MCL_FREE(file);
}

/**
 * GIVEN :@p time_to_live_hint is equal to #MCL_SIZE_MAX.
 * WHEN  :json_from_item_meta is created with max @p time_to_live_hint value.
 * THEN  :Generated json string consists of "timeToLive":4294967295.
 */
void test_json_from_item_meta_005(void)
{
    // TODO: After completing mcl_file.c, update below with initialize function of mcl_file.c
    file_t *file;
    MCL_NEW_WITH_ZERO(file);
    TEST_ASSERT_NOT_NULL_RETURN(file);

    E_MCL_ERROR_CODE code = string_initialize_static(meta_field_values[META_FIELD_TYPE_ITEM].buffer, meta_field_values[META_FIELD_TYPE_ITEM].length, &file->meta.type);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "String initialize fail for meta_type.");

    code = string_initialize_static(meta_field_values[META_FIELD_VERSION_CURRENT].buffer, meta_field_values[META_FIELD_VERSION_CURRENT].length, &file->meta.version);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "String initialize fail for meta_version.");

    // meta_details
    code = string_initialize_new("vnd.kuka.LogParser", 0, &file->meta.details.routing);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "String initialize fail for meta.details.routing.");

    // meta_payload
    code = string_initialize_static(meta_field_values[META_FIELD_PAYLOAD_TYPE_FILE].buffer, meta_field_values[META_FIELD_PAYLOAD_TYPE_FILE].length, &file->meta.payload.type);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "String initialize fail for meta.payload.type.");

    code = string_initialize_static(meta_field_values[META_FIELD_PAYLOAD_VERSION_CURRENT].buffer, meta_field_values[META_FIELD_PAYLOAD_VERSION_CURRENT].length,
                                  &file->meta.payload.version);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "String initialize fail for meta.payload.version.");

    // meta_payload_details
    code = string_initialize_new("data_collector.log.old", 0, &file->meta.payload.details.file_details.file_name);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "String initialize fail for file_name.");

    code = string_initialize_new("2017-12-13T11:04:37.000Z", 0, &file->meta.payload.details.file_details.creation_date);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "String initialize fail for creation_date.");

    json_from_item_meta(&(file->meta), &json_string);

    string_t *expected_json_string = MCL_NULL;

#if SIZE_MAX == UINT32_MAX
    code = string_initialize_static("{\"type\":\"item\",\"version\":\"1.0\",\"details\":{\"routing\":\"vnd.kuka.LogParser\"},\"payload\":{\"type\":\"file\",\"version\":\"1.0\",\"details\":{\"fileName\":\"data_collector.log.old\",\"creationDate\":\"2017-12-13T11:04:37.000Z\"}}}", 0, &expected_json_string);
        TEST_ASSERT_MESSAGE(MCL_OK == code, "String initialize fail for expected_json_string in 32 bit-platform.");
#else
    code = string_initialize_static("{\"type\":\"item\",\"version\":\"1.0\",\"details\":{\"routing\":\"vnd.kuka.LogParser\"},\"payload\":{\"type\":\"file\",\"version\":\"1.0\",\"details\":{\"fileName\":\"data_collector.log.old\",\"creationDate\":\"2017-12-13T11:04:37.000Z\"}}}", 0, &expected_json_string);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "String initialize fail for expected_json_string in 64 bit-platform.");
#endif

    TEST_ASSERT_NOT_EQUAL(expected_json_string, MCL_NULL);
    TEST_ASSERT_EQUAL_STRING_MESSAGE(expected_json_string->buffer, json_string->buffer, "json string fail");

    // TODO: After completing mcl_file.c, update below with just mcl_file_destroy();
    string_destroy(&expected_json_string);
    string_destroy(&file->meta.type);
    string_destroy(&file->meta.version);
    string_destroy(&file->meta.details.routing);
    string_destroy(&file->meta.payload.type);
    string_destroy(&file->meta.payload.version);
    string_destroy(&file->meta.payload.details.file_details.file_name);
    string_destroy(&file->meta.payload.details.file_details.creation_date);
    MCL_FREE(file);
}

/**
 * GIVEN : Initialized custom_data.
 * WHEN  : json_from_item_meta is called with filled custom-type struct with optional routing parameter.
 * THEN  : It returns full meta part of custom-type json string.
 */
void test_json_from_item_meta_006(void)
{
    custom_data_t *custom_data;
    char *type = "helloCustomType";

    E_MCL_ERROR_CODE code = custom_data_initialize(payload_version, type, routing, &custom_data);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Initialization failed for custom_data.");

    // Create details json object of custom data.
    mcl_json_t *details = MCL_NULL;
    mcl_json_util_initialize(MCL_JSON_OBJECT, &details);

    // Add string to details json object.
    mcl_json_util_add_string(details, "name_1", "value_1");
    mcl_json_util_add_string(details, "name_2", "value_2");

    // Set details of custom data.
    mcl_custom_data_set_meta_details(custom_data, details);

    MCL_DEBUG("call function tested.");

    custom_data_t *custom_data_local = (custom_data_t *)custom_data;
    json_from_item_meta(&(custom_data_local->meta), &json_string);

    char *expected_json_string =
        "{\"type\":\"item\",\"version\":\"1.0\",\"details\":{\"routing\":\"vnd.kuka.FingerprintAnalizer\"},\"payload\":{\"type\":\"helloCustomType\",\"version\":\"1.0\",\"details\":{\"name_1\":\"value_1\",\"name_2\":\"value_2\"}}}";
    TEST_ASSERT_EQUAL_STRING_MESSAGE(expected_json_string, json_string->buffer, "json string fail");

    custom_data_destroy(&custom_data);
    json_util_destroy(&details);
}

/**
}

/**
 * GIVEN : Initialized time_series.
 * WHEN  : json_from_time_series_payload is called with filled mcl_time_series_payload struct.
 * THEN  : It returns the json string for the payload part of time series.
 */
void test_json_from_time_series_payload_001(void)
{
    time_series_t *time_series;
    time_series_initialize(payload_version, configuration_id, routing, &time_series);

    mcl_time_series_value_set_t *value_set_1;
    mcl_time_series_new_value_set(time_series, timestamp, &value_set_1);
    mcl_time_series_add_value(value_set_1, data_point_id, value, quality_code);
    mcl_time_series_add_value(value_set_1, data_point_id_2, value_2, quality_code_2);

    mcl_time_series_value_set_t *value_set_2;
    mcl_time_series_new_value_set(time_series, timestamp, &value_set_2);
    mcl_time_series_add_value(value_set_2, data_point_id, value, quality_code);
    mcl_time_series_add_value(value_set_2, data_point_id_2, value_2, quality_code_2);

    json_from_time_series_payload(&time_series->payload, &json_string);

    char *expected_json_string =
        "[{\"timestamp\":\"2016-04-26T08:06:25.317Z\",\"values\":[{\"dataPointId\":\"e50ab7ca-fd5d-11e5-8000-001b1bc14a1d\",\"value\":\"65\",\"qualityCode\":\"00000000\"},{\"dataPointId\":\"e50ab7ca-fd5d-11e5-8000-001b1bc14a1d\",\"value\":\"65\",\"qualityCode\":\"00000000\"}]},{\"timestamp\":\"2016-04-26T08:06:25.317Z\",\"values\":[{\"dataPointId\":\"e50ab7ca-fd5d-11e5-8000-001b1bc14a1d\",\"value\":\"65\",\"qualityCode\":\"00000000\"},{\"dataPointId\":\"e50ab7ca-fd5d-11e5-8000-001b1bc14a1d\",\"value\":\"65\",\"qualityCode\":\"00000000\"}]}]";

    TEST_ASSERT_EQUAL_STRING_MESSAGE(expected_json_string, json_string->buffer, "json string fail.");

    time_series_destroy(&time_series);
}

/**
 * GIVEN : Initialized payload of data source configuration without optional fields.
 * WHEN  : json_from_data_source_configuration_payload is called.
 * THEN  : It returns the json string for the payload part of data source configuration.
 */
void test_json_from_data_source_configuration_payload_001(void)
{
    // Mock configuration id.
    string_t *id;
    string_initialize_new("xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx", 0, &id);
    random_generate_guid_ExpectAnyArgsAndReturn(MCL_OK);
    random_generate_guid_ReturnThruPtr_guid(&id);

    // Initialize data_source_configuration.
    data_source_configuration_t *data_source_configuration = MCL_NULL;
    E_MCL_ERROR_CODE code = data_source_configuration_initialize(payload_version, &data_source_configuration);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Initialization failed for data source configuration.");

    // Add data source.
    mcl_data_source_t *data_source = MCL_NULL;
    char *data_source_name = "Main motor";
    code = mcl_data_source_configuration_add_data_source(data_source_configuration, data_source_name, MCL_NULL, MCL_NULL, &data_source);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "mcl_data_source_configuration_add_data_source() failed.");

    // Add data point.
    char *data_point_id = "e50ab7ca-fd5d-11e5-8000-001b1bc14a1d";
    char *data_point_name = "Temperature";
    char *data_point_type = "uint";
    char *data_point_unit = "C";
    code = mcl_data_source_configuration_add_data_point(data_source, data_point_id, data_point_name, MCL_NULL, data_point_type, data_point_unit,
                                                        MCL_NULL);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "mcl_data_source_configuration_add_data_point() failed.");

    code = json_from_data_source_configuration_payload(&data_source_configuration->payload, &json_string);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "mcl_data_source_configuration_add_data_source() failed.");

    char *expected_json_string = "{\"configurationId\":\"xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx\",\"dataSources\":[{\"name\":\"Main motor\",\"dataPoints\":[{\"id\":\"e50ab7ca-fd5d-11e5-8000-001b1bc14a1d\",\"name\":\"Temperature\",\"type\":\"uint\",\"unit\":\"C\"}]}]}";
    TEST_ASSERT_EQUAL_STRING_MESSAGE(expected_json_string, json_string->buffer, "json string fail.");

    data_source_configuration_destroy(&data_source_configuration);
}

/**
 * GIVEN : Initialized payload of data source configuration with optional fields.
 * WHEN  : json_from_data_source_configuration_payload is called.
 * THEN  : It returns the json string for the payload part of data source configuration.
 */
void test_json_from_data_source_configuration_payload_002(void)
{
    // Mock configuration id.
    string_t *id;
    string_initialize_new("xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx", 0, &id);
    random_generate_guid_ExpectAnyArgsAndReturn(MCL_OK);
    random_generate_guid_ReturnThruPtr_guid(&id);

    // Initialize data_source_configuration.
    data_source_configuration_t *data_source_configuration = MCL_NULL;
    E_MCL_ERROR_CODE code = data_source_configuration_initialize(payload_version, &data_source_configuration);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Initialization failed for data source configuration.");

    // Create custom data of data source in json format.
    json_t *data_source_custom_data = MCL_NULL;
    code = json_util_initialize(JSON_OBJECT, &data_source_custom_data);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "json_util_initialize() failed.");

    // Add strings to data source custom data.
    code = json_util_add_string(data_source_custom_data, "additionalProp1", "string");
    TEST_ASSERT_MESSAGE(MCL_OK == code, "json_util_add_string() failed.");

    // Add data source.
    mcl_data_source_t *data_source = MCL_NULL;
    char *data_source_name = "Main motor";
    char *data_source_description = "Motor performance";
    code = mcl_data_source_configuration_add_data_source(data_source_configuration, data_source_name, data_source_description, data_source_custom_data, &data_source);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "mcl_data_source_configuration_add_data_source() failed.");

    // Create custom data of data point in json format.
    json_t *data_point_custom_data = MCL_NULL;
    code = json_util_initialize(JSON_OBJECT, &data_point_custom_data);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "json_util_initialize() failed.");

    // Add strings to data source custom data.
    code = json_util_add_string(data_point_custom_data, "additionalProp2", "string");
    TEST_ASSERT_MESSAGE(MCL_OK == code, "json_util_add_string() failed.");

    // Add data point.
    char *data_point_id = "e50ab7ca-fd5d-11e5-8000-001b1bc14a1d";
    char *data_point_name = "Temperature";
    char *data_point_description = "Climate change";
    char *data_point_type = "uint";
    char *data_point_unit = "C";
    code = mcl_data_source_configuration_add_data_point(data_source, data_point_id, data_point_name, data_point_description, data_point_type, data_point_unit,
                                                        data_point_custom_data);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "mcl_data_source_configuration_add_data_point() failed.");

    // Add the same data source again.
    code = mcl_data_source_configuration_add_data_source(data_source_configuration, data_source_name, data_source_description, data_source_custom_data, &data_source);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Second mcl_data_source_configuration_add_data_source() failed.");

    // Add the same data point again.
    code = mcl_data_source_configuration_add_data_point(data_source, data_point_id, data_point_name, data_point_description, data_point_type, data_point_unit,
                                                        data_point_custom_data);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Second mcl_data_source_configuration_add_data_point() failed.");

    code = json_from_data_source_configuration_payload(&data_source_configuration->payload, &json_string);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "mcl_data_source_configuration_add_data_source() failed.");

    char *expected_json_string = "{\"configurationId\":\"xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx\",\"dataSources\":[{\"name\":\"Main motor\",\"description\":\"Motor performance\",\"dataPoints\":[{\"id\":\"e50ab7ca-fd5d-11e5-8000-001b1bc14a1d\","
        "\"name\":\"Temperature\",\"description\":\"Climate change\",\"type\":\"uint\",\"unit\":\"C\",\"customData\":{\"additionalProp2\":\"string\"}}],\"customData\":{\"additionalProp1\":\"string\"}},{\"name\":\"Main motor\",\"description\":\"Motor performance\","
        "\"dataPoints\":[{\"id\":\"e50ab7ca-fd5d-11e5-8000-001b1bc14a1d\",\"name\":\"Temperature\",\"description\":\"Climate change\",\"type\":\"uint\",\"unit\":\"C\",\"customData\":{\"additionalProp2\":\"string\"}}],\"customData\":{\"additionalProp1\":\"string\"}}]}";
    TEST_ASSERT_EQUAL_STRING_MESSAGE(expected_json_string, json_string->buffer, "json string fail.");

    json_util_destroy(&data_source_custom_data);
    json_util_destroy(&data_point_custom_data);
    data_source_configuration_destroy(&data_source_configuration);
}

/**
 * GIVEN : Initialized event.
 * WHEN  : json_from_event_payload is called with all fields filled in event to add event to event set.
 * THEN  : It returns the json string for the payload part of event list with all optional and mandatory fields.
 */
void test_json_from_event_payload_001(void)
{
    string_t *id;
    string_initialize_new("xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx", 0, &id);
    random_generate_guid_ExpectAnyArgsAndReturn(MCL_OK);
    random_generate_guid_ReturnThruPtr_guid(&id);

    event_list_t *event_list = MCL_NULL;
    event_list_initialize(payload_version, &event_list);

    // Initialize event.
    event_t *event = MCL_NULL;
    event_initialize(event_list->meta, event_payload_type, event_payload_version, 1, timestamp, &event);

    // Set optional correlation id.
    E_MCL_ERROR_CODE code = mcl_event_set_option(event, MCL_EVENT_OPTION_CORRELATION_ID, correlation_id);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Correlation id could not be set.");

    // Set optional description.
    code = mcl_event_set_option(event, MCL_EVENT_OPTION_DESCRIPTION, description);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Description could not be set.");

    // Create details json object.
    mcl_json_t *details = MCL_NULL;
    code = mcl_json_util_initialize(MCL_JSON_OBJECT, &details);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for json.");

    code = mcl_json_util_add_string(details, details_name, details_value);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "String could not be added to the json object.");

    // Set optional details.
    code = mcl_event_set_option(event, MCL_EVENT_OPTION_DETAILS, details);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Details could not be set.");

    // Add event to event list.
    event_list_add_event(event, event_list);

    json_from_event_payload(event_list->events, &json_string);

    char *expected_json_string =
        "[{\"id\":\"xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx\",\"correlationId\":\"6ba7b810-9dad-11d1-80b4-00c04fd430c8\",\"timestamp\":\"2016-04-26T08:06:25.317Z\",\"severity\":1,\"description\":\"Test Rack in ERL M with MindConnectNano\",\"type\":\"thresholdViolation\",\"version\":\"1.0\",\"details\":{\"message\":\"Critical oscillation level\"}}]";
    TEST_ASSERT_EQUAL_STRING_MESSAGE(expected_json_string, json_string->buffer, "json string fail.");

    json_util_destroy(&details);
    event_list_destroy(&event_list);
}

/**
 * GIVEN : Initialized two events.
 * WHEN  : json_from_event_payload is called without optional fields of event to add events to event list.
 * THEN  : It returns the json string for the payload part of event list without optional fields.
 */
void test_json_from_event_payload_002(void)
{
    string_t *id;
    string_initialize_new("xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx", 0, &id);
    random_generate_guid_ExpectAnyArgsAndReturn(MCL_OK);
    random_generate_guid_ReturnThruPtr_guid(&id);

    event_list_t *event_list = MCL_NULL;
    event_list_initialize(payload_version, &event_list);

    // Initialize event.
    event_t *event = MCL_NULL;
    event_initialize(event_list->meta, event_payload_type, event_payload_version, 1, timestamp, &event);

    // For event_2 initialization.
    string_t *id_2;
    string_initialize_new("xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxy", 0, &id_2);
    random_generate_guid_ExpectAnyArgsAndReturn(MCL_OK);
    random_generate_guid_ReturnThruPtr_guid(&id_2);

    // For event_2 initialization.
    // Initialize event.
    event_t *event_2 = MCL_NULL;
    event_initialize(event_list->meta, event_payload_type, event_payload_version, 2, timestamp, &event_2);

    // Add event to event set.
    event_list_add_event(event, event_list);

    // Add event_2 to event set.
    event_list_add_event(event_2, event_list);

    json_from_event_payload(event_list->events, &json_string);

    char *expected_json_string =
        "[{\"id\":\"xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx\",\"timestamp\":\"2016-04-26T08:06:25.317Z\",\"severity\":1,\"type\":\"thresholdViolation\",\"version\":\"1.0\",\"details\":{}},"
        "{\"id\":\"xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxy\",\"timestamp\":\"2016-04-26T08:06:25.317Z\",\"severity\":2,\"type\":\"thresholdViolation\",\"version\":\"1.0\",\"details\":{}}]";
    TEST_ASSERT_EQUAL_STRING_MESSAGE(expected_json_string, json_string->buffer, "json string fail.");

    event_list_destroy(&event_list);
}
