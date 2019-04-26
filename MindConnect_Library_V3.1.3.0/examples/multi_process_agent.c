/*!**********************************************************************
*
* @copyright Copyright (C) 2018 Siemens Aktiengesellschaft.\n
*            All rights reserved.
*
*************************************************************************
*
* @file     timeseries_upload.c
* @date     Jan 10, 2018
* @brief    This example code uploads timeseries from an already onboarded agent.
*
* The timeseries contains two data points and values of each data point at two timestamps to be uploaded.
*
* It is assumed that "registration_information_file.txt" exists which is created by the agent after successful onboarding.
* It is also assumed that data source configuration is uploaded which complies with the timeseries you want to exchange.
*
************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "mcl/mcl.h"
#include "callbacks.h"
#include "critical_section.h"

const char *mindsphere_certificate = "-----BEGIN CERTIFICATE-----\n"\
"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\n"\
"-----END CERTIFICATE-----";

const char *registration_information_file = "registrationInformationFile.txt";

int main(int argc, char *argv[])
{
	printf("***** CUSTOM AGENT using MCL *****\n");

	// Initialize log utility.
	mcl_log_util_initialize(E_LOG_OUTPUT_FILE, "log.txt");
	mcl_log_util_set_output_level(MCL_LOG_UTIL_LEVEL_INFO);

	// MCL Configuration. This configuration handle is used to initialize communication handle.
	mcl_configuration_t *configuration = NULL;
	printf("Initializing mcl configuration : ");
	E_MCL_ERROR_CODE code = mcl_configuration_initialize(&configuration);
	printf("%s.\n", mcl_log_util_convert_error_code_to_string(code));

	// The main handle for Agent - MindSphere communication.
	mcl_communication_t *communication = NULL;

	initialize_critical_section();

	if (MCL_OK == code)
	{
		configuration->mindsphere_hostname = "https://southgate.eu1.mindsphere.io";
		configuration->mindsphere_port = 443;
		configuration->mindsphere_certificate = mindsphere_certificate;
		configuration->security_profile = MCL_SECURITY_SHARED_SECRET;
		configuration->proxy_hostname = "http://192.168.0.1";
		configuration->proxy_port = 3128;
		configuration->proxy_type = MCL_PROXY_HTTP;
		configuration->proxy_username = NULL;
		configuration->proxy_password = NULL;
		configuration->proxy_domain = NULL;
		configuration->user_agent = "custom agent v1.0";
		configuration->enter_critical_section = enter_critical_section;
		configuration->leave_critical_section = leave_critical_section;

		// Since this agent is known to be an already onboarded agent we do not need to provide an initial access token.
		configuration->initial_access_token = NULL;
		configuration->tenant = "connttrc";

		// Below callback functions are used to read/write onboarding info.
		// Setting store path to NULL and providing these callback functions is enough.
		configuration->store_path = NULL;
		configuration->load_function.shared_secret = (mcl_load_shared_secret_callback_t)custom_load_function;
		configuration->save_function.shared_secret = (mcl_save_shared_secret_callback_t)custom_save_function;

		// MCL Initialization.
		printf("Initializing mcl communication : ");
		E_MCL_ERROR_CODE code = mcl_communication_initialize(configuration, &communication);
		printf("%s.\n", mcl_log_util_convert_error_code_to_string(code));
	}

	// REMINDER: It is assumed that "registration_information_file.txt" exists which is created by the agent after successful onboarding.
	// REMINDER: It is also assumed that data source configuration is uploaded which complies with the timeseries you want to exchange.

	// Configuration handle is passed to communication handle above. It can be destroyed here.
	mcl_configuration_destroy(&configuration);

	// Store handle is used to put timeseries in it and exchange later.
	mcl_store_t *store = NULL;

	// If mcl communication is successfully initialized, try to initialize store handle.
	if (MCL_OK == code)
	{
		printf("Initializing store : ");
		code = mcl_store_initialize(MCL_FALSE, &store);
		printf("%s.\n", mcl_log_util_convert_error_code_to_string(code));
	}

	// Get the configuration id of data source configuration.
	// This must be the configuration ID generated when uploading the data source configuration.
	char *configuration_id = "e3217e2b-7036-49f2-9814-4c38542cd781";

	// Version of the time series data.
	const char *timeseries_version = "1.0";

	// Time series handle.
	mcl_time_series_t *time_series = NULL;

	// If everything is successful so far, create timeseries in the store.
	if (MCL_OK == code)
	{
		printf("Creating timeseries : ");
		code = mcl_store_new_time_series(store, timeseries_version, configuration_id, NULL, &time_series);
		printf("%s.\n", mcl_log_util_convert_error_code_to_string(code));
	}

	// Time of samples.
	char *timestamp_1 = "2017-12-11T12:00:00.000Z";
	char *timestamp_2 = "2017-12-11T13:00:00.000Z";

	// Values of first data point at two timestamps.
	char *data_point_1_value_1 = "10";
	char *data_point_1_value_2 = "11";

	// Values of second data point at two timestamps.
	char *data_point_2_value_1 = "1";
	char *data_point_2_value_2 = "2";

	// Value set handlers for each timestamp.
	mcl_time_series_value_set_t *value_set_1;
	mcl_time_series_value_set_t *value_set_2;

	// If timeseries is created in the store successfully, then add value_set_1 to time_series.
	if (MCL_OK == code)
	{
		code = mcl_time_series_new_value_set(time_series, timestamp_1, &value_set_1);
	}

	// If value_set_1 is added to timeseries, then add value_set_2 to timeseries.
	if (MCL_OK == code)
	{
		code = mcl_time_series_new_value_set(time_series, timestamp_2, &value_set_2);
	}

	// Ids of the data points the value is read from. This id is used to map the provided value to an appropriate asset, aspect and variable.
	// Datapoint id is received from the data source configuration upload which is performed previously.
	char *data_point_id_1 = "cc3933bb-7330-4a91-a814-62dfa7387e1c";
	char *data_point_id_2 = "1ddeb3bc-a21a-42bc-bc34-8655fe1f3535";

	// Quality of the value provided. Possible values are defined by OPC UA Quality codes. "00000000" means good.
	char *quality_code = "00000000";

	// If value sets are added to time_series successfully, add first value to value_set_1.
	if (MCL_OK == code)
	{
		code = mcl_time_series_add_value(value_set_1, data_point_id_1, data_point_1_value_1, quality_code);
	}

	// If the first value is added to value_set_1, add second value to the same value_set_1.
	if (MCL_OK == code)
	{
		code = mcl_time_series_add_value(value_set_1, data_point_id_2, data_point_2_value_1, quality_code);
	}

	// If the two values are added to value_set_1, add first value to the value_set_2.
	if (MCL_OK == code)
	{
		code = mcl_time_series_add_value(value_set_2, data_point_id_1, data_point_1_value_2, quality_code);
	}

	// If the first value is added to value_set_2, add second value to the same value_set_2.
	if (MCL_OK == code)
	{
		code = mcl_time_series_add_value(value_set_2, data_point_id_2, data_point_2_value_2, quality_code);
	}

	// If everything is successful so far, exchange the store which contains timeseries.
	if (MCL_OK == code)
	{
		printf("Uploading timeseries : ");
		code = mcl_communication_exchange(communication, store, NULL);
		printf("%s.\n", mcl_log_util_convert_error_code_to_string(code));
	}

	// If unauthorized error received, call rotate key to generate a new authentication key.
	if (MCL_OK != code)
	{
		code = mcl_communication_update_security_information(communication);
		
		if(MCL_OK != code)
		{
			code = mcl_communication_get_access_token(communication);		
		}	
		
		if(MCL_OK != code)
		{
			printf("Key rotation is being done : ");
			code = mcl_communication_rotate_key(communication);
			printf("%s.\n", mcl_log_util_convert_error_code_to_string(code));	
		}
		
		if (MCL_OK == code)
		{
			printf("Uploading timeseries : ");
			code = mcl_communication_exchange(communication, store, NULL);
			printf("%s.\n", mcl_log_util_convert_error_code_to_string(code));
		}
	}

	mcl_log_util_finalize();
	mcl_store_destroy(&store);
	mcl_communication_destroy(&communication);

	if (MCL_OK != code)
	{
		return -1;
	}

	return 0;
}
