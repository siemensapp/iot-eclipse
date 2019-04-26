/*!**********************************************************************
*
* @copyright Copyright (C) 2018 Siemens Aktiengesellschaft.\n
*            All rights reserved.
*
*************************************************************************
*
* @file     event.c
* @date     11 May 2018
* @brief    Example custom agent code which uploads event.
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

// Replace with your own initial access token.
char *initial_access_token = "eyJraWQiOiJrZXktaWQtMSIsInR5cCI6IkpXVCIsImFsZyI6IlJTMjU2In0.eyJpc3MiOiJTQ0kiLCJzdWIiOiJlOTBlOWFmZjE2MTY0NGU1OGE3YzcyMTY0NGY3ODEyYyIsImF1ZCI6IkFJQU0iLCJpYXQiOjE1MjU3NjUzMDUsIm5iZiI6MTUyNTc2NTMwNSwiZXhwIjoxNTI2MzcwMTA1LCJqdGkiOiI4MzNiODEyMi00MzFmLTRkYTUtYjhkZi05ZWRlNjM3Y2VjMjQiLCJzY29wZSI6IklBVCIsInRlbiI6Im1jbGliaW90IiwiY2xpZW50X2NyZWRlbnRpYWxzX3Byb2ZpbGUiOlsiU0hBUkVEX1NFQ1JFVCJdLCJzY2hlbWFzIjpbInVybjpzaWVtZW5zOm1pbmRzcGhlcmU6djEiXX0.ld1kdtLfnSeQuNX5GpSBA47FEzvVBfpKD9lc5DkXqjt7SbNtxI7yr61DDERdIwvvKKxKOM2lONHLfGS6JnfhRycWo2yLf8Imsi55Ylo8YyljtDyPZwExnuatfw6qcDGVzoJaqBfleUfjaPa-_9UVk8ZjCBc88sJvT7JJF6AJDc8XGMxwAGO4qMH9MoQoXHBDV513XWt-eJ6hAE5Ph-a7AphOLjuPywQvJcAWR9ryjPlwokmR-I877zNOv34orAKKij042N_AYZCjk20GEwER-W3wCJAzE4Du_tg8h2TY44sjXPcdROM6NlRgF4U8je7Gm7XEVkQNqEsgdxVmJv0IQg";

int main(int argc, char *argv[])
{
    printf("***** CUSTOM AGENT using MCL *****\n");

    E_MCL_ERROR_CODE code;
    mcl_configuration_t *configuration = NULL;
    mcl_communication_t *communication = NULL;
    mcl_store_t *store = NULL;
    mcl_event_t *event = NULL;
    mcl_json_t *event_details = NULL;

    // Initialize log utility.
    mcl_log_util_initialize(E_LOG_OUTPUT_FILE, "log.txt");
    mcl_log_util_set_output_level(MCL_LOG_UTIL_LEVEL_INFO);

    // MCL Configuration.
    printf("Initializing mcl configuration: ");
    code = mcl_configuration_initialize(&configuration);
    printf("%s.\n", mcl_log_util_convert_error_code_to_string(code));

    if (MCL_OK == code)
    {
        configuration->mindsphere_hostname = "https://southgate.eu1.mindsphere.io";
        configuration->tenant = "mclibiot";
        configuration->mindsphere_port = 443;
        configuration->mindsphere_certificate = mindsphere_certificate;
        configuration->security_profile = MCL_SECURITY_SHARED_SECRET;
        configuration->proxy_hostname = "http://192.168.0.1";
        configuration->proxy_port = 3128;
        configuration->proxy_type = MCL_PROXY_HTTP;
        configuration->user_agent = "custom agent v1.0";
        configuration->initial_access_token = initial_access_token;
        configuration->load_function.shared_secret = (mcl_load_shared_secret_callback_t)custom_load_function;
        configuration->save_function.shared_secret = (mcl_save_shared_secret_callback_t)custom_save_function;

        // Initialize mcl communication with the configuration.
        printf("Initializing mcl communication: ");
        code = mcl_communication_initialize(configuration, &communication);
        printf("%s.\n", mcl_log_util_convert_error_code_to_string(code));
    }

    // If mcl communication is successfully initialized, try to onboard.
    if (MCL_OK == code)
    {
        printf("Onboarding: ");
        code = mcl_communication_onboard(communication);
        printf("%s.\n", mcl_log_util_convert_error_code_to_string(code));
    }

    // If onboarding is successful, or the agent is already onboarded, initialize a store.
    if (MCL_OK == code || MCL_ALREADY_ONBOARDED == code)
    {
        printf("Initializing store: ");
        code = mcl_store_initialize(MCL_FALSE, &store);
        printf("%s.\n", mcl_log_util_convert_error_code_to_string(code));
    }

    // If store initialization is successful, create a new event.
    if (MCL_OK == code)
    {
        printf("Creating new event: ");
        code = mcl_store_new_event(store, "1.0", "MindSphereStandardEvent", "2.0", MCL_EVENT_SEVERITY_INFORMATION, "2018-05-11T12:00:00.000Z", &event);
        printf("%s.\n", mcl_log_util_convert_error_code_to_string(code));
    }

    // If creating event is successful, set optional field correlation id.
    if (MCL_OK == code)
    {
        printf("Setting correlation id: ");
        code = mcl_event_set_option(event, MCL_EVENT_OPTION_CORRELATION_ID, "6ba7b810-9dad-11d1-80b4-00c04fd430c8");
        printf("%s.\n", mcl_log_util_convert_error_code_to_string(code));
    }

    // If setting correlation id is successful, set optional field description.
    if (MCL_OK == code)
    {
        printf("Setting description: ");
        code = mcl_event_set_option(event, MCL_EVENT_OPTION_DESCRIPTION, "temperature is high");
        printf("%s.\n", mcl_log_util_convert_error_code_to_string(code));
    }

    // If setting description is successful, create json for event details.
    if (MCL_OK == code)
    {
        printf("Creating event details: ");
        code = mcl_json_util_initialize(MCL_JSON_OBJECT, &event_details);
        printf("%s.\n", mcl_log_util_convert_error_code_to_string(code));
    }

    // If event details is created, add string to event details.
    if (MCL_OK == code)
    {
        printf("Adding string to event details: ");
        code = mcl_json_util_add_string(event_details, "source", "motor 1");
        printf("%s.\n", mcl_log_util_convert_error_code_to_string(code));
    }

    // If adding string to event details is successful, set optional field details.
    if (MCL_OK == code)
    {
        printf("Setting details: ");
        code = mcl_event_set_option(event, MCL_EVENT_OPTION_DETAILS, event_details);
        printf("%s.\n", mcl_log_util_convert_error_code_to_string(code));
    }

    // If setting event details is successful, exchange the store.
    if (MCL_OK == code)
    {
        printf("Exchange: ");
        code = mcl_communication_exchange(communication, store, NULL);
        printf("%s.\n", mcl_log_util_convert_error_code_to_string(code));
    }

    mcl_json_util_destroy(&event_details);
    mcl_store_destroy(&store);
    mcl_communication_destroy(&communication);

    if (MCL_OK == code)
    {
        return 0;
    }

    return -1;
}
