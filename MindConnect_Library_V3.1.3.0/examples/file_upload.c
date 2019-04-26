#include <stdio.h>
#include "mcl/mcl.h"
// callbacks.h contains custom load/save functions for onboarding info.
#include "callbacks.h"

const char *mindsphere_certificate = "-----BEGIN CERTIFICATE-----\n"\
"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\n"\
"-----END CERTIFICATE-----";

const char *registration_information_file = "registrationInformationFile.txt";
char *initial_access_token = "eyJraWQiOiJrZXktaWQtMSIsInR5cCI6IkpXVCIsImFsZyI6IlJTMjU2In0.eyJpc3MiOiJTQ0kiLCJzdWIiOiI0ODhhOTQ0NDE2NzY0YzFjYWExMjVmNGMxMzMxYmQ0YiIsImF1ZCI6IkFJQU0iLCJpYXQiOjE1MTU5OTkzNjMsIm5iZiI6MTUxNTk5OTM2MywiZXhwIjoxNTE2NjA0MTYzLCJqdGkiOiJhMThhZjNmMS03OWRjLTQwMWYtOTkyZC1jNzcxZjk4ZTk0MGIiLCJzY29wZSI6IklBVCIsInRlbiI6ImNvbm50dHJjIiwiY2xpZW50X2NyZWRlbnRpYWxzX3Byb2ZpbGUiOlsiU0hBUkVEX1NFQ1JFVCJdLCJzY2hlbWFzIjpbInVybjpzaWVtZW5zOm1pbmRzcGhlcmU6djEiXX0.j1nZgrskiFrJXRgPZbrEzK50tnm2YOq4AXDoxT_UkqiXT6Io0N17664EzbGV_gehuBdXNnXJEumLmdKtouC_t1uZsbFQP2ESYujKv7SsVrePlJ1y9NDmLSz4o3U08mR-aZA_pXonz8_cCZzC3g6_gBll16NUQHIV1pq_VrbFXr7h_4WxvE4TMGncby0ZXpOegxDgmrAYLUaxbCbgYwDFeXV73gNW-NGJiapo8dKNPnjLxNI99_dA6EnXbaNMJoIJOpwKBwEtl14sY8LsEx2xSfS24CnwZla45IXDaokLCB1IiSiuSPevHd1hsUDHxMf9xUtT9S6DXKumCFkI5WWHzA";

int main()
{
	// Initialize log util and set it's output level.
	printf("Initializing log util");
	E_MCL_ERROR_CODE code = mcl_log_util_initialize(E_LOG_OUTPUT_STDERR);
	printf("%s.\n", mcl_log_util_convert_error_code_to_string(code));

	if (MCL_OK == code)
	{
		printf("Setting log output level: ");
		mcl_log_util_set_output_level(MCL_LOG_UTIL_LEVEL_INFO);
		printf("%s.\n", mcl_log_util_convert_error_code_to_string(code));
	}

	// Initialize an MCL configuration
	mcl_configuration_t *configuration = NULL;
	printf("Initializing configuration handle: ");
	code = mcl_configuration_initialize(&configuration);
    printf("%s.\n", mcl_log_util_convert_error_code_to_string(code));

    mcl_communication_t *communication = NULL;

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
        configuration->initial_access_token = initial_access_token;
        configuration->tenant = "connttrc";
        configuration->max_http_payload_size = 1024 * 1024 * 10;

        // Below callback functions are used to read/write onboarding info.
        // If callbacks are provided store path is ignored.
        configuration->store_path = NULL;
        configuration->load_function.shared_secret = (mcl_load_shared_secret_callback_t)custom_load_function;
        configuration->save_function.shared_secret = (mcl_save_shared_secret_callback_t)custom_save_function;

        // Initialize an MCL Communication Handle with the given configuration.
        printf("Initializing communication handle: ");
        code = mcl_communication_initialize(configuration, &communication);
        printf("%s.\n", mcl_log_util_convert_error_code_to_string(code));
    }

    // Configuration handle is passed to communication handle above. It can be destroyed here.
    mcl_configuration_destroy(&configuration);
	
	// Onboard before doing any further operation.
	if (MCL_OK == code)
	{
		printf("Onboarding: ");
		code = mcl_communication_onboard(communication);
		printf("%s.\n", mcl_log_util_convert_error_code_to_string(code));
	}

	// To upload a file file name, file path and file type information are necessary.
	const char *file_path = "log_file.txt";
	const char *file_name = "error_log.txt";
	const char *file_type = "log";

	FILE *file_to_upload = fopen(file_path, "w");
	if (NULL != file_to_upload && (MCL_OK == code || MCL_ALREADY_ONBOARDED == code))
	{
		fputs("This file is for file upload example.", file_to_upload);
		fclose(file_to_upload);
	}
	else
	{
		code = MCL_FILE_CANNOT_BE_OPENED;
	}
	
	// For any exchange operation a store must be created.
	mcl_store_t *store = NULL;
	if (MCL_OK == code || MCL_ALREADY_ONBOARDED == code)
	{
		printf("Initializing store: ");
		code = mcl_store_initialize(MCL_FALSE, &store);
		printf("%s.\n", mcl_log_util_convert_error_code_to_string(code));
	}

	// In order to exchange file it must be added to store.
	mcl_file_t *file = NULL;

	if (MCL_OK == code)
	{
		printf("Adding file to store: ");
		code = mcl_store_new_file(store, "1.0", file_path, file_name, file_type, "vnd.kuka.FingerprintAnalizer", &file);
		printf("%s.\n", mcl_log_util_convert_error_code_to_string(code));
	}

	// Exchange store along with file (and other data if previously added to store).
	if (MCL_OK == code)
	{
		printf("Uploading file: ");
		code = mcl_communication_exchange(communication, store, NULL);
		printf("%s.\n", mcl_log_util_convert_error_code_to_string(code));

		// If exchange returns unauthorized, then get new access token.
		if (MCL_UNAUTHORIZED == code)
		{
	        printf("Getting new access token: ");
		    code = mcl_communication_get_access_token(communication);
	        printf("%s.\n", mcl_log_util_convert_error_code_to_string(code));

	        // If getting new access token is successful, then try to exchange again.
	        if (MCL_OK == code)
	        {
	            printf("Retrying exchange: ");
	            code = mcl_communication_exchange(communication, store, NULL);
	            printf("%s.\n", mcl_log_util_convert_error_code_to_string(code));
	        }
	        else if (MCL_BAD_REQUEST == code)
	        {
	            // If get access token returns bad request error then rotate key and try exchanging again.
	            printf("Calling rotate key: ");
	            code = mcl_communication_rotate_key(communication);
	            printf("%s.\n", mcl_log_util_convert_error_code_to_string(code));

	            // If key rotation is successful, then retry exchange.
	            if (MCL_OK == code)
	            {
	                printf("Retrying exchange: ");
	                code = mcl_communication_exchange(communication, store, NULL);
	                printf("%s.\n", mcl_log_util_convert_error_code_to_string(code));
	            }
	        }
		}
	}

	// At the end all resources should be freed (e.g store, communication etc.).
	mcl_store_destroy(&store);
	mcl_communication_destroy(&communication);
	mcl_log_util_finalize();

	if (MCL_OK != code)
	{
		return -1;
	}

	return 0;
}
