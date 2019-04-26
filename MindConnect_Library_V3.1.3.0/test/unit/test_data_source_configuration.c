/*!**********************************************************************
*
* @copyright Copyright (C) 2017 Siemens Aktiengesellschaft.\n
*            All rights reserved.
*
*************************************************************************
*
* @file     test_data_source_configuration.c
* @date     Nov 16, 2017
* @brief    This file implements all test cases of mcl_data_source_configuration module.
*
************************************************************************/

#include "mcl/mcl_data_source_configuration.h"
#include "data_source_configuration.h"
#include "log_util.h"
#include "data_types.h"
#include "mcl/mcl_common.h"
#include "memory.h"
#include "unity.h"
#include "definitions.h"
#include "string_type.h"
#include "string_util.h"
#include "mock_random.h"
#include "list.h"
#include "json_util.h"

data_source_configuration_t *data_source_configuration = MCL_NULL;

// To use in item meta part.
string_t *guid = MCL_NULL;
char *payload_version = "1.0";
char *meta_version = "1.0";
char *meta_type = "item";
char *payload_type = "dataSourceConfiguration";
char *configuration_id = "xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx";
E_MCL_ERROR_CODE code = MCL_FALSE;

// To use in data source part.
char *data_source_name = "Main motor";
char *data_source_description = "Motor performance";

// To use in data point part.
char *data_point_id = "e50ab7ca-fd5d-11e5-8000-001b1bc14a1d";
char *data_point_name = "Temperature";
char *data_point_description = "Climate change";
char *data_point_type = "uint";
char *data_point_unit = "°C";

void setUp(void)
{
    // Mock configuration_id of payload which is guid.
    string_initialize_new(configuration_id, 0, &guid);

    random_generate_guid_ExpectAnyArgsAndReturn(MCL_OK);
    random_generate_guid_ReturnThruPtr_guid(&guid);
}

void tearDown(void)
{
}

/**
 * GIVEN : data_source_configuration to initialize.
 * WHEN  : data_source_configuration_initialize() is called with valid parameters.
 * THEN  : data_source_configuration is initialized.
 */
void test_initialize_001(void)
{
    code = data_source_configuration_initialize(payload_version, &data_source_configuration);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Initialization failed for data source configuration.");

    // Test meta.type
    TEST_ASSERT_EQUAL_STRING_MESSAGE(meta_type, data_source_configuration->meta.type->buffer, "meta_type fail");

    // Test meta.version
    TEST_ASSERT_EQUAL_STRING_MESSAGE(meta_version, data_source_configuration->meta.version->buffer, "meta_version fail");

    // Test meta.payload.type
    TEST_ASSERT_EQUAL_STRING_MESSAGE(payload_type, data_source_configuration->meta.payload.type->buffer, "meta_payload_type fail");

    // Test meta.payload.version
    TEST_ASSERT_EQUAL_STRING_MESSAGE(payload_version, data_source_configuration->meta.payload.version->buffer, "meta_payload_version fail");

    // Test data_source_configuration->payload.configuration_id
    TEST_ASSERT_EQUAL_STRING_MESSAGE(configuration_id, data_source_configuration->payload.configuration_id->buffer, "meta_payload_details_configuration_id fail");

    data_source_configuration_destroy(&data_source_configuration);
}

/**
 * GIVEN : Initialized data_source_configuration.
 * WHEN  : mcl_data_source_configuration_add_data_source() is called without optional arguments.
 * THEN  : data source is added to data_source_configuration without optional arguments.
 */
void test_add_data_source_001(void)
{
    // Initialize data_source_configuration.
    data_source_configuration_initialize(payload_version, &data_source_configuration);

    mcl_data_source_t *data_source = MCL_NULL;
    code = mcl_data_source_configuration_add_data_source(data_source_configuration, data_source_name, MCL_NULL, MCL_NULL, &data_source);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "mcl_data_source_configuration_add_data_source() failed.");

    // Get data source from the data source configuration.
    mcl_data_source_t *data_source_of_data_source_configuration = ((mcl_data_source_t *)data_source_configuration->payload.data_sources->head->data);

    // Check data source name if it is added to data_source_configuration successfully.
    TEST_ASSERT_EQUAL_STRING_MESSAGE(data_source_name, data_source_of_data_source_configuration->name->buffer, "data source name fail.");

    data_source_configuration_destroy(&data_source_configuration);
}

