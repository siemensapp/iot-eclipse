/*!**********************************************************************
*
* @copyright Copyright (C) 2018 Siemens Aktiengesellschaft.\n
*            All rights reserved.
*
*************************************************************************
*
* @file     onboard_dsc_upload.c
* @date     9 Jan 2018
* @brief    Example custom agent code which onboards and uploads data source configuration.
*
************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "mcl/mcl.h"
#include "callbacks.h"

const char *mindsphere_certificate = "-----BEGIN CERTIFICATE-----\n"\
"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\n"\
"-----END CERTIFICATE-----";

const char *configuration_id_file_name = "configurationId.txt";
const char *data_point_ids_file_name = "dataPointIds.txt";

// Replace with your own initial access token.
char *initial_access_token = "eyJraWQiOiJrZXktaWQtMSIsInR5cCI6IkpXVCIsImFsZyI6IlJTMjU2In0.eyJpc3MiOiJTQ0kiLCJzdWIiOiJjYmM5Mjk3MTM0MWY0ZTc4YTI5ODlmZTY4ODIzOWY3NyIsImF1ZCI6IkFJQU0iLCJpYXQiOjE1MTU0OTk0NTcsIm5iZiI6MTUxNTQ5OTQ1NywiZXhwIjoxNTE2MTA0MjU3LCJqdGkiOiJiOWJmODdiMy0zNjBhLTRkNTctYmIwMC05ZjIxMDZjNjlmMDYiLCJzY29wZSI6IklBVCIsInRlbiI6ImNvbm50ZW5hbnRvbmVxYSIsImNsaWVudF9jcmVkZW50aWFsc19wcm9maWxlIjpbIlNIQVJFRF9TRUNSRVQiXSwic2NoZW1hcyI6WyJ1cm46c2llbWVuczptaW5kc3BoZXJlOnYxIl19.GPbTg9O2-KBI1HpqtgY6VGztI0w9mmwG66zeSnwl4tQHirTonB3xmxFXPxlloPdmrr30vp7c0ZNi2hFfMUvjpz-xvTuVtSXMZV13fr0eo87uMjF0JWCHGaPmZDielmiZKTcLDNF7uwyDzKjON33t5BjKz3X__77gvCL3a7DYzfOUV0ebxGzoMRu2l2gUEpEWAXerUURDPu-0R8vsgLbxsgZzgp0JzcbvSKG5_oIj86TtOSBNXhsZEhArrC5fCfjp2A2IP-imgQ2RMyuGazvn3olpMwoYoFGYDricpXD1cpMEA2jIFXf6iMkTkiOJ_-7YYccUykb1VJZ-nJkE30T2LQ";

void save_data_source_configuration_id(char *configuration_id)
{
    FILE *fd = fopen(configuration_id_file_name, "w");
    fputs(configuration_id, fd);
    fclose(fd);
}

void save_data_point_ids(char *data_point_id_1, char *data_point_id_2)
{
    FILE *fd = fopen(data_point_ids_file_name, "w");
    fputs(data_point_id_1, fd);
    fputs("\n", fd);
    fputs(data_point_id_2, fd);
    fclose(fd);
}

int main(int argc, char *argv[])
{
    printf("***** CUSTOM AGENT using MCL *****\n");

    E_MCL_ERROR_CODE code;
    mcl_configuration_t *configuration = NULL;
    mcl_communication_t *communication = NULL;
    mcl_store_t *store = NULL;
    mcl_data_source_configuration_t *data_source_configuration = NULL;
    mcl_data_source_t *data_source = NULL;

    // Data source configuration id.
    char *configuration_id = NULL;

    // Data source configuration version.
    const char *dsc_version = "1.0";

    // Data point ids.
    char *data_point_id_1 = NULL;
    char *data_point_id_2 = NULL;

    // Initialize log utility.
    mcl_log_util_initialize(E_LOG_OUTPUT_FILE, "log.txt");
    mcl_log_util_set_output_level(MCL_LOG_UTIL_LEVEL_INFO);

    // MCL Configuration.
    printf("Initializing mcl configuration : ");
    code = mcl_configuration_initialize(&configuration);
    printf("%s.\n", mcl_log_util_convert_error_code_to_string(code));

    if (MCL_OK == code)
    {
        configuration->mindsphere_hostname = "https://southgate.eu1.mindsphere.io";
        configuration->mindsphere_port = 443;
        configuration->mindsphere_certificate = mindsphere_certificate;
        configuration->security_profile = MCL_SECURITY_SHARED_SECRET;
        configuration->proxy_hostname =  "http://192.168.0.1";
        configuration->proxy_port = 3128;
        configuration->proxy_type = MCL_PROXY_HTTP;
        configuration->user_agent = "custom agent v1.0";
        configuration->initial_access_token = initial_access_token;
        configuration->tenant = "connttrc";
        configuration->load_function.shared_secret = (mcl_load_shared_secret_callback_t) custom_load_function;
        configuration->save_function.shared_secret = (mcl_save_shared_secret_callback_t) custom_save_function;

        // Initialize mcl communication with the configuration.
        printf("Initializing mcl communication : ");
        code = mcl_communication_initialize(configuration, &communication);
        printf("%s.\n", mcl_log_util_convert_error_code_to_string(code));
    }

    // If mcl communication is successfully initialized, try to onboard.
    if(MCL_OK == code)
    {
        printf("Onboarding : ");
        code = mcl_communication_onboard(communication);
        printf("%s.\n", mcl_log_util_convert_error_code_to_string(code));
    }

    // If onboarding is successful, or the agent is already onboarded, initialize a store.
    if (MCL_OK == code || MCL_ALREADY_ONBOARDED == code)
    {
        printf("Initializing store : ");
        code = mcl_store_initialize(MCL_FALSE, &store);
        printf("%s.\n", mcl_log_util_convert_error_code_to_string(code));
    }

    // Create data source configuration.
    if(MCL_OK == code)
    {
        printf("Creating data source configuration : ");
        code = mcl_store_new_data_source_configuration(store, dsc_version, &data_source_configuration);
        printf("%s.\n", mcl_log_util_convert_error_code_to_string(code));
    }

    // Get configuration id of data_source_configuration.
    if(MCL_OK == code)
    {
        printf("Getting configuration id of data source configuration : ");
        code = mcl_data_source_configuration_get_id(data_source_configuration, &configuration_id);
        printf("%s.\n", mcl_log_util_convert_error_code_to_string(code));
    }

    if (MCL_OK == code)
    {
        char *data_source_name = "dataSourceName";
        char *data_source_description = "dataSourceDescription";

        // Prepare custom data json (optional).
        mcl_json_t *data_source_custom_data = NULL;
        mcl_json_util_initialize(MCL_JSON_OBJECT, &data_source_custom_data);
        mcl_json_util_add_string(data_source_custom_data, "dataSourceCustomFieldA", "dataSourceCustomValueA");
        mcl_json_util_add_string(data_source_custom_data, "dataSourceCustomFieldB", "dataSourceCustomValueB");

        // Add data source to data source configuration.
        printf("Adding data source to data source configuration : ");
        code = mcl_data_source_configuration_add_data_source(data_source_configuration, data_source_name, data_source_description, data_source_custom_data, &data_source);
        printf("%s.\n", mcl_log_util_convert_error_code_to_string(code));

        // Destroy custom data json.
        mcl_json_util_destroy(&data_source_custom_data);
    }

    if(MCL_OK == code)
    {
        // Generate GUID for data point 1.
        mcl_random_generate_guid(&data_point_id_1);

        // Add data point to data source.
        char *data_point_1_name = "Voltage";
        char *data_point_1_type = "DOUBLE";
        char *data_point_1_unit = "Volt";
        char *data_point_1_description = "Voltage Description";

        // Prepare custom data json (optional).
        mcl_json_t *data_point_1_custom_data = NULL;
        mcl_json_util_initialize(MCL_JSON_OBJECT, &data_point_1_custom_data);
        mcl_json_util_add_string(data_point_1_custom_data, "dataPoint1CustomFieldA", "dataPoint1CustomValueA");
        mcl_json_util_add_string(data_point_1_custom_data, "dataPoint1CustomFieldB", "dataPoint1CustomValueB");

        printf("Adding data point 1 to data source : ");
        code = mcl_data_source_configuration_add_data_point(data_source, data_point_id_1, data_point_1_name, data_point_1_description, data_point_1_type, data_point_1_unit, data_point_1_custom_data);
        printf("%s.\n", mcl_log_util_convert_error_code_to_string(code));

        // Destroy custom data json.
        mcl_json_util_destroy(&data_point_1_custom_data);
    }

    if(MCL_OK == code)
    {
        // Generate GUID for data point 2.
        mcl_random_generate_guid(&data_point_id_2);

        // Add data point to data source.
        char *data_point_2_name = "Current";
        char *data_point_2_type = "DOUBLE";
        char *data_point_2_unit = "Ampere";
        char *data_point_2_description = "Current Description";

        // Prepare custom data json (optional).
        mcl_json_t *data_point_2_custom_data = NULL;
        mcl_json_util_initialize(MCL_JSON_OBJECT, &data_point_2_custom_data);
        mcl_json_util_add_string(data_point_2_custom_data, "dataPoint2CustomFieldA", "dataPoint2CustomValueA");
        mcl_json_util_add_string(data_point_2_custom_data, "dataPoint2CustomFieldB", "dataPoint2CustomValueB");

        printf("Adding data point 2 to data source : ");
        mcl_data_source_configuration_add_data_point(data_source, data_point_id_2, data_point_2_name, data_point_2_description, data_point_2_type, data_point_2_unit, data_point_2_custom_data);
        printf("%s.\n", mcl_log_util_convert_error_code_to_string(code));

        // Destroy custom data json.
        mcl_json_util_destroy(&data_point_2_custom_data);
    }

    if(MCL_OK == code)
    {
        // Upload the data source configuration which is in the store.
        printf("Uploading data source configuration : ");
        code = mcl_communication_exchange(communication, store, NULL);
        printf("%s.\n", mcl_log_util_convert_error_code_to_string(code));
    }

    if(MCL_OK == code)
    {
        // Data source configuration is successfully uploaded, save the configuration id and data point ids.
        save_data_source_configuration_id(configuration_id);
        save_data_point_ids(data_point_id_1, data_point_id_2);
    }

    // Clean up.
    free(configuration_id);
    free(data_point_id_1);
    free(data_point_id_2);
    mcl_configuration_destroy(&configuration);
    mcl_communication_destroy(&communication);
    mcl_store_destroy(&store);
    mcl_log_util_finalize();

    if(MCL_OK != code)
    {
        return -1;
    }

    return 0;
}