/**
 * GIVEN : Initialized data_source_configuration.
 * WHEN  : mcl_data_source_configuration_add_data_source() is called with optional arguments.
 * THEN  : data source is added to data_source_configuration with optional arguments.
 */
void test_add_data_source_002(void)
{
    // Initialize data_source_configuration.
    data_source_configuration_initialize(payload_version, &data_source_configuration);

    // Create custom data in json format.
    json_t *custom_data = MCL_NULL;
    json_util_initialize(JSON_OBJECT, &custom_data);

    json_util_add_string(custom_data, "motorHeat", "low");
    json_util_add_string(custom_data, "motorEfficiency", "high");

    mcl_data_source_t *data_source = MCL_NULL;
    code = mcl_data_source_configuration_add_data_source(data_source_configuration, data_source_name, data_source_description, custom_data, &data_source);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "mcl_data_source_configuration_add_data_source() failed.");

    // Get data source from the data source configuration.
    mcl_data_source_t *data_source_of_data_source_configuration = ((mcl_data_source_t *)data_source_configuration->payload.data_sources->head->data);

    // Check data source name if it is added to data_source_configuration successfully.
    TEST_ASSERT_EQUAL_STRING_MESSAGE(data_source_name, data_source_of_data_source_configuration->name->buffer, "data source name fail.");

    // Check data source description if it is added to data_source_configuration successfully.
    TEST_ASSERT_EQUAL_STRING_MESSAGE(data_source_description, data_source_of_data_source_configuration->description->buffer, "data source description fail.");

    json_t *custom_data_of_data_source_configuration = data_source->custom_data;

    // Convert custom_data json object to string.
    char *custom_data_string = MCL_NULL;
    json_util_to_string(custom_data_of_data_source_configuration, &custom_data_string);

    char *expected_custom_data_string = "{\"motorHeat\":\"low\",\"motorEfficiency\":\"high\"}";

    TEST_ASSERT_EQUAL_STRING_MESSAGE(expected_custom_data_string, custom_data_string, "custom_data_string fail.");

    json_util_destroy(&custom_data);
    MCL_FREE(custom_data_string);
    data_source_configuration_destroy(&data_source_configuration);
}

/**
 * GIVEN : Initialized data source.
 * WHEN  : mcl_data_source_configuration_add_data_point() is called without optional arguments.
 * THEN  : data point is added to data source without optional arguments.
 */
void test_add_data_point_001(void)
{
    // Initialize data_source_configuration.
    data_source_configuration_initialize(payload_version, &data_source_configuration);

    mcl_data_source_t *data_source = MCL_NULL;
    mcl_data_source_configuration_add_data_source(data_source_configuration, data_source_name, MCL_NULL, MCL_NULL, &data_source);

    code = mcl_data_source_configuration_add_data_point(data_source, data_point_id, data_point_name, MCL_NULL, data_point_type, data_point_unit, MCL_NULL);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "mcl_data_source_configuration_add_data_point() failed.");

    // Get data source from the data source configuration.
    mcl_data_source_t *data_source_of_data_source_configuration = (mcl_data_source_t *)data_source_configuration->payload.data_sources->head->data;

    // Get data point from the data source.
    data_point_t *data_point_of_data_source = (data_point_t *)data_source_of_data_source_configuration->data_points->head->data;

    // Check data point id if it is added to data_source successfully.
    TEST_ASSERT_EQUAL_STRING_MESSAGE(data_point_id, data_point_of_data_source->id->buffer, "data point id fail.");

    // Check data point name if it is added to data_source successfully.
    TEST_ASSERT_EQUAL_STRING_MESSAGE(data_point_name, data_point_of_data_source->name->buffer, "data point name fail.");

    // Check data point type if it is added to data_source successfully.
    TEST_ASSERT_EQUAL_STRING_MESSAGE(data_point_type, data_point_of_data_source->type->buffer, "data point type fail.");

    // Check data point unit if it is added to data_source successfully.
    TEST_ASSERT_EQUAL_STRING_MESSAGE(data_point_unit, data_point_of_data_source->unit->buffer, "data point unit fail.");

    data_source_configuration_destroy(&data_source_configuration);
}

/**
 * GIVEN : Initialized data source.
 * WHEN  : mcl_data_source_configuration_add_data_point() is called with optional arguments.
 * THEN  : data point is added to data source with optional arguments.
 */
void test_add_data_point_002(void)
{
    // Initialize data_source_configuration.
    data_source_configuration_initialize(payload_version, &data_source_configuration);

    mcl_data_source_t *data_source = MCL_NULL;
    mcl_data_source_configuration_add_data_source(data_source_configuration, data_source_name, MCL_NULL, MCL_NULL, &data_source);

    // Create custom data in json format.
    json_t *custom_data = MCL_NULL;
    json_util_initialize(JSON_OBJECT, &custom_data);

    json_util_add_string(custom_data, "temperatureAccuracy", "low");
    json_util_add_string(custom_data, "temperatureValue", "medium");

    code = mcl_data_source_configuration_add_data_point(data_source, data_point_id, data_point_name, data_point_description, data_point_type, data_point_unit, custom_data);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "mcl_data_source_configuration_add_data_point() failed.");

    // Get data source from the data source configuration.
    mcl_data_source_t *data_source_of_data_source_configuration = (mcl_data_source_t *)data_source_configuration->payload.data_sources->head->data;

    // Get data point from the data source.
    data_point_t *data_point_of_data_source = (data_point_t *)data_source_of_data_source_configuration->data_points->head->data;

    // Check data point id if it is added to data_source successfully.
    TEST_ASSERT_EQUAL_STRING_MESSAGE(data_point_id, data_point_of_data_source->id->buffer, "data point id fail.");

    // Check data point name if it is added to data_source successfully.
    TEST_ASSERT_EQUAL_STRING_MESSAGE(data_point_name, data_point_of_data_source->name->buffer, "data point name fail.");

    // Check data point description if it is added to data_source successfully.
    TEST_ASSERT_EQUAL_STRING_MESSAGE(data_point_description, data_point_of_data_source->description->buffer, "data point description fail.");

    // Check data point type if it is added to data_source successfully.
    TEST_ASSERT_EQUAL_STRING_MESSAGE(data_point_type, data_point_of_data_source->type->buffer, "data point type fail.");

    // Check data point unit if it is added to data_source successfully.
    TEST_ASSERT_EQUAL_STRING_MESSAGE(data_point_unit, data_point_of_data_source->unit->buffer, "data point unit fail.");

    json_t *custom_data_of_data_point = data_point_of_data_source->custom_data;

    // Convert custom_data json object to string.
    char *custom_data_string = MCL_NULL;
    json_util_to_string(custom_data_of_data_point, &custom_data_string);

    char *expected_custom_data_string = "{\"temperatureAccuracy\":\"low\",\"temperatureValue\":\"medium\"}";

    TEST_ASSERT_EQUAL_STRING_MESSAGE(expected_custom_data_string, custom_data_string, "custom_data_string fail.");

    json_util_destroy(&custom_data);
    MCL_FREE(custom_data_string);
    data_source_configuration_destroy(&data_source_configuration);
}

/**
 * GIVEN : Initialized data source.
 * WHEN  : mcl_data_source_configuration_get_id() is called.
 * THEN  : Id of data source configuration is received.
 */
void test_get_id_001(void)
{
    // Initialize data_source_configuration.
    data_source_configuration_initialize(payload_version, &data_source_configuration);

    char *configuration_id_local = MCL_NULL;
    code = mcl_data_source_configuration_get_id(data_source_configuration, &configuration_id_local);

    TEST_ASSERT_MESSAGE(MCL_OK == code, "mcl_data_source_configuration_get_id() failed.");

    // We mocked random_generate_guid() in the setup stage to generate this guid "xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx".
    TEST_ASSERT_EQUAL_STRING_MESSAGE(configuration_id, data_source_configuration->payload.configuration_id->buffer, "mcl_data_source_configuration_get_id() fails.");

    MCL_FREE(configuration_id_local);
    data_source_configuration_destroy(&data_source_configuration);
}

/**
 * GIVEN : Initialized data_source_configuration.
 * WHEN  : User requests to destroy data_source_configuration.
 * THEN  : User expects the data_source_configuration and everything in it have been freed and pointer is NULL
 */
void test_destroy_001(void)
{
    data_source_configuration_initialize(payload_version, &data_source_configuration);
    TEST_ASSERT_NOT_NULL_RETURN(data_source_configuration);

    data_source_configuration_destroy(&data_source_configuration);
    TEST_ASSERT_NULL_MESSAGE(data_source_configuration, "After destroy operation, data_source_configuration is not NULL!");
}
