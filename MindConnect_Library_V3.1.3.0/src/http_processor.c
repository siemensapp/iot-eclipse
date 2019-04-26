/*!**********************************************************************
 *
 * @copyright Copyright (C) 2016 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 *
 *************************************************************************
 *
 * @file     http_processor.c
 * @date     Jul 19, 2016
 * @brief    HTTP processor module implementation file.
 *
 ************************************************************************/

#include "http_processor.h"
#include "log_util.h"
#include "memory.h"
#include "http_definitions.h"
#include "json.h"
#include "json_util.h"
#include "list.h"
#include "data_types.h"
#include "time_series.h"
#include "event_list.h"
#include "file.h"
#include "custom_data.h"
#include "stream_data.h"
#include "data_source_configuration.h"
#include "definitions.h"
#include "jwt.h"
#include "storage.h"
#include "security.h"
#include "http_request.h"
#include "random.h"
#include "file_util.h"
#include "mcl/mcl_common.h"
#include "time_util.h"

#define SERVER_NONCE "server_nonce"
#define SERVER_PROOF "server_proof"

#define JSON_NAME_CLIENT_ID "client_id"
#define JSON_NAME_TOKEN_ENDPOINT_AUTH_METHOD "token_endpoint_auth_method"
#define JSON_NAME_GRANT_TYPES "grant_types"
#define JSON_NAME_CLIENT_SECRET "client_secret"
#define JSON_NAME_CLIENT_SECRET_EXPIRES_AT "client_secret_expires_at"
#define JSON_NAME_REGISTRATION_ACCESS_TOKEN "registration_access_token"
#define JSON_NAME_REGISTRATION_CLIENT_URI "registration_client_uri"
#define JSON_NAME_JWKS "jwks"
#define JSON_NAME_KEYS "keys"
#define JSON_NAME_E "e"
#define JSON_NAME_N "n"
#define JSON_NAME_KTY "kty"
#define JSON_NAME_KID "kid"
#define JSON_NAME_ACCESS_TOKEN "access_token"

#define REGISTER_URI_PATH "/register"
#define ACCESS_TOKEN_URI_PATH "/token"

// TODO : Max count is TBD :
#define MAX_STREAM_CALLBACK_CALL_COUNT 10

#define MAX_RANGE_HEADER_LENGTH (100)
#define MAX_TOP_EVENT_PARAMETER_LENGTH (50)
#define EVENT_REQUEST_LIMIT (100) //!< According to specification

#define CORRELATION_ID_BYTE_LENGTH 16

/**
 * @brief Enumeration for the Mindsphere endpoints.
 */
typedef enum E_ENDPOINT_TYPE
{
    ENDPOINT_ONBOARDING,   //!< Onboarding endpoint.
    ENDPOINT_KEY_ROTATION, //!< Key rotation endpoint.
    ENDPOINT_EXCHANGE      //!< Exchange endpoint.
} E_ENDPOINT_TYPE;

/**
 * @brief Enumeration for URIs for different endpoints.
 */
typedef enum E_ENDPOINT_URI
{
    ENDPOINT_URI_EXCHANGE,      //!< Uri for exchange endpoint.
	ENDPOINT_URI_DOWNLOAD,      //!< Uri for download endpoint.
	ENDPOINT_URI_EVENT,         //!< Uri for event endpoint.
	ENDPOINT_URI_ACK_EVENT,     //!< Uri for ack event endpoint.
	ENDPOINT_URI_ACCESS_TOKEN,  //!< Uri for access token endpoint.
	ENDPOINT_URI_REGISTER,		//!< Uri for registration endpoint.
	ENDPOINT_URI_END            //!< End of uri endpoint.
} E_ENDPOINT_URI;

// This is the array for endpoints.
string_t endpoint_uri[ENDPOINT_URI_END] =
{
	STRING_CONSTANT("api/mindconnect/v3/exchange"),
	STRING_CONSTANT("api/mindconnect/v3/files"),
	STRING_CONSTANT("api/mindconnect/v3/events"),
	STRING_CONSTANT("api/mindconnect/v3/events/acknowledge"),
	STRING_CONSTANT("api/agentmanagement/v3/oauth/token"),
	STRING_CONSTANT("api/agentmanagement/v3/register")
};

// This function processes the HTTP response to an onboarding or key rotation request for shared secret security profile.
static E_MCL_ERROR_CODE _process_registration_response_shared_secret(http_processor_t *http_processor, http_response_t *http_response);

// This function processes the HTTP response to an onboarding or key rotation request for shared RSA 3072 security profile.
static E_MCL_ERROR_CODE _process_registration_response_rsa_3072(http_processor_t *http_processor, http_response_t *http_response);

// This function concatenates host and endpoint to form URI.
static E_MCL_ERROR_CODE _concatenate_host_and_endpoint(string_t *host, string_t *endpoint, string_t **uri);

#if MCL_FILE_DOWNLOAD_ENABLED
// This function extracts file name from Content Disposition header
static E_MCL_ERROR_CODE _get_file_name(char *header, string_t **file_name);

// Adds range header to HTTP request
static E_MCL_ERROR_CODE _add_range_header(http_request_t *request, mcl_size_t start_byte, mcl_size_t end_byte);

// Adds actual range values to file
static E_MCL_ERROR_CODE _add_actual_range_to_file(http_response_t *response, file_t *file);
#endif

// This function fills an http request with the provided store as much as it can.
static E_MCL_ERROR_CODE _exchange_fill_http_request(http_processor_t *http_processor, store_t *store, http_request_t *request);

// Used for preperation of store data. This means the meta and payload strings preperation from their respective structed objects and setting the state of the data to PREPARED:
static E_MCL_ERROR_CODE _exchange_prepare_data(store_data_t *store_data);

// This function adds the provided store data to the request. Gets called by _exchange_fill_http_request:
static E_MCL_ERROR_CODE _exchange_add_current_data_to_request(store_data_t *current_store_data, http_request_t *request);

// This function adds the provided store data to the request using stream mechanism. Gets called by _exchange_fill_http_request:
static E_MCL_ERROR_CODE _exchange_add_current_data_to_request_by_streaming(store_data_t *current_store_data, http_request_t *request);

// This is http response evaluation function. Also updates the store data states based on response status.
static E_MCL_ERROR_CODE _exchange_evaluate_response(store_t *store, E_MCL_ERROR_CODE send_result, http_response_t *response, void **reserved, string_t *correlation_id);

// This function updates the state's of the data in the store based on the send operation result. If the send operation is failed data needs to be written again so its write counters reset.
static E_MCL_ERROR_CODE _exchange_update_store_state(store_t *store, mcl_bool_t send_operation_successful);

// This function is for updateing one single store data based on the send operation result. Called by _exchange_update_store_state:
static E_MCL_ERROR_CODE _exchange_update_store_data_state(store_data_t *store_data, mcl_bool_t send_operation_successful);

// This is for clearing the already sent data from the store :
static E_MCL_ERROR_CODE _exchange_clear_sent_data_from_store(store_t *store);

// This is used for getting the content info such as content id and types for meta and payload of the current store data :
static E_MCL_ERROR_CODE _exchange_store_data_get_content_info(store_data_t *store_data, string_t **meta_content_type, string_t **meta_content_id,
        string_t **payload_content_type);

// This function performs the initial operations of an http_request object, like adding the common headers :
static E_MCL_ERROR_CODE _exchange_initialize_http_request_headers(http_processor_t *http_processor, http_request_t *request, mcl_bool_t with_authentication);

// This function performs the final operations of an http_reqeust object, like adding the jwt header and closing the boundaries:
static E_MCL_ERROR_CODE _exchange_finalize_http_request(http_processor_t *http_processor, http_request_t *request, mcl_bool_t with_authentication);

// generates the jwt authentication header and adds to the request if with_authentication is MCL_TRUE. Else, does nothing.
static E_MCL_ERROR_CODE _add_authentication_header_to_request(http_processor_t *http_processor, http_request_t *request, mcl_bool_t with_authentication);

// This is the callback function given as an argument to http_request_add_tuple function to get the payload from buffer.
// Returns the number of actual written count. user_context is not currently used.
static mcl_size_t _get_payload_from_buffer(void *destination, void *source, mcl_size_t size, void *user_context);

// This is the callback function given as an argument to http_request_add_tuple function to get the payload from file.
// Returns the number of actual written count. user_context is not currently used.
static mcl_size_t _get_payload_from_file(void *destination, void *file_descriptor, mcl_size_t size, void *user_context);

#if MCL_STREAM_ENABLED
// This is the http client read callback for stream operation. This function fills the provided buffer with the http reqeust payload data generated from the store.
mcl_size_t _stream_callback(void *buffer, mcl_size_t size, mcl_size_t count, void *user_context);
#endif

// Composes JSON string for onboarding with RSA security profile.
static E_MCL_ERROR_CODE _compose_rsa_onboarding_json(security_handler_t * security_handler, string_t **payload);

// Composes JSON string for key rotation with RSA security profile.
static E_MCL_ERROR_CODE _compose_rsa_key_rotation_json(security_handler_t * security_handler, string_t **payload);

// Adds key json object to keys json array.
static E_MCL_ERROR_CODE _add_key_to_keys_array(json_t *root, json_t **json_object);

// Checks if the path provided is accessible.
static E_MCL_ERROR_CODE _check_file_path(const char *file_name);

// Use custom function for loading register info.
static E_MCL_ERROR_CODE _custom_load_register_info(http_processor_t *http_processor);
static E_MCL_ERROR_CODE _evaluate_response_codes(http_response_t *response);
static E_MCL_ERROR_CODE _generate_correlation_id_string(string_t **correlation_id);

// Saves registration information.
E_MCL_ERROR_CODE _save_registration_information(http_processor_t *http_processor);

// Compose the payload for the request to be sent to /token endpoint.
E_MCL_ERROR_CODE _compose_access_token_request_payload(http_processor_t *http_processor, string_t **request_payload);

E_MCL_ERROR_CODE http_processor_initialize(configuration_t *configuration, http_processor_t **http_processor)
{
    DEBUG_ENTRY("configuration_t *configuration = <%p>, http_processor_t **http_processor = <%p>", configuration, http_processor)
	
	// Return code for the functions to be called.
	E_MCL_ERROR_CODE return_code;

    // initialize json module
    json_util_initialize_json_library();

    // initialize security module
    security_initialize();

    // Create http processor handle.
    MCL_NEW(*http_processor);
    ASSERT_CODE_MESSAGE(MCL_NULL != *http_processor, MCL_OUT_OF_MEMORY, "Memory can not be allocated for http processor.");

    // Make sure pointer members of http_processor which will be created inside this function are null initially.
    // This is necessary for an unexpected call to http_processor_destroy() function.
    (*http_processor)->http_client = MCL_NULL;
    (*http_processor)->security_handler = MCL_NULL;

    // Set pointer to configuration parameters.
    (*http_processor)->configuration = configuration;

    // Initialize http client.
    return_code = http_client_initialize(configuration, &((*http_processor)->http_client));
    ASSERT_STATEMENT_CODE_MESSAGE(MCL_OK == return_code, http_processor_destroy(http_processor), return_code, "Http client initialization failed.");
    MCL_DEBUG("Http client is successfully initialized.");

    // Initialize security handler.
    return_code = security_handler_initialize(&((*http_processor)->security_handler));
    ASSERT_STATEMENT_CODE_MESSAGE(MCL_OK == return_code, http_processor_destroy(http_processor), return_code, "Security handler initialization failed.");
    MCL_DEBUG("Security handler is successfully initialized.");

    // Load registration information via custom functions if both function pointers are not null
    // or via file system if the path is specified.
    if ((MCL_NULL != (*http_processor)->configuration->load_function.rsa && MCL_NULL != (*http_processor)->configuration->save_function.rsa) || MCL_NULL != (*http_processor)->configuration->store_path)
    {
        // Load registration information via custom functions if both function pointers are not null.
        if(MCL_NULL != (*http_processor)->configuration->load_function.rsa && MCL_NULL != (*http_processor)->configuration->save_function.rsa)
        {
            return_code = _custom_load_register_info(*http_processor);
        }
        // Load registration information via file system if the path is specified.
        else
        {
			if (MCL_OK == _check_file_path((*http_processor)->configuration->store_path->buffer))
			{
				if (MCL_SECURITY_SHARED_SECRET == (*http_processor)->configuration->security_profile)
				{
					return_code = storage_load_shared_secret_registration_information((*http_processor)->configuration->store_path, &((*http_processor)->security_handler->client_id), &((*http_processor)->security_handler->client_secret), &((*http_processor)->security_handler->registration_access_token), &((*http_processor)->security_handler->registration_client_uri));
				}
				else
				{
					return_code = storage_load_rsa_registration_information((*http_processor)->configuration->store_path, &((*http_processor)->security_handler->client_id), &((*http_processor)->security_handler->rsa.public_key), &((*http_processor)->security_handler->rsa.private_key), &((*http_processor)->security_handler->registration_access_token), &((*http_processor)->security_handler->registration_client_uri));
				}
			}
			else
			{
				return_code = MCL_PATH_NOT_ACCESSIBLE;
			}
        }

        if (MCL_OK == return_code)
        {
            MCL_INFO("MCL is initialized with registration information provided in registration information file.");
        }
        else if (MCL_REGISTRATION_INFO_IS_NOT_LOADED == return_code)
        {
            if ((MCL_NULL == (*http_processor)->configuration->initial_access_token))
            {
                MCL_ERROR("No registration information is provided.");
                return_code = MCL_NO_ACCESS_TOKEN_PROVIDED;
                http_processor_destroy(http_processor);
            }
            else
            {
                // Generate RSA keys if RSA security profile is selected.
                if (MCL_SECURITY_RSA_3072 == (*http_processor)->configuration->security_profile)
                {
                    return_code = security_handler_generate_rsa_key((*http_processor)->security_handler);
                    ASSERT_STATEMENT_CODE_MESSAGE(MCL_OK == return_code, http_processor_destroy(http_processor), return_code, "RSA keys can not be generated.");
                    MCL_DEBUG("RSA keys are generated.");
                }
                MCL_INFO("MCL is initialized to onboard with initial registration information.");
                return_code = MCL_OK;
            }
        }
        else
        {
            // MCL_PATH_NOT_ACCESSIBLE
            http_processor_destroy(http_processor);
        }
    }
    else
    {
        if ((MCL_NULL == (*http_processor)->configuration->initial_access_token))
        {
            MCL_ERROR("No registration information is provided.");
            return_code = MCL_NO_ACCESS_TOKEN_PROVIDED;
            http_processor_destroy(http_processor);
        }
        else
        {
            // Generate RSA keys if RSA security profile is selected.
            if (MCL_SECURITY_RSA_3072 == (*http_processor)->configuration->security_profile)
            {
                return_code = security_handler_generate_rsa_key((*http_processor)->security_handler);
                ASSERT_STATEMENT_CODE_MESSAGE(MCL_OK == return_code, http_processor_destroy(http_processor), return_code, "RSA keys can not be generated.");
                MCL_DEBUG("RSA keys are generated.");
            }
            MCL_INFO("MCL is initialized to onboard with initial registration information.");
            return_code = MCL_OK;
        }
    }

	// TODO: Check whether or not below code should be inside communication_initialize
	// Populate endpoints in configuration
	(MCL_OK == return_code) && (return_code = _concatenate_host_and_endpoint((*http_processor)->configuration->mindsphere_hostname, &endpoint_uri[ENDPOINT_URI_REGISTER], &(*http_processor)->configuration->registration_endpoint));
	(MCL_OK == return_code) && (return_code = _concatenate_host_and_endpoint((*http_processor)->configuration->mindsphere_hostname, &endpoint_uri[ENDPOINT_URI_EXCHANGE], &(*http_processor)->configuration->exchange_endpoint));
	(MCL_OK == return_code) && (return_code = _concatenate_host_and_endpoint((*http_processor)->configuration->mindsphere_hostname, &endpoint_uri[ENDPOINT_URI_ACCESS_TOKEN], &(*http_processor)->configuration->access_token_endpoint));

    DEBUG_LEAVE("retVal = <%d>", return_code);
    return return_code;
}

E_MCL_ERROR_CODE http_processor_register(http_processor_t *http_processor)
{
    DEBUG_ENTRY("http_processor_t *http_processor = <%p>", http_processor)

    E_MCL_ERROR_CODE result = MCL_FAIL;

    http_request_t *http_request = MCL_NULL;
    http_response_t *http_response = MCL_NULL;
    string_t *access_token = MCL_NULL;
    string_t *registration_uri = MCL_NULL;
    string_t *payload = MCL_NULL;
	string_t *correlation_id = MCL_NULL;
    mcl_size_t header_size = 6;
    E_MCL_HTTP_METHOD http_method;

    // Determine if onboarding or key rotation is requested.
    if (MCL_NULL == http_processor->security_handler->registration_access_token)
    {
        // Compose payload of the request.
        if (MCL_SECURITY_SHARED_SECRET == http_processor->configuration->security_profile)
        {
            result = string_initialize_new("{}", 2, &payload);
        }
        else if (MCL_SECURITY_RSA_3072 == http_processor->configuration->security_profile)
        {
            result = _compose_rsa_onboarding_json(http_processor->security_handler, &payload);
        }
        ASSERT_CODE_MESSAGE(MCL_OK == result, result, "Payload of onboarding request can not be constructed.");

        http_method = MCL_HTTP_POST;
        registration_uri = http_processor->configuration->registration_endpoint;

        // Add "Bearer" before initial access token.
        result = string_initialize_new(MCL_NULL, 7 + http_processor->configuration->initial_access_token->length, &access_token);
        ASSERT_STATEMENT_CODE_MESSAGE(MCL_OK == result, string_destroy(&payload), result, "Memory can not be allocated for initial access token.");
        result = string_util_snprintf(access_token->buffer, access_token->length + 1, "Bearer %s", http_processor->configuration->initial_access_token->buffer);
    }
    else
    {
        // Compose payload of the request.
        if (MCL_SECURITY_SHARED_SECRET == http_processor->configuration->security_profile)
        {
            // TODO: Compose payload as JSON string using JSON functions
            if (MCL_OK == (result = string_initialize_new(MCL_NULL, 16 + http_processor->security_handler->client_id->length, &payload)))
            {
                result = string_util_snprintf(payload->buffer, payload->length + 1, "{\"client_id\":\"%s\"}", http_processor->security_handler->client_id->buffer);
            }
        }
        else if (MCL_SECURITY_RSA_3072 == http_processor->configuration->security_profile)
        {
            result = _compose_rsa_key_rotation_json(http_processor->security_handler, &payload);
        }
        ASSERT_STATEMENT_CODE_MESSAGE(MCL_OK == result, string_destroy(&payload), result, "Payload of key rotation request can not be constructed.");

        http_method = MCL_HTTP_PUT;
        registration_uri = http_processor->security_handler->registration_client_uri;

        // Add "Bearer" before registration access token.
        result = string_initialize_new(MCL_NULL, 7 + http_processor->security_handler->registration_access_token->length, &access_token);
        ASSERT_STATEMENT_CODE_MESSAGE(MCL_OK == result, string_destroy(&payload), result, "Memory can not be allocated for registration access token.");
        result = string_util_snprintf(access_token->buffer, access_token->length + 1, "Bearer %s",
                                      http_processor->security_handler->registration_access_token->buffer);
    }

    (MCL_OK == result) && (result = http_request_initialize(http_method, registration_uri, header_size, payload->length, HTTP_REQUEST_RESIZE_ENABLED, http_processor->configuration->user_agent,
                 http_processor->configuration->max_http_payload_size, &http_request));

    if (MCL_OK != result)
    {
        string_destroy(&payload);
        string_destroy(&access_token);
        http_request_destroy(&http_request);
        MCL_ERROR_RETURN(result, "HTTP request can not be initialized.");
    }

    // Compose header and payload fields of HTTP request.
    string_util_memcpy(http_request->payload, payload->buffer, payload->length);
    result = http_request_add_header(http_request, &http_header_names[HTTP_HEADER_CONTENT_TYPE], &content_type_values[CONTENT_TYPE_APPLICATION_JSON]);
    (MCL_OK == result) && (result = http_request_add_header(http_request, &http_header_names[HTTP_HEADER_ACCEPT], &content_type_values[CONTENT_TYPE_APPLICATION_JSON]));
    (MCL_OK == result) && (result = http_request_add_header(http_request, &http_header_names[HTTP_HEADER_AUTHORIZATION], access_token));

    (MCL_OK == result) && (result = _generate_correlation_id_string(&correlation_id));
    (MCL_OK == result) && (result = http_request_add_header(http_request, &http_header_names[HTTP_HEADER_CORRELATION_ID], correlation_id));

    // Send the request and get the response.
    if (MCL_OK == result)
    {
        // Send the onboarding request and retrieve the response.
        if (MCL_OK != (result = http_client_send(http_processor->http_client, http_request, MCL_NULL, &http_response)))
        {
            MCL_ERROR("HTTP request can not be sent to the server.");
        }
        else
        {
            E_MCL_ERROR_CODE server_response = _evaluate_response_codes(http_response);
			if ((MCL_NULL == http_processor->security_handler->registration_access_token)
			    && (MCL_OK != (result = (MCL_CREATED == server_response ? MCL_OK : (MCL_OK == server_response ? MCL_UNEXPECTED_RESULT_CODE : server_response)))))
			{
				MCL_ERROR("Onboarding failed: Correlation-ID = \"%s\"", correlation_id->buffer);
			}
			else if ((MCL_NULL != http_processor->security_handler->registration_access_token)
				&& (MCL_OK != (result = server_response)))
			{
				MCL_ERROR("Key rotation failed: Correlation-ID = \"%s\"", correlation_id->buffer);
			}
            else
            {
                MCL_INFO("Correlation-ID = \"%s\"", correlation_id->buffer);
                if (MCL_SECURITY_SHARED_SECRET == http_processor->configuration->security_profile)
                {
                    result = _process_registration_response_shared_secret(http_processor, http_response);
                }
                else
                {
                    result = _process_registration_response_rsa_3072(http_processor, http_response);
                }

                if (MCL_OK == result)
                {
                    result = _save_registration_information(http_processor);
                }
            }
        }
    }

    // Clean up.
    string_destroy(&correlation_id);
    string_destroy(&payload);
    string_destroy(&access_token);
    http_request_destroy(&http_request);
    http_response_destroy(&http_response);

    DEBUG_LEAVE("retVal = <%d>", result);
    return result;
}

E_MCL_ERROR_CODE http_processor_update_security_information(http_processor_t *http_processor)
{
    DEBUG_ENTRY("http_processor_t *http_processor = <%p>", http_processor)

    E_MCL_ERROR_CODE code;

    // These will be used in any security profile.
    char *client_id = MCL_NULL;
    char *registration_access_token = MCL_NULL;
    char *registration_uri = MCL_NULL;

    string_t *client_id_string = MCL_NULL;
    string_t *registration_access_token_string = MCL_NULL;
    string_t *registration_uri_string = MCL_NULL;

    // For shared secret.
    char *client_secret = MCL_NULL;
    string_t *client_secret_string = MCL_NULL;

    // For RSA 3072.
    char *public_key = MCL_NULL;
    char *private_key = MCL_NULL;

    mcl_bool_t callbacks_are_used = (MCL_NULL != http_processor->configuration->load_function.shared_secret && MCL_NULL != http_processor->configuration->save_function.shared_secret);
    ASSERT_CODE_MESSAGE(MCL_TRUE == (callbacks_are_used || MCL_NULL != http_processor->configuration->store_path), MCL_FAIL, "There is no way to update security information.");

    if(callbacks_are_used)
    {
        if(MCL_SECURITY_SHARED_SECRET == http_processor->configuration->security_profile)
        {
            code = http_processor->configuration->load_function.shared_secret(&client_id, &client_secret, &registration_access_token, &registration_uri);
        }
        else
        {
            code = http_processor->configuration->load_function.rsa(&client_id, &public_key, &private_key, &registration_access_token, &registration_uri);
        }
    }
    else
    {
        if(MCL_SECURITY_SHARED_SECRET == http_processor->configuration->security_profile)
        {
            code = storage_load_shared_secret_registration_information(http_processor->configuration->store_path, &client_id_string, &client_secret_string, &registration_access_token_string, &registration_uri_string);
            client_secret = client_secret_string->buffer;
        }
        else
        {
            code = storage_load_rsa_registration_information(http_processor->configuration->store_path, &client_id_string, &public_key, &private_key, &registration_access_token_string, &registration_uri_string);
        }
        client_id = client_id_string->buffer;
        registration_access_token = registration_access_token_string->buffer;
    }

    // If loading security information is failed, user must have set all parameters to NULL. No need to clean up.
    ASSERT_CODE_MESSAGE(MCL_OK == code, code, "Security information cannot be loaded.");

    // Registration URI will not be used. 
    MCL_NULL == registration_uri_string ? MCL_FREE(registration_uri) : string_destroy(&registration_uri_string);

    // Check whether client id is correct.
    code = string_compare_with_cstr(http_processor->security_handler->client_id, client_id);

    MCL_NULL == client_id_string ? MCL_FREE(client_id) : string_destroy(&client_id_string);

    if(MCL_OK == code)
    {
        if (MCL_SECURITY_SHARED_SECRET == http_processor->configuration->security_profile)
        {
            code = string_compare_with_cstr(http_processor->security_handler->client_secret, client_secret);
            (MCL_OK == code) && (code = string_compare_with_cstr(http_processor->security_handler->registration_access_token, registration_access_token));
        }
        else
        {
            mcl_size_t length = string_util_strlen(http_processor->security_handler->rsa.private_key);
            code = MCL_FAIL;

            if(length == string_util_strlen(private_key))
            {
                code = string_util_strncmp(private_key, http_processor->security_handler->rsa.private_key, length);
            }
        }

        // If code == MCL_OK, client secret is not different from what we have.
        code = (MCL_OK == code) ? MCL_SECURITY_UP_TO_DATE : MCL_OK;
    }

    if(MCL_OK == code && callbacks_are_used)
    {
        code = string_initialize_dynamic(registration_access_token, 0, &registration_access_token_string);

        if(MCL_OK == code && MCL_SECURITY_SHARED_SECRET == http_processor->configuration->security_profile)
        {
            code = string_initialize_dynamic(client_secret, 0, &client_secret_string);
        }
    }

    if(MCL_OK == code)
    {
        // Everything is fine, now we can replace old information.
        // There is no need to check security profile, unrelated parameters will be just reassigned to NULL.
        string_destroy(&(http_processor->security_handler->registration_access_token));
        string_destroy(&(http_processor->security_handler->client_secret));
        MCL_FREE(http_processor->security_handler->rsa.public_key);
        MCL_FREE(http_processor->security_handler->rsa.private_key);

        http_processor->security_handler->registration_access_token = registration_access_token_string;
        http_processor->security_handler->client_secret = client_secret_string;
        http_processor->security_handler->rsa.public_key = public_key;
        http_processor->security_handler->rsa.private_key = private_key;
    }
    else
    {
        MCL_NULL == registration_access_token_string ? MCL_FREE(registration_access_token) : string_destroy(&registration_access_token_string);
        MCL_NULL == client_secret_string ? MCL_FREE(client_secret) : string_destroy(&client_secret_string);
        MCL_FREE(public_key);
        MCL_FREE(private_key);
    }
	
	DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

E_MCL_ERROR_CODE http_processor_get_access_token(http_processor_t *http_processor)
{
    DEBUG_ENTRY("http_processor_t *http_processor = <%p>", http_processor)

    E_MCL_ERROR_CODE code;
    string_t *request_payload = MCL_NULL;
    string_t *correlation_id = MCL_NULL;
    http_request_t *request = MCL_NULL;
    http_response_t *response = MCL_NULL;
    E_MCL_ERROR_CODE optional_field_code = MCL_FAIL;
    string_t *server_time_header = MCL_NULL;
    json_t *response_payload = MCL_NULL;
    json_t *access_token = MCL_NULL;

    // Create access token request payload.
    code = _compose_access_token_request_payload(http_processor, &request_payload);

    // Initialize HTTP request.
    (MCL_OK == code) && (code = http_request_initialize(MCL_HTTP_POST, http_processor->configuration->access_token_endpoint, 2, request_payload->length, HTTP_REQUEST_RESIZE_ENABLED, http_processor->configuration->user_agent,
        http_processor->configuration->max_http_payload_size, &request));

    // Add headers to the request.
    (MCL_OK == code) && (code = http_request_add_header(request, &http_header_names[HTTP_HEADER_CONTENT_TYPE], &content_type_values[CONTENT_TYPE_URL_ENCODED]));
    (MCL_OK == code) && (code = _generate_correlation_id_string(&correlation_id));
    (MCL_OK == code) && (code = http_request_add_header(request, &http_header_names[HTTP_HEADER_CORRELATION_ID], correlation_id));

    // Add payload to the request.
    if (MCL_OK == code)
    {
        string_util_memcpy(request->payload, request_payload->buffer, request_payload->length);
    }
    string_destroy(&request_payload);

    // Send the request.
    (MCL_OK == code) && (code = http_client_send(http_processor->http_client, request, MCL_NULL, &response));
    http_request_destroy(&request);

    // Get server time which is an optional header in the response.
    (MCL_OK == code) && (optional_field_code = http_response_get_header(response, http_header_names[HTTP_HEADER_SERVER_TIME].buffer, &server_time_header));

    if(MCL_OK == code)
    {
        if (MCL_OK == optional_field_code)
        {
            string_destroy(&http_processor->security_handler->last_token_time);
            http_processor->security_handler->last_token_time = server_time_header;
        }

        code = _evaluate_response_codes(response);
        if(MCL_OK == code)
        {
            MCL_INFO("Correlation-ID = \"%s\"", correlation_id->buffer);
        }
        else
        {
            MCL_ERROR("Getting access token failed. Correlation-ID = \"%s\"", correlation_id->buffer);
        }
    }
    string_destroy(&correlation_id);

    // Parse the response to get access token.
    (MCL_OK == code) && (code = json_util_parse((const char*)http_response_get_payload(response), &response_payload));
    http_response_destroy(&response);

    (MCL_OK == code) && (code = json_util_get_object_item(response_payload, JSON_NAME_ACCESS_TOKEN, &access_token));

    string_destroy(&http_processor->security_handler->access_token);
    (MCL_OK == code) && (code = json_util_get_string(access_token, &http_processor->security_handler->access_token));

    MCL_FREE(access_token);
    json_util_destroy(&response_payload);

    DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

#if MCL_FILE_DOWNLOAD_ENABLED
E_MCL_ERROR_CODE http_processor_download(http_processor_t *http_processor, mcl_uint8_t * buffer, mcl_size_t buffer_size, mcl_size_t start_byte, mcl_size_t end_byte, string_t *file_id, mcl_bool_t with_range, file_t **file)
{
	DEBUG_ENTRY("http_processor_t *http_processor = <%p>, mcl_uint8_t *buffer = <%p>, mcl_size_t start_byte = <%u>, mcl_size_t end_byte <%u>, string_t *file_id = <%p>, mcl_bool_t with_range = <%d>, file_t **file = <%p>",
		http_processor, buffer, start_byte, end_byte, file_id, with_range, file)

	// Prepare local vars
	http_request_t *request = MCL_NULL;
	mcl_size_t header_size = 3;
	string_t *uri = MCL_NULL;
	string_t *uri_temp = MCL_NULL;

	E_MCL_ERROR_CODE result = _concatenate_host_and_endpoint(http_processor->configuration->mindsphere_hostname, &endpoint_uri[ENDPOINT_URI_DOWNLOAD], &uri_temp);
	(MCL_OK == result) && (result = _concatenate_host_and_endpoint(uri_temp, file_id, &uri));
	string_destroy(&uri_temp);

	(MCL_OK == result) && (result = http_request_initialize(MCL_HTTP_GET, uri, header_size, 0, HTTP_REQUEST_RESIZE_ENABLED, http_processor->configuration->user_agent,
		http_processor->configuration->max_http_payload_size, &request));
	string_destroy(&uri);

	// Add authentication header
    (MCL_OK == result) && (result = _add_authentication_header_to_request(http_processor, request, MCL_TRUE));

	string_t *accept_header = MCL_NULL;
	(MCL_OK == result) && (result = string_initialize_static("application/octet-stream", 0, &accept_header));
	(MCL_OK == result) && (result = http_request_add_header(request, &http_header_names[HTTP_HEADER_ACCEPT], accept_header));
	string_destroy(&accept_header);

	// If download requested for range then add Range header
    (MCL_OK == result) && (MCL_TRUE == with_range) && (result = _add_range_header(request, start_byte, end_byte));

	string_t *correlation_id = MCL_NULL;
	(MCL_OK == result) && (result = _generate_correlation_id_string(&correlation_id));
	(MCL_OK == result) && (result = http_request_add_header(request, &http_header_names[HTTP_HEADER_CORRELATION_ID], correlation_id));

	// Send request
	http_response_t *response = MCL_NULL;
	(MCL_OK == result) && (result = http_client_send(http_processor->http_client, request, MCL_NULL, &response));
	http_request_destroy(&request);

    if(MCL_OK == result)
    {
        E_MCL_ERROR_CODE server_response = _evaluate_response_codes(response);
        result = (MCL_PARTIAL_CONTENT == server_response ? MCL_OK : server_response);

        if(MCL_OK == result)
        {
            MCL_INFO("Correlation-ID = \"%s\"", correlation_id->buffer);
        }
        else
        {
            MCL_ERROR("Acknowledge events failed. Correlation-ID = \"%s\"", correlation_id->buffer);
        }
    }

    string_destroy(&correlation_id);

	// Get file name.
	string_t *content_disposition_header = MCL_NULL;
	(MCL_OK == result) && (result = http_response_get_header(response, http_header_names[HTTP_HEADER_CONTENT_DISPOSITION].buffer, &content_disposition_header));
	ASSERT_STATEMENT_CODE_MESSAGE(MCL_OK == result, http_response_destroy(&response), result, "Could not get header.");

	result = file_initialize("", MCL_NULL, "", "", MCL_NULL, file);

	if(MCL_NULL == *file)
	{
	    http_response_destroy(&response);
	    string_destroy(&content_disposition_header);
	    MCL_ERROR_RETURN(result, "File initialization failed.");
	}

	if (MCL_NULL != content_disposition_header)
	{
		result = _get_file_name(content_disposition_header->buffer, &(*file)->meta.payload.details.file_details.file_name);
		string_destroy(&content_disposition_header);

		if(MCL_OK != result)
		{
		    file_destroy(file);
		    http_response_destroy(&response);
		    MCL_ERROR_RETURN(result, "Getting file name failed.")
		}
	}
	
	// Copy response payload to given buffer.
	(*file)->payload.size = response->payload_size;

	if (buffer_size >= response->payload_size)
	{
		string_util_memcpy(buffer, response->payload, response->payload_size);
		(*file)->payload.buffer = buffer;
	}
	else
	{
		result = MCL_BUFFER_OVERFLOW_ERROR;
	}

	if (MCL_TRUE == with_range)
	{
		// Add range values do file
		ASSERT_CODE_MESSAGE(MCL_OK == _add_actual_range_to_file(response, *file), MCL_FAIL, "Could not obtain range values from Content-Range header");
	}

	http_response_destroy(&response);

    DEBUG_LEAVE("retVal = <%d>", result);
    return result;
}
#endif

E_MCL_ERROR_CODE _concatenate_host_and_endpoint(string_t *host, string_t *endpoint, string_t **uri)
{
    DEBUG_ENTRY("string_t *host = <%p>, string_t *endpoint = <%p>, string_t **uri = <%p>", host, endpoint, uri)

    E_MCL_ERROR_CODE return_code = MCL_FAIL;

    // Size of the URI string (host and endpoint with '/' in between).
    mcl_size_t uri_length = host->length + endpoint->length + 1;

    // Allocate memory for the request URI.
    if (MCL_OK == (return_code = string_initialize_new(MCL_NULL, uri_length, uri)))
    {
        return_code = string_util_snprintf((*uri)->buffer, uri_length + 1, "%s/%s", host->buffer, endpoint->buffer);
        ASSERT_STATEMENT_CODE_MESSAGE(MCL_OK == return_code, string_destroy(uri), return_code, "Host and endpoint can not be concatenated.");
    }

    DEBUG_LEAVE("retVal = <%d>", return_code);
    return return_code;
}

E_MCL_ERROR_CODE http_processor_exchange(http_processor_t *http_processor, store_t *store, void **reserved)
{
    DEBUG_ENTRY("http_processor_t *http_processor = <%p>, store_t *store = <%p>, void **reserved = <%p>", http_processor, store, reserved)

	E_MCL_ERROR_CODE result;

	// TODO : Below var values needs to be optimized :
	mcl_size_t header_size = 3;
	mcl_size_t payload_size = 200;

    ASSERT_CODE_MESSAGE(0 < store->high_priority_list->count + store->low_priority_list->count, MCL_STORE_IS_EMPTY, "Received store doesn't have any data inside!");

    // Continue generating an http request and send, until no data left in the store OR an error received:
	do
	{
		// Prepare local vars
		http_request_t *request = MCL_NULL;
		string_t *correlation_id = MCL_NULL;

		MCL_DEBUG("Start of a fill and send iteration");

        result = http_request_initialize(MCL_HTTP_POST, http_processor->configuration->exchange_endpoint, header_size, payload_size, HTTP_REQUEST_RESIZE_ENABLED, http_processor->configuration->user_agent,
            http_processor->configuration->max_http_payload_size, &request);

		ASSERT_CODE_MESSAGE(MCL_OK == result, result, "Initializing HTTP Request has failed!");

		result = _exchange_initialize_http_request_headers(http_processor, request, MCL_TRUE);

        if (MCL_OK != result)
        {
            http_request_destroy(&request);
            MCL_ERROR_RETURN(result, "Initializing HTTP Request headers failed!");
        }

		MCL_DEBUG("A new http_request has been initialized");

		result = _exchange_fill_http_request(http_processor, store, request);

		(MCL_OK == result) && (result = _generate_correlation_id_string(&correlation_id));
		(MCL_OK == result) && (result = http_request_add_header(request, &http_header_names[HTTP_HEADER_CORRELATION_ID], correlation_id));

		if (MCL_OK == result)
		{
			// Check if http_request is ready to be sent.
			// Send it if it is ready
			// request ready to be sent :
			http_response_t *response = MCL_NULL;

			// Not checking the result of send operation. Result will be evaluated based on the response.
			result = http_client_send(http_processor->http_client, request, MCL_NULL, &response);

			// request can be destroyed now ;
	        http_request_destroy(&request);

	        // then evaluate the response :
			result = _exchange_evaluate_response(store, result, response, NULL, correlation_id);

			if (MCL_OK != result)
			{
	            string_destroy(&correlation_id);
				MCL_DEBUG("Evaluating the result returned as failed. Terminating the exchange operation.");
				break;
			}
		}
		else
		{
			// error case. Return the error.
		    string_destroy(&correlation_id);
			MCL_DEBUG("An error has been occured during http request preparation = <%d>. Terminating the exchange operation.", result);
			http_request_destroy(&request);
			break;
		}

		string_destroy(&correlation_id);
	} 
	while (0 < store_get_data_count(store));

	DEBUG_LEAVE("retVal = <%d>", result);
	return result;
}

E_MCL_ERROR_CODE http_processor_stream(http_processor_t *http_processor, store_t *store, void **reserved)
{
#if MCL_STREAM_ENABLED
    DEBUG_ENTRY("http_processor_t *http_processor = <%p>, store_t *store = <%p>, void **reserved = <%p>", http_processor, store, reserved)

    ASSERT_CODE_MESSAGE(0 < store->high_priority_list->count + store->low_priority_list->count, MCL_STORE_IS_EMPTY, "Received store doesn't have any data inside!");

    // 0- Prepare local vars
    http_request_t *request = MCL_NULL;
    http_response_t *response = MCL_NULL;

    // TODO : Below var values needs to be corrected :
    mcl_size_t header_size = 2;
    mcl_size_t payload_size = 0;

    // adjust callback function and data :
    http_processor_stream_callback_context_t http_processor_callback_context =
    {
        http_processor, store, MCL_NULL, 0, MCL_OK
    };
    http_client_send_callback_info_t send_callback_info;
    send_callback_info.read_callback = _stream_callback;
    send_callback_info.user_context = &http_processor_callback_context;

	E_MCL_ERROR_CODE result = MCL_FAIL;

    while (0 < store_get_data_count(store))
    {
        // Initialize a new http_request:
        result = http_request_initialize(MCL_HTTP_POST, http_processor->configuration->exchange_endpoint, header_size, payload_size, HTTP_REQUEST_RESIZE_DISABLED, http_processor->configuration->user_agent,
                                         http_processor->configuration->max_http_payload_size, &request);

        ASSERT_CODE_MESSAGE(MCL_OK == result, result, "Initializing HTTP Request is failed!");

        // update the request info in callback info :
        http_processor_callback_context.request = request;

        result = _exchange_initialize_http_request_headers(http_processor, request, MCL_TRUE);

        if (MCL_OK != result)
        {
            http_request_destroy(&request);
            MCL_ERROR_RETURN(result, "Initializing HTTP Request headers failed!");
        }

        string_t *correlation_id = MCL_NULL;
        (MCL_OK == result) && (result = _generate_correlation_id_string(&correlation_id));
        (MCL_OK == result) && (result = http_request_add_header(request, &http_header_names[HTTP_HEADER_CORRELATION_ID], correlation_id));
        (MCL_OK == result) && (result = http_client_send(http_processor->http_client, request, &send_callback_info, &response));

        // clean up request :
        http_request_destroy(&request);

        // then evaluate the response :
        result = _exchange_evaluate_response(store, result, response, NULL, correlation_id);
        string_destroy(&correlation_id);

        if (MCL_OK != result)
        {
            MCL_DEBUG("Evaluating the result returned as failed. Terminating the exchange operation.");
            break;
        }
    }

    DEBUG_LEAVE("retVal = <%d>", result);
    return result;
#else
	return MCL_OPERATION_IS_NOT_SUPPORTED;
#endif // MCL_STREAM_ENABLED
}

void http_processor_destroy(http_processor_t **http_processor)
{
    DEBUG_ENTRY("http_processor_t **http_processor = <%p>", http_processor)

    if (MCL_NULL != *http_processor)
    {
        // Destroy http client handler.
        http_client_destroy(&((*http_processor)->http_client));

        // Destroy security handler.
        security_handler_destroy(&((*http_processor)->security_handler));

		// TODO: Check whether or not below code should be in communication_destroy.
		string_destroy(&((*http_processor)->configuration->access_token_endpoint));
		string_destroy(&((*http_processor)->configuration->exchange_endpoint));
		string_destroy(&((*http_processor)->configuration->registration_endpoint));

        // Free http_processor handle.
        MCL_FREE(*http_processor);

        MCL_DEBUG("Http processor handle is destroyed.");
    }
    else
    {
        MCL_DEBUG("Http processor handle is already NULL.");
    }

    DEBUG_LEAVE("retVal = void");
}

static E_MCL_ERROR_CODE _exchange_add_current_data_to_request_by_streaming(store_data_t *current_store_data, http_request_t *request)
{
    DEBUG_ENTRY("store_data_t *current_store_data = <%p>, http_request_t *request = <%p>", current_store_data, request)

    E_MCL_ERROR_CODE result = MCL_OK;

	mcl_size_t available_size_in_request;

	// get content type and id to use when calling add_tuple/single  :
	string_t *meta_content_type = MCL_NULL;
	string_t *payload_content_type = MCL_NULL;
	string_t *meta_content_id = MCL_NULL;

    mcl_size_t left_meta_size;
    mcl_size_t left_payload_size;

    // If stream_info is NULL, initialize it :
    if (MCL_NULL == current_store_data->stream_info)
    {
        MCL_NEW(current_store_data->stream_info);
        ASSERT_CODE_MESSAGE(MCL_NULL != current_store_data->stream_info, MCL_OUT_OF_MEMORY, "Memory can not be allocated for stream_info.");

        // Initialize stream information.
        current_store_data->stream_info->meta_stream_index = 0;
        current_store_data->stream_info->payload_stream_index = 0;
        current_store_data->stream_info->tuple_subboundary = MCL_NULL;

        // For explanation of -2 please check below usage :
        current_store_data->stream_info->meta_state = -2;

        // For explanation of -1 please check below usage :
        current_store_data->stream_info->payload_state = -1;
    }

    ASSERT_CODE_MESSAGE(MCL_OK == _exchange_store_data_get_content_info(current_store_data, &meta_content_type, &meta_content_id, &payload_content_type), MCL_FAIL,
                        "Get content type and id info failed!");

    // tuple
    MCL_DEBUG("Data is tuple.");

    // start tuple if not started before :
    if (-2 == current_store_data->stream_info->meta_state)
    {
        result = http_request_start_tuple(request);
        ASSERT_CODE_MESSAGE(MCL_OK == result, result, "Starting tuple is failed!");

        // increasing meta state to -1 to indicate that a tuple is opened for this:
        current_store_data->stream_info->meta_state++;
    }

    // start tuple sub section for meta if not started before
    if (-1 == current_store_data->stream_info->meta_state)
    {
        result = http_request_start_tuple_sub_section(request, meta_content_type, meta_content_id, &(current_store_data->stream_info->tuple_subboundary));
        ASSERT_CODE_MESSAGE(MCL_OK == result, result, "Starting tuple section for meta is failed!");

        // increasing meta state to 0 to indicate that a subsection is opened for this:
        current_store_data->stream_info->meta_state++;
    }

    // calculate meta left size :
    left_meta_size = current_store_data->meta->length - current_store_data->stream_info->meta_stream_index;
    if (0 < left_meta_size)
    {
        // get available space :
        available_size_in_request = http_request_get_available_space_for_raw_data(request);

        if (0 < available_size_in_request)
        {
            // compare with left meta size and calculate how much meta can be written ( use left_meta_size to hold that) :
            if (left_meta_size > available_size_in_request)
            {
                MCL_DEBUG("Left meta size has been reduced to available size in request = <%u>", available_size_in_request);
                left_meta_size = available_size_in_request;
            }

            // write left meta :
            result = http_request_add_raw_data(request, _get_payload_from_buffer, MCL_NULL,
                                               (void *)(current_store_data->meta->buffer + current_store_data->stream_info->meta_stream_index), left_meta_size, MCL_NULL);
            ASSERT_CODE_MESSAGE(MCL_OK == result, result, "Add raw data has been failed for meta!");

            // left meta has been written. mark the position:
            current_store_data->stream_info->meta_stream_index += left_meta_size;
        }
        else
        {
            MCL_DEBUG("Available space in request is 0.");
            result = MCL_HTTP_REQUEST_NO_MORE_SPACE;
        }
    }

    // check if meta write completed :
    left_meta_size = current_store_data->meta->length - current_store_data->stream_info->meta_stream_index;
    if (0 == left_meta_size)
    {
        // start a sub section for payload if it is not started before :
        if (-1 == current_store_data->stream_info->payload_state)
        {
            result = http_request_start_tuple_sub_section(request, payload_content_type, MCL_NULL, &(current_store_data->stream_info->tuple_subboundary));
            ASSERT_CODE_MESSAGE(MCL_OK == result, result, "Starting tuple section for payload is failed!");

            // increasing payload state to 0 to indicate that a subsection is opened for this:
            current_store_data->stream_info->payload_state++;
        }

        left_payload_size = current_store_data->payload_size - current_store_data->stream_info->payload_stream_index;
        if ((0 < left_payload_size) || (STORE_DATA_STREAM == current_store_data->type))
        {
            available_size_in_request = http_request_get_available_space_for_raw_data(request);

            if (0 < available_size_in_request)
            {
                // compare with left meta size and calculate how much meta can be written ( use left_meta_size to hold that) :
                if (left_payload_size > available_size_in_request)
                {
                    MCL_DEBUG("Left payload size has been equalized to available size in request = <%u>", available_size_in_request);
                    left_payload_size = available_size_in_request;
                }

                // write left meta :
                if (STORE_DATA_FILE == current_store_data->type)
                {
                    file_t *file;

                    MCL_DEBUG("Type is STORE_DATA_FILE. Calling file read callback function.");

                    file = (file_t *)current_store_data->data;
                    result = http_request_add_raw_data(request, _get_payload_from_file, MCL_NULL, (void *)(file->descriptor), left_payload_size, MCL_NULL);
                }
                else if (STORE_DATA_STREAM == current_store_data->type)
                {
                    mcl_size_t actual_written = 0;
                    stream_data_t *stream_data = (stream_data_t *)current_store_data->data;
                    MCL_DEBUG("Type is STORE_DATA_STREAM. Calling user provided read callback function = <%p>, with user_context = <%p>", stream_data->read_callback,
                          stream_data->user_context);

                    // Using the available size since this is a stream data :
                    result = http_request_add_raw_data(request, (payload_copy_callback_t)stream_data->read_callback, stream_data->user_context, MCL_NULL,
                                                       available_size_in_request, &actual_written);

                    if ((MCL_OK == result) && (0 == actual_written))
                    {
                        MCL_DEBUG("User callback completed writing data.");

                        // set payload_size and payload_stream_index to 0 to mark it WRITTEN below :
                        current_store_data->payload_size = 0;
                        current_store_data->stream_info->payload_stream_index = 0;
                    }
                }
                else
                {
                    MCL_DEBUG("Type is <standard types>. Calling memory read callback function.");
                    result = http_request_add_raw_data(request, _get_payload_from_buffer, MCL_NULL,
                                                       (void *)(current_store_data->payload_buffer + current_store_data->stream_info->payload_stream_index),
                                                       left_payload_size, MCL_NULL);
                }

                ASSERT_CODE_MESSAGE(MCL_OK == result, result, "Add raw data has been failed for payload!");

                // left meta has been written. mark the position:
                current_store_data->stream_info->payload_stream_index += left_payload_size;
            }
            else
            {
                MCL_DEBUG("Available space in request is 0.");
                result = MCL_HTTP_REQUEST_NO_MORE_SPACE;
            }
        }
    }

    // if meta and payload written is completed end tuple :
    left_payload_size = current_store_data->payload_size - current_store_data->stream_info->payload_stream_index;
    if ((0 == left_meta_size) && (0 == left_payload_size))
    {
        result = http_request_end_tuple_sub_section(request, current_store_data->stream_info->tuple_subboundary);
        ASSERT_CODE_MESSAGE(MCL_OK == result, result, "End tuple section has been failed!");

        MCL_DEBUG("Meta and payload have been written completely. Updating its state to WRITTEN.");
        store_data_set_state(current_store_data, DATA_STATE_WRITTEN);
    }

    DEBUG_LEAVE("retVal = <%d>", result);
    return result;
}

static E_MCL_ERROR_CODE _exchange_add_current_data_to_request(store_data_t *current_store_data, http_request_t *request)
{
    DEBUG_ENTRY("store_data_t *current_store_data = <%p>, http_request_t *request = <%p>", current_store_data, request)

    // Decide whether to add a single or tuple :
    // if current item has both a meta and a payload, it requires a tuple to be sent

	E_MCL_ERROR_CODE result;

    // if this is a stream_data, we couldn't know its size. Safe action to do so is to return MCL_HTTP_REQUEST_NO_MORE_SPACE.
    // This way it will redirected to _exchange_add_current_data_to_request_by_streaming() function :
    if (STORE_DATA_STREAM == current_store_data->type)
    {
        MCL_DEBUG("This is a streaming data. We don't know the size. Returning MCL_HTTP_REQUEST_NO_MORE_SPACE to make it be written with stream method.");
        result = MCL_HTTP_REQUEST_NO_MORE_SPACE;
    }
    else
    {
        // get content type and id to use when calling add_tuple/single  :
        string_t *meta_content_type = MCL_NULL;
        string_t *payload_content_type = MCL_NULL;
        string_t *meta_content_id = MCL_NULL;
        ASSERT_CODE_MESSAGE(MCL_OK == _exchange_store_data_get_content_info(current_store_data, &meta_content_type, &meta_content_id, &payload_content_type), MCL_FAIL,
                            "Get content type and id info failed!");

        // Check if the store item is file or not.
        if (STORE_DATA_FILE == current_store_data->type)
        {
            file_t *file = (file_t *)current_store_data->data;
            result = http_request_add_tuple(request, current_store_data->meta, meta_content_type, _get_payload_from_file, MCL_NULL, file->descriptor,
                                            current_store_data->payload_size, payload_content_type);
        }
        else
        {
            result = http_request_add_tuple(request, current_store_data->meta, meta_content_type, _get_payload_from_buffer, MCL_NULL, current_store_data->payload_buffer,
                                            current_store_data->payload_size, payload_content_type);
        }

        // check the result and adjust written info of the data ::
        if (MCL_OK == result)
        {
            // successfully written. Update its state to written:
            store_data_set_state(current_store_data, DATA_STATE_WRITTEN);
        }
        else if (MCL_HTTP_REQUEST_NO_MORE_SPACE == result)
        {
                // meta and payload data couldn't be written to http_request. here we are not increasing write offset.
            MCL_DEBUG("No more space left in the request for normal writing the data.");
        }
        else
        {
            // error returned. not increasing write offset.
            // error will be directly returned.
            MCL_DEBUG("Error response received from http_request_add_tuple");
        }
    }

    DEBUG_LEAVE("retVal = <%d>", result);
    return result;
}

static E_MCL_ERROR_CODE _exchange_prepare_data(store_data_t *store_data)
{
	DEBUG_ENTRY("store_data_t *store_data = <%p>", store_data)

	event_list_t *event_list;
	string_t *payload_string = MCL_NULL;

    // generate the meta/payload string based on the type:
    if (STORE_DATA_TIME_SERIES == store_data->type)
    {
		time_series_t *time_series;
		string_t *payload_string = MCL_NULL;

        MCL_DEBUG("Item type = <time_series>");

        time_series = (time_series_t *)store_data->data;

        // generate the meta string :
        ASSERT_CODE_MESSAGE(MCL_OK == json_from_item_meta(&time_series->meta, &store_data->meta), MCL_FAIL, "Get meta string from item meta for time_series has been failed!");

        if (MCL_OK != json_from_time_series_payload(&time_series->payload, &payload_string))
        {
            string_destroy(&store_data->meta);
            MCL_ERROR_RETURN(MCL_FAIL, "Get meta string from item meta for time_series has been failed!");
        }
        store_data->payload_buffer = (mcl_uint8_t *)payload_string->buffer;
        store_data->payload_size = payload_string->length;

        // only buffer of the string_t is used as the payload. string_t object needs to be freed. We don't use string_destroy because it also frees the buffer which we don't want.
        MCL_FREE(payload_string);
    }
    else if (STORE_DATA_EVENT_LIST == store_data->type)
    {
        MCL_DEBUG("Item type = <event>");

        event_list = (event_list_t *)store_data->data;

        // generate the meta string :
        ASSERT_CODE_MESSAGE(MCL_OK == json_from_item_meta(event_list->meta, &store_data->meta), MCL_FAIL, "Get meta string from item meta for event has been failed!");

        if (MCL_OK != json_from_event_payload(event_list->events, &payload_string))
        {
            string_destroy(&store_data->meta);
            MCL_ERROR_RETURN(MCL_FAIL, "Get meta string from item meta for event has been failed!");
        }
        store_data->payload_buffer = (mcl_uint8_t *)payload_string->buffer;
        store_data->payload_size = payload_string->length;

        // only buffer of the string_t is used as the payload. string_t object needs to be freed. We don't use string_destroy because it also frees the buffer which we don't want.
        MCL_FREE(payload_string);
    }
    else if (STORE_DATA_FILE == store_data->type)
    {
		file_t *file;

        MCL_DEBUG("Item type = <file>");

        file = (file_t *)store_data->data;

        // generate the meta string :
        ASSERT_CODE_MESSAGE(MCL_OK == json_from_item_meta(&file->meta, &store_data->meta), MCL_FAIL, "Get meta string from item meta for file has been failed!");

        // get file size.
        store_data->payload_size = file->payload.size;
    }
    else if (STORE_DATA_CUSTOM == store_data->type)
    {
		custom_data_t *custom_data;

        MCL_DEBUG("Item type = <custom_data>");

        custom_data = (custom_data_t *)store_data->data;

        // generate the meta string :
        ASSERT_CODE_MESSAGE(MCL_OK == json_from_item_meta(&custom_data->meta, &store_data->meta), MCL_FAIL, "Get meta string from item meta for custom_data has been failed!");

        // custom data's payload will be its payload without any conversion :
        store_data->payload_buffer = custom_data->payload.buffer;
        store_data->payload_size = custom_data->payload.size;
    }
    else if (STORE_DATA_STREAM == store_data->type)
    {
		stream_data_t *stream_data;

        MCL_DEBUG("Item type = <stream_data>");

        stream_data = (stream_data_t *)store_data->data;

        // generate the meta string :
        ASSERT_CODE_MESSAGE(MCL_OK == json_from_item_meta(&stream_data->base->meta, &store_data->meta), MCL_FAIL,
                            "Get meta string from item meta for custom_data has been failed!");

        // its payload will be read from the callback.
    }
    else if (STORE_DATA_DATA_SOURCE_CONFIGURATION == store_data->type)
    {
		data_source_configuration_t *data_source_configuration;
		string_t *payload_string = MCL_NULL;

        MCL_DEBUG("Item type = <data_source_configuration>");

        data_source_configuration = (data_source_configuration_t *)store_data->data;

        // generate the meta string :
        ASSERT_CODE_MESSAGE(MCL_OK == json_from_item_meta(&data_source_configuration->meta, &store_data->meta), MCL_FAIL, "Get meta string from item meta for data source configuration has been failed!");

        if (MCL_OK != json_from_data_source_configuration_payload(&data_source_configuration->payload, &payload_string))
        {
            string_destroy(&store_data->meta);
            MCL_ERROR_RETURN(MCL_FAIL, "Get meta string from item meta for data source configuration has been failed!");
        }
        store_data->payload_buffer = (mcl_uint8_t *)payload_string->buffer;
        store_data->payload_size = payload_string->length;

        // only buffer of the string_t is used as the payload. string_t object needs to be freed. We don't use string_destroy because it also frees the buffer which we don't want.
        MCL_FREE(payload_string);
    }

    store_data_set_state(store_data, DATA_STATE_PREPARED);

    DEBUG_LEAVE("retVal = <%d>", MCL_OK);
    return MCL_OK;
}

E_MCL_ERROR_CODE _exchange_fill_http_request(http_processor_t *http_processor, store_t *store, http_request_t *request)
{
    DEBUG_ENTRY("http_processor_t *http_processor = <%p>, store_t *store = <%p>, http_request_t *request = <%p>", http_processor, store, request)

    // With a main loop, function will try to add one item from the lists in store in every iteration.
    //
    // Loop will continue until no more suitable item left in the list.
    //
    // If current item in list couldn't be added because of no more space left in the request,
    // we continue trying to add next items in the list, since they might be added. And we iterate the current item which couldn't be added
    // to its next item in order to be able to try next item in the next iteration of the main loop.
    //
    // When getting the next item in the list, we are not using list_next() because it resets its current pointer
    // to the list_head if current points to the last element. This will cause same items to come up again
    // and cause infinite loops here(since current_item will never be NULL).
    //
    // We try to add an item to the request ( write it to the request ) only if that item isn't added before.
    // If add ( write ) operation is successful, _exchange_add_current_data_to_request() will set its write counter.
    //
    // Every item must be tried only once for adding to the current http_request
    // If they couldn't be added to the request, at some point current_item will point to the NULL which means
    // every item in the list is tried.
    //
    // In one iteration, if none of the items in the list is able to fit in the current request ( added_any == MCL_FALSE ), we break the loop.
    // There is no point to try any further in this case.
    //

    store_data_t *current_store_data = MCL_NULL;

    // Initialize with store empty code; if there won't be any suitable data found in store ( no data in Prepared ) or the store is actually empty,
    // this function will return MCL_STORE_IS_EMPTY.

    E_MCL_ERROR_CODE add_result;
    mcl_bool_t added_any = MCL_FALSE;
    mcl_bool_t added_any_at_all = MCL_FALSE;

    // TODO : Currently using only the high priority list
    list_t *current_list = store->high_priority_list;

    // Set current node to list's current node:
    list_node_t *current_list_node = current_list->current;

    MCL_DEBUG("Loop starting");
    do
    {
        if (MCL_NULL != current_list_node)
        {
			E_STORE_DATA_STATE state;

            current_store_data = (store_data_t *)current_list_node->data;

            // Prepare data ( Generate meta/payload strings ) if it is not already prepared.
            if (DATA_STATE_INITIAL == store_data_get_state(current_store_data))
            {
                ASSERT_CODE_MESSAGE(MCL_OK == _exchange_prepare_data(current_store_data), MCL_FAIL, "Generation of meta/payload buffers has been failed!");
            }

            // If data is not written already ( state == DATA_STATE_PREPARED ), try to add it :
            state = store_data_get_state(current_store_data);
            if ((DATA_STATE_PREPARED == state) || (DATA_STATE_STREAMING == state))
            {
                if (DATA_STATE_PREPARED == state)
                {
                    add_result = _exchange_add_current_data_to_request(current_store_data, request);
                }
                else if (DATA_STATE_STREAMING == state)
                {
                    add_result = _exchange_add_current_data_to_request_by_streaming(current_store_data, request);
                }

                // check result
                if ((MCL_OK == add_result) || (MCL_PARTIALLY_WRITTEN == add_result))
                {
                    MCL_DEBUG("Current data has been added to the http request successfully. We continue to add for other items in the list.");
                    added_any = MCL_TRUE;
                    added_any_at_all = MCL_TRUE;
                }
                else if (MCL_HTTP_REQUEST_NO_MORE_SPACE == add_result)
                {
                    MCL_DEBUG("There is not enough space left in the http request for current data.");

                    // Check if we can stream this one. Or if it is already, continue.
                    // If streaming is currently active; return right away to continue adding the same data w/o interrupting.
                    // If not active; check if streaming is enabled and start if it is.
                    if (DATA_STATE_STREAMING == store_data_get_state(current_store_data))
                    {
                        MCL_DEBUG("Streaming is already active for current data. Returning right away to write the rest.");

                        DEBUG_LEAVE("retVal = <%d>", MCL_EXCHANGE_STREAMING_IS_ACTIVE);
                        return MCL_EXCHANGE_STREAMING_IS_ACTIVE;
                    }
                    else if (MCL_TRUE == store->streamable)
                    {
                        MCL_DEBUG("Streaming is enabled. Streaming will be tried for this one.");
                        store_data_set_state(current_store_data, DATA_STATE_STREAMING);

                        // Because of this state, in the next iteration this data will be redirected to
                        // _exchange_add_current_data_to_request_by_streaming() above.
                        // ( current node won't be iterated to its next one. note the below check. )

                    }
                
                    // else : continue normal processing. No need for #else block.
                }
                else
                {
                    MCL_DEBUG("Error response received from http request add operation. Aborting the operation!");
                    break;
                }
            }
            else
            {
                MCL_DEBUG("Current data in the store is not in PREPARED state. Either it is not ready or it is already written. Skipping..");
            }

            // Iterate the list only if the current state of the data is not DATA_STATE_STREAMING
            // If it is, we need to stay on it until whole data is written
            if (DATA_STATE_STREAMING != store_data_get_state(current_store_data))
            {
                current_list_node = current_list_node->next;
            }
            else
            {
                MCL_DEBUG("Since streaming is active for current data, not iterating to the next store data.");
            }
        }
        else
        {
            MCL_DEBUG("End of list reached in the list.");

            // reached list end. If we didn't add anything since the last trip on the list; it means there is no data left in the list to add
            if (MCL_FALSE == added_any)
            {
                // In this iteration we tried till the NULL node and not any data could be added to the request
                // There is either no data left in the list or the no data is not suitable for the current
                // http request ( there must be not enough space left ). We can return now, no point in trying again :
                MCL_DEBUG("No item has been added to the request in this iteration. There is no suitable data in the store for the current request. Not going to try again.");
                break;
            }
            else
            {
                MCL_DEBUG("Some data has been added to the list in this iteration. We will try again from the beginning of the list. We might fit some more data.");

                // reset add flag :
                added_any = MCL_FALSE;

                // reset the current_list_node to the beginning of the list:
                current_list_node = current_list->head;
            }
        }
    }
    while (MCL_TRUE);

    MCL_DEBUG("Loop has ended.");

    if (MCL_FALSE == added_any_at_all)
    {
        MCL_DEBUG("Not any of the data in the store could be added to the request. There is no suitable data in it.");
        add_result = MCL_STORE_ITEM_EXCEEDS_MAX_HTTP_REQUEST_SIZE;
    }
    else
    {
        MCL_DEBUG("Http request is ready to send. Finalizing..");
        add_result = _exchange_finalize_http_request(http_processor, request, MCL_FALSE);
    }

    DEBUG_LEAVE("retVal = <%d>", add_result);
    return add_result;
}

static E_MCL_ERROR_CODE _exchange_evaluate_response(store_t *store, E_MCL_ERROR_CODE send_result, http_response_t *response, void **reserved, string_t *correlation_id)
{
    DEBUG_ENTRY("store_t *store = <%p>, E_MCL_ERROR_CODE send_result = <%d>, http_response_t *response = <%p>, void **reserved = <%p>, string_t *correlation_id = <%p>", store, send_result, response, reserved, correlation_id)

    E_MCL_ERROR_CODE result = MCL_FAIL;

    if (MCL_OK == send_result)
    {
        if (MCL_NULL != response)
        {
            if (MCL_HTTP_RESULT_CODE_SUCCESS == response->result_code)
            {
                MCL_INFO("Send operation is successful. 200 OK Has been received from server. Correlation-ID = \"%s\"", correlation_id->buffer);
                result = MCL_OK;   
            }
            else
            {
                // server did not respond OK.
                MCL_ERROR("Send operation is successful. ERROR code has been received from server = <%d>. Correlation-ID = \"%s\"", response->result_code, correlation_id->buffer);
                result = _evaluate_response_codes(response);
            }

            // TODO : Remove destroying the response if evaluation needs it to be used somewhere else in the future:
            // response is not necessary currently.
            http_response_destroy(&response);
        }
        else
        {
            MCL_DEBUG("HTTP response object is NULL. Means the send operation is failed!");
        }
    }
    else
    {
        MCL_DEBUG("Send result is not successful.");
        result = send_result;
    }

    // Update the store data states according to the result of send operation :
    if (MCL_OK == result)
    {
        _exchange_update_store_state(store, MCL_TRUE);
        _exchange_clear_sent_data_from_store(store);
    }
    else
    {
        _exchange_update_store_state(store, MCL_FALSE);
    }

    DEBUG_LEAVE("retVal = <%d>", result);
    return result;
}

static E_MCL_ERROR_CODE _exchange_update_store_data_state(store_data_t *store_data, mcl_bool_t send_operation_successful)
{
    DEBUG_ENTRY("store_data_t *store_data = <%p>, mcl_bool_t send_operation_successful = <%u>", store_data, send_operation_successful)

    E_STORE_DATA_STATE state = store_data_get_state(store_data);

    if (DATA_STATE_WRITTEN == state)
    {
        if (MCL_TRUE == send_operation_successful)
        {
            // data has been sent. go to next state :
            store_data_set_state(store_data, DATA_STATE_SENT);
        }
        else
        {
            // data couldn't be sent. go to previous state. do necessary arrangements :
            store_data_set_state(store_data, DATA_STATE_PREPARED);
        }
    }
    else
    {
        // no need to update the state
        MCL_DEBUG("State is %d, no need to update current items state", state);
    }

    DEBUG_LEAVE("retVal = <%d>", MCL_OK);
    return MCL_OK;
}

static E_MCL_ERROR_CODE _exchange_update_store_state(store_t *store, mcl_bool_t send_operation_successful)
{
    DEBUG_ENTRY("store_t *store = <%p>, mcl_bool_t send_operation_successful = <%u>", store, send_operation_successful)

    list_node_t *current_node = MCL_NULL;

    list_reset(store->high_priority_list);
    list_reset(store->low_priority_list);

    // high_priority_list
    while (MCL_NULL != (current_node = list_next(store->high_priority_list)))
    {
        _exchange_update_store_data_state((store_data_t *)current_node->data, send_operation_successful);
    }

    // low_priority_list
    while (MCL_NULL != (current_node = list_next(store->low_priority_list)))
    {
        _exchange_update_store_data_state((store_data_t *)current_node->data, send_operation_successful);
    }

    DEBUG_LEAVE("retVal = <%d>", MCL_OK);
    return MCL_OK;
}

static E_MCL_ERROR_CODE _exchange_clear_sent_data_from_store(store_t *store)
{
    DEBUG_ENTRY("store_t *store = <%p>", store)

    // in every store_data item, remove the item if its state is sent
    list_node_t *current_node = MCL_NULL;

    // high priority list
    while (MCL_NULL != (current_node = list_next(store->high_priority_list)))
    {
        if (DATA_STATE_SENT == store_data_get_state((store_data_t *)current_node->data))
        {
            store_data_remove(store->high_priority_list, current_node);
        }
    }

    // low priority list
    while (MCL_NULL != (current_node = list_next(store->low_priority_list)))
    {
        if (DATA_STATE_SENT == store_data_get_state((store_data_t *)current_node->data))
        {
            store_data_remove(store->low_priority_list, current_node);
        }
    }

    DEBUG_LEAVE("retVal = <%d>", MCL_OK);
    return MCL_OK;
}

static E_MCL_ERROR_CODE _exchange_store_data_get_content_info(store_data_t *store_data, string_t **meta_content_type, string_t **meta_content_id,
        string_t **payload_content_type)
{
    DEBUG_ENTRY("store_data_t *store_data = <%p>, string_t *meta_content_type = <%p>, string_t *meta_content_id = <%p>, string_t *payload_content_type = <%p>", store_data,
                meta_content_type, meta_content_id, payload_content_type)

    E_MCL_ERROR_CODE result = MCL_OK;

    // generate the meta/payload string based on the type:
    if (STORE_DATA_TIME_SERIES == store_data->type)
    {
		time_series_t *time_series;

        MCL_DEBUG("Item type = <time_series>");
        *meta_content_type = &content_type_values[CONTENT_TYPE_META_JSON];
        *payload_content_type = &content_type_values[CONTENT_TYPE_APPLICATION_JSON];

        time_series = (time_series_t *)store_data->data;
        *meta_content_id = time_series->meta.content_id;
    }
    else if (STORE_DATA_EVENT_LIST == store_data->type)
    {
		event_list_t *event_list;

        MCL_DEBUG("Item type = <event>");
        *meta_content_type = &content_type_values[CONTENT_TYPE_META_JSON];
        *payload_content_type = &content_type_values[CONTENT_TYPE_APPLICATION_JSON];

        event_list = (event_list_t *)store_data->data;
        *meta_content_id = event_list->meta->content_id;
    }
    else if (STORE_DATA_FILE == store_data->type)
    {
		file_t *file;
        MCL_DEBUG("Item type = <file>");
        *meta_content_type = &content_type_values[CONTENT_TYPE_META_JSON];
        *payload_content_type = &content_type_values[CONTENT_TYPE_APPLICATION_OCTET_STREAM];

        file = (file_t *)store_data->data;
        *meta_content_id = file->meta.content_id;
    }
    else if (STORE_DATA_CUSTOM == store_data->type)
    {
		custom_data_t *custom_data;
        MCL_DEBUG("Item type = <custom_data>");
        *meta_content_type = &content_type_values[CONTENT_TYPE_META_JSON];
        *payload_content_type = &content_type_values[CONTENT_TYPE_APPLICATION_OCTET_STREAM];

        custom_data = (custom_data_t *)store_data->data;
        *meta_content_id = custom_data->meta.content_id;

    }
    else if (STORE_DATA_STREAM == store_data->type)
    {
		custom_data_t *custom_data;

        MCL_DEBUG("Item type = <stream_data>");
        *meta_content_type = &content_type_values[CONTENT_TYPE_META_JSON];
        *payload_content_type = &content_type_values[CONTENT_TYPE_APPLICATION_OCTET_STREAM];

        custom_data = ((stream_data_t *)store_data->data)->base;
        *meta_content_id = custom_data->meta.content_id;
    }
    else if (STORE_DATA_DATA_SOURCE_CONFIGURATION == store_data->type)
    {
		data_source_configuration_t *data_source_configuration;

        MCL_DEBUG("Item type = <data_source_configuration>");
        *meta_content_type = &content_type_values[CONTENT_TYPE_META_JSON];
        *payload_content_type = &content_type_values[CONTENT_TYPE_APPLICATION_JSON];

        data_source_configuration = (data_source_configuration_t *)store_data->data;
        *meta_content_id = data_source_configuration->meta.content_id;
    }
    else
    {
        MCL_DEBUG("Item type = <UNKNOWN(%d)>", store_data->type);
        result = MCL_FAIL;

        *meta_content_type = MCL_NULL;
        *meta_content_id = MCL_NULL;
        *payload_content_type = MCL_NULL;
    }

    DEBUG_LEAVE("retVal = <%d>", result);
    return result;
}

static E_MCL_ERROR_CODE _exchange_initialize_http_request_headers(http_processor_t *http_processor, http_request_t *request, mcl_bool_t with_authentication)
{
    DEBUG_ENTRY("http_request_t *request = <%p>", request)

    // Add these headers :
    // 1- Content-type
    // 2- If with_authentication is true : Authentication header
    // 2- Else; Authentication header will be included at the end

    E_MCL_ERROR_CODE result = http_request_add_header(request, &http_header_names[HTTP_HEADER_CONTENT_TYPE], &content_type_values[CONTENT_TYPE_MULTIPART_MIXED]);
    ASSERT_CODE_MESSAGE(MCL_OK == result, result, "Add header to request has been failed for <Content-Type>.");

    result = http_request_add_header(request, &http_header_names[HTTP_HEADER_ACCEPT], &content_type_values[CONTENT_TYPE_APPLICATION_JSON]);
    ASSERT_CODE_MESSAGE(MCL_OK == result, result, "Add header to request has been failed for <Accept>.");

    ASSERT_CODE(MCL_OK == _add_authentication_header_to_request(http_processor, request, with_authentication), MCL_HTTP_REQUEST_FINALIZE_FAILED);

    DEBUG_LEAVE("retVal = <%d>", result);
    return result;
}

static E_MCL_ERROR_CODE _exchange_finalize_http_request(http_processor_t *http_processor, http_request_t *request, mcl_bool_t with_authentication)
{
    DEBUG_ENTRY("http_processor_t *http_processor = <%p>, http_request_t *request = <%p>", http_processor, request)

    // finalize the http_request
    E_MCL_ERROR_CODE result = http_request_finalize(request);
    ASSERT_CODE_MESSAGE(MCL_OK == result, result, "http request finalize is failed!");

    ASSERT_CODE(MCL_OK == _add_authentication_header_to_request(http_processor, request, with_authentication), MCL_HTTP_REQUEST_FINALIZE_FAILED);

    DEBUG_LEAVE("retVal = <%d>", MCL_OK);
    return MCL_OK;
}

static E_MCL_ERROR_CODE _add_authentication_header_to_request(http_processor_t *http_processor, http_request_t *request, mcl_bool_t with_authentication)
{
    if (MCL_TRUE == with_authentication)
    {
        // Add "Bearer" in front of JWT string.
        string_t *bearer = MCL_NULL;
		string_t *jwt_string = MCL_NULL;
        E_MCL_ERROR_CODE code = string_initialize_static("Bearer ", 0, &bearer);
        ASSERT_CODE_MESSAGE(MCL_OK == code, code, "Bearer string can not be created.");

        code = string_concatenate(bearer, http_processor->security_handler->access_token, &jwt_string);
        string_destroy(&bearer);
        ASSERT_CODE_MESSAGE(MCL_OK == code, code, "Bearer string can not be added to JWT string.");

        code = http_request_add_header(request, &http_header_names[HTTP_HEADER_AUTHORIZATION], jwt_string);
        string_destroy(&jwt_string);
        ASSERT_CODE_MESSAGE(MCL_OK == code, MCL_HTTP_REQUEST_FINALIZE_FAILED, "Add header to request has been failed for <Authorization>.");
    }

    return MCL_OK;
}

static mcl_size_t _get_payload_from_buffer(void *destination, void *source, mcl_size_t size, void *user_context)
{
    DEBUG_ENTRY("void *destination = <%p>, void *source = <%p>, mcl_size_t size = <%u>, void *user_context = <%p>", destination, source, size, user_context)

    string_util_memcpy(destination, source, size);

    DEBUG_LEAVE("retVal = <%u>", size);
    return size;
}

static mcl_size_t _get_payload_from_file(void *destination, void *file_descriptor, mcl_size_t size, void *user_context)
{
    DEBUG_ENTRY("void *destination = <%p>, void *file_descriptor = <%p>, mcl_size_t size = <%u>, void *user_context = <%p>", destination, file_descriptor, size, user_context)

    mcl_size_t actual_size_read = 0;
    file_util_fread(destination, 1, size, file_descriptor, &actual_size_read);

    DEBUG_LEAVE("retVal = <%u>", actual_size_read);
    return actual_size_read;
}

#if MCL_STREAM_ENABLED
mcl_size_t _stream_callback(void *buffer, mcl_size_t size, mcl_size_t count, void *user_context)
{
    DEBUG_ENTRY("void *buffer = <%p>, mcl_size_t size = <%u>, mcl_size_t count = <%u>, void *user_context = <%p>", buffer, size, count, user_context)

    // get http_request and store data from user context:
    http_processor_stream_callback_context_t *http_processor_callback_context = (http_processor_stream_callback_context_t *)user_context;
    http_processor_t *http_processor = http_processor_callback_context->http_processor;
    http_request_t *request = http_processor_callback_context->request;
    store_t *store = http_processor_callback_context->store;
    E_MCL_ERROR_CODE previous_result = http_processor_callback_context->previous_result;

    mcl_size_t written = 0;

    // Check if max call count is reached. Do not terminate if we are in the middle of something even if tha max call count is reached.
    if ((MAX_STREAM_CALLBACK_CALL_COUNT > http_processor_callback_context->call_count) || (MCL_EXCHANGE_STREAMING_IS_ACTIVE == previous_result)
            || (MCL_HTTP_REQUEST_FINALIZE_FAILED == previous_result))
    {
        request->payload = buffer;
        request->payload_size = size * count;
        request->payload_offset = 0;

        E_MCL_ERROR_CODE result;

        // If only finalize is failed last time, lets try it again:
        if (MCL_HTTP_REQUEST_FINALIZE_FAILED == previous_result)
        {
            result = _exchange_finalize_http_request(http_processor, request, MCL_FALSE);

            // if it is failed again, just give up
            if (MCL_OK != result)
            {
                MCL_DEBUG("Second try of http_request_finalize is failed again! Terminating the operation.");
                result = MCL_FAIL;
            }
        }
        else
        {
            result = _exchange_fill_http_request(http_processor, store, request);
        }

        // Check the result :
        if ((MCL_OK == result) || (MCL_HTTP_REQUEST_FINALIZE_FAILED == result) || (MCL_STORE_IS_EMPTY == result) || (MCL_EXCHANGE_STREAMING_IS_ACTIVE == result) || (MCL_STORE_ITEM_EXCEEDS_MAX_HTTP_REQUEST_SIZE == result))
        {
            // If MCL_HTTP_REQUEST_FINALIZE_FAILED, let's return the current written amount and try for finalize in the next call.

            // If MCL_STORE_IS_EMPTY, payload_offset must be 0, just return it to end the send operation.

            // If MCL_EXCHANGE_STREAMING_IS_ACTIVE: We are still in the middle of something. Just continue ..

            // if this will be 0 callback will be terminated. Not checking.
            written = request->payload_offset;
        }
        else
        {
            MCL_DEBUG("Fill http request operation is failed. Terminating send operation.");

            // Get actual terminating error code from http_client.
            written = http_client_get_callback_termination_code();
        }

        // save result in order to be used in next call ( if there will be ):
        MCL_DEBUG("Fill operation result will be saved to be used in next call = <%u>.", result);
        http_processor_callback_context->previous_result = result;

        // reset the request :
        request->payload = MCL_NULL;
        request->payload_size = 0;
        request->payload_offset = 0;
    }
    else
    {
        MCL_DEBUG("Max callback call limit has been reached. Call count = <%d>. Terminating the send operation.", http_processor_callback_context->call_count);
        written = 0;
    }

    http_processor_callback_context->call_count++;

    DEBUG_LEAVE("retVal = <%u>", written);
    return written;
}
#endif

E_MCL_ERROR_CODE _process_registration_response_shared_secret(http_processor_t *http_processor, http_response_t *http_response)
{
    DEBUG_ENTRY("http_processor_t *http_processor = <%p>, http_response_t *http_response = <%p>", http_processor, http_response)

    json_t *json_root = MCL_NULL;
	json_t *client_id = MCL_NULL;
	json_t *client_secret = MCL_NULL;
	json_t *registration_access_token = MCL_NULL;
	json_t *registration_client_uri = MCL_NULL;
	mcl_bool_t ok;

    E_MCL_ERROR_CODE code = json_util_parse((char *)http_response_get_payload(http_response), &json_root);
    ASSERT_CODE_MESSAGE(MCL_OK == code, code, "parsing payload of http_response failed.");

    ok = MCL_OK == (code = json_util_get_object_item(json_root, JSON_NAME_CLIENT_ID, &client_id));
    ok = ok && (MCL_OK == (code = json_util_get_object_item(json_root, JSON_NAME_CLIENT_SECRET, &client_secret)));
    ok = ok && (MCL_OK == (code = json_util_get_object_item(json_root, JSON_NAME_REGISTRATION_ACCESS_TOKEN, &registration_access_token)));
    ok = ok && (MCL_OK == (code = json_util_get_object_item(json_root, JSON_NAME_REGISTRATION_CLIENT_URI, &registration_client_uri)));

    if (ok)
    {
        string_destroy(&http_processor->security_handler->client_id);
        string_destroy(&http_processor->security_handler->client_secret);
        string_destroy(&http_processor->security_handler->registration_access_token);
        string_destroy(&http_processor->security_handler->registration_client_uri);

        ok = ok && (MCL_OK == (code = json_util_get_string(client_id, &http_processor->security_handler->client_id)));
        ok = ok && (MCL_OK == (code = json_util_get_string(client_secret, &http_processor->security_handler->client_secret)));
        ok = ok && (MCL_OK == (code = json_util_get_string(registration_access_token, &http_processor->security_handler->registration_access_token)));
        ok = ok && (MCL_OK == (code = json_util_get_string(registration_client_uri, &http_processor->security_handler->registration_client_uri)));

        if (!ok)
        {
            string_destroy(&http_processor->security_handler->client_id);
            string_destroy(&http_processor->security_handler->client_secret);
            string_destroy(&http_processor->security_handler->registration_access_token);
            string_destroy(&http_processor->security_handler->registration_client_uri);
        }
    }

    // Child json objects are reference to their parts in root json. So we only free the struct.
    MCL_FREE(client_id);
    MCL_FREE(client_secret);
    MCL_FREE(registration_access_token);
    MCL_FREE(registration_client_uri);
    json_util_destroy(&json_root);

    DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

E_MCL_ERROR_CODE _process_registration_response_rsa_3072(http_processor_t *http_processor, http_response_t *http_response)
{
    DEBUG_ENTRY("http_processor_t *http_processor = <%p>, http_response_t *http_response = <%p>", http_processor, http_response)

    json_t *json_root = MCL_NULL;
	json_t *client_id = MCL_NULL;
	json_t *registration_access_token = MCL_NULL;
	json_t *registration_client_uri = MCL_NULL;
	mcl_bool_t ok;

    E_MCL_ERROR_CODE code = json_util_parse((char *)http_response_get_payload(http_response), &json_root);
    ASSERT_CODE_MESSAGE(MCL_OK == code, code, "parsing payload of http_response failed.");

    ok = MCL_OK == (code = json_util_get_object_item(json_root, JSON_NAME_CLIENT_ID, &client_id));
    ok = ok && (MCL_OK == (code = json_util_get_object_item(json_root, JSON_NAME_REGISTRATION_ACCESS_TOKEN, &registration_access_token)));
    ok = ok && (MCL_OK == (code = json_util_get_object_item(json_root, JSON_NAME_REGISTRATION_CLIENT_URI, &registration_client_uri)));

    if (ok)
    {
        string_destroy(&http_processor->security_handler->client_id);
        string_destroy(&http_processor->security_handler->registration_access_token);
        string_destroy(&http_processor->security_handler->registration_client_uri);

        ok = ok && (MCL_OK == (code = json_util_get_string(client_id, &http_processor->security_handler->client_id)));
        ok = ok && (MCL_OK == (code = json_util_get_string(registration_access_token, &http_processor->security_handler->registration_access_token)));
        ok = ok && (MCL_OK == (code = json_util_get_string(registration_client_uri, &http_processor->security_handler->registration_client_uri)));

        if (!ok)
        {
            string_destroy(&http_processor->security_handler->client_id);
            string_destroy(&http_processor->security_handler->registration_access_token);
            string_destroy(&http_processor->security_handler->registration_client_uri);
        }
    }

    // Child json objects are reference to their parts in root json. So we only free the struct.
    MCL_FREE(client_id);
    MCL_FREE(registration_access_token);
    MCL_FREE(registration_client_uri);
    json_util_destroy(&json_root);

    DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

#if MCL_FILE_DOWNLOAD_ENABLED
static E_MCL_ERROR_CODE _get_file_name(char *header, string_t **file_name)
{
	DEBUG_ENTRY("char *header = <%p>, char **file_name = <%p>", header, file_name)

	/*
	 File name is present in header "Content-Disposition" in following format.
	 
			Content-Disposition: attachment; filename="<filename>"

	We try to find two quotes after "filename" keyword and then the file name in between.
	*/

	char *file_name_keyword = "filename";
	mcl_size_t file_name_keyword_length = 9;

	mcl_size_t file_name_index;
	mcl_bool_t file_name_found = string_util_find(header, file_name_keyword, &file_name_index);
	ASSERT_CODE_MESSAGE(MCL_TRUE == file_name_found, MCL_FAIL, "Could not find filename keyword in header.");

	// Add length of the keyword to it's start index.
	file_name_index += file_name_keyword_length;

	// Find first quote
	mcl_size_t first_quote_index;
	mcl_bool_t first_quote_found = string_util_find(header + file_name_index, "\"", &first_quote_index);
	ASSERT_CODE_MESSAGE(MCL_TRUE == first_quote_found, MCL_FAIL, "Could not find second header in header.");

	first_quote_index += file_name_index;

	mcl_size_t second_quote_index;
	mcl_bool_t second_quote_found = string_util_find(header + first_quote_index + 1, "\"", &second_quote_index);
	ASSERT_CODE_MESSAGE(MCL_TRUE == second_quote_found, MCL_FAIL, "Could not find second header in header.");

 	mcl_size_t file_name_size = (second_quote_index + 1);
	char *file_name_local = MCL_MALLOC(file_name_size + 1);

	ASSERT_CODE(MCL_NULL != file_name_local, MCL_OUT_OF_MEMORY);

	string_util_strncpy(file_name_local, header + first_quote_index + 1, file_name_size);
	file_name_local[file_name_size] = MCL_NULL_CHAR;

	string_initialize_dynamic(file_name_local, 0, file_name);

	DEBUG_LEAVE("retVal = <%d>", MCL_OK);
	return MCL_OK;
}
#endif

#if MCL_FILE_DOWNLOAD_ENABLED
static E_MCL_ERROR_CODE _add_range_header(http_request_t *request, mcl_size_t start_byte, mcl_size_t end_byte)
{
	DEBUG_ENTRY("http_request_t *request = <%p>, mcl_size_t start_byte = <%u>, mcl_size_t end_byte = <%u>", request, start_byte, end_byte)

	char temp_header_value[MAX_RANGE_HEADER_LENGTH] = { 0 };

	E_MCL_ERROR_CODE result = string_util_snprintf(temp_header_value, MAX_RANGE_HEADER_LENGTH, "bytes=%u-%u", start_byte, end_byte);
	ASSERT_CODE_MESSAGE(MCL_OK == result, result, "string_util_snprintf() failed for start_byte <%u> and end_byte <%u>", start_byte, end_byte);

	string_t *range_header_value = MCL_NULL;
	result = string_initialize_new(temp_header_value, 0, &range_header_value);
	ASSERT_CODE_MESSAGE(MCL_OK == result, result, "Memory cannot be allocated for Range header value.");

	result = http_request_add_header(request, &http_header_names[HTTP_HEADER_RANGE], range_header_value);
	string_destroy(&range_header_value);
	ASSERT_CODE_MESSAGE(MCL_OK == result, result, "Could not add Range header to request.");

	DEBUG_LEAVE("retVal = <%d>", MCL_OK);
	return MCL_OK;
}

static E_MCL_ERROR_CODE _add_actual_range_to_file(http_response_t *response, file_t *file)
{
	DEBUG_ENTRY("http_response_t *response = <%p>,  mcl_file_t *file = <%p>", response, file)

	string_t *content_range_header = MCL_NULL;
	E_MCL_ERROR_CODE result = http_response_get_header(response, http_header_names[HTTP_HEADER_CONTENT_RANGE].buffer, &content_range_header);
	ASSERT_CODE_MESSAGE(MCL_OK == result, result, "Could not get header.");

	// Find first integral value
	char *rest = MCL_NULL;
	char *rest2 = MCL_NULL;

	char *string = content_range_header->buffer;

	// Skip until the current character is a digit (i.e. it's value is between ASCII value of 0 and 9 inclusive)
	while (!((47 < *string) && (58 > *string)))
	{
		// If null termination is reached then break
		if (0 == *string)
		{
			break;
		}
		++string;
	}

	mcl_size_t start_byte = string_util_strtol(string, 10, &rest);
	ASSERT_STATEMENT_CODE_MESSAGE(MCL_NULL != rest, string_destroy(&content_range_header), MCL_FAIL, "string_util_strtol() failed for %s", string);

	while (!((47 < *rest) && (58 > *rest)))
	{
		if (0 == *rest)
		{
			break;
		}
		++rest;
	}
	mcl_size_t end_byte = string_util_strtol(rest, 10, &rest2);
	string_destroy(&content_range_header);
	ASSERT_CODE_MESSAGE((MCL_NULL != rest2) && (rest2 != rest), MCL_FAIL, "string_util_strtol() failed");

	file->payload.start_byte = start_byte;
	file->payload.end_byte = end_byte;

	DEBUG_LEAVE("retVal = <%d>", MCL_OK);
	return MCL_OK;
}
#endif

static E_MCL_ERROR_CODE _compose_rsa_onboarding_json(security_handler_t * security_handler, string_t **payload)
{
    DEBUG_ENTRY("security_handler_t *security_handler = <%p>, string_t **payload = <%p>", security_handler, payload)

	json_t *root = MCL_NULL;
	json_t *jwks = MCL_NULL;
	json_t *keys = MCL_NULL;
	json_t *key_1 = MCL_NULL;

	char *modulus = MCL_NULL;
	char *public_exponent = MCL_NULL;
	char *payload_string = MCL_NULL;

	E_MCL_ERROR_CODE code = security_rsa_get_modulus_and_exponent(security_handler->rsa.public_key, &modulus, &public_exponent);

	(MCL_OK == code) && (code = json_util_initialize(JSON_OBJECT, &root));
	(MCL_OK == code) && (code = json_util_start_object(root, JSON_NAME_JWKS, &jwks));
	(MCL_OK == code) && (code = json_util_start_array(jwks, JSON_NAME_KEYS, &keys));
	(MCL_OK == code) && (code = _add_key_to_keys_array(keys, &key_1));
	(MCL_OK == code) && (code = json_util_add_string(key_1, JSON_NAME_E, public_exponent));
	(MCL_OK == code) && (code = json_util_add_string(key_1, JSON_NAME_N, modulus));
	(MCL_OK == code) && (code = json_util_add_string(key_1, JSON_NAME_KTY, "RSA"));
	(MCL_OK == code) && (code = json_util_add_string(key_1, JSON_NAME_KID, "key-1"));
	(MCL_OK == code) && (code = json_util_to_string(root, &payload_string));
	(MCL_OK == code) && (code = string_initialize_dynamic(payload_string, 0, payload));

	if (MCL_OK != code)
	{
		MCL_FREE(payload_string);
	}

	MCL_FREE(modulus);
	MCL_FREE(public_exponent);
	json_util_finish_object(&key_1);
	json_util_finish_object(&keys);
	json_util_finish_object(&jwks);
	json_util_destroy(&root);

	DEBUG_LEAVE("retVal = <%d>", code);
	return code;
}

static E_MCL_ERROR_CODE _compose_rsa_key_rotation_json(security_handler_t * security_handler, string_t **payload)
{
	DEBUG_ENTRY("security_handler_t *security_handler = <%p>, string_t **payload = <%p>", security_handler, payload)

	E_MCL_ERROR_CODE code;
    json_t *root = MCL_NULL;
    json_t *jwks = MCL_NULL;
    json_t *keys = MCL_NULL;
    json_t *key_1 = MCL_NULL;

    char *modulus = MCL_NULL;
    char *public_exponent = MCL_NULL;
    char *payload_string = MCL_NULL;

    MCL_FREE(security_handler->rsa.public_key);
    MCL_FREE(security_handler->rsa.private_key);
    code = security_handler_generate_rsa_key(security_handler);

    (MCL_OK == code) && (code = security_rsa_get_modulus_and_exponent(security_handler->rsa.public_key, &modulus, &public_exponent));
    (MCL_OK == code) && (code = json_util_initialize(JSON_OBJECT, &root));
    (MCL_OK == code) && (code = json_util_add_string(root, JSON_NAME_CLIENT_ID, security_handler->client_id->buffer));
    (MCL_OK == code) && (code = json_util_start_object(root, JSON_NAME_JWKS, &jwks));
    (MCL_OK == code) && (code = json_util_start_array(jwks, JSON_NAME_KEYS, &keys));
    (MCL_OK == code) && (code = _add_key_to_keys_array(keys, &key_1));
    (MCL_OK == code) && (code = json_util_add_string(key_1, JSON_NAME_E, public_exponent));
    (MCL_OK == code) && (code = json_util_add_string(key_1, JSON_NAME_N, modulus));
    (MCL_OK == code) && (code = json_util_add_string(key_1, JSON_NAME_KTY, "RSA"));
    (MCL_OK == code) && (code = json_util_add_string(key_1, JSON_NAME_KID, "key-1"));
    (MCL_OK == code) && (code = json_util_to_string(root, &payload_string));
    (MCL_OK == code) && (code = string_initialize_dynamic(payload_string, 0, payload));

    if (MCL_OK != code)
    {
        MCL_FREE(payload_string);
    }

    MCL_FREE(modulus);
    MCL_FREE(public_exponent);
    json_util_finish_object(&key_1);
    json_util_finish_object(&keys);
    json_util_finish_object(&jwks);
    json_util_destroy(&root);

    DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

static E_MCL_ERROR_CODE _add_key_to_keys_array(json_t *root, json_t **json_object)
{
	DEBUG_ENTRY("json_t *root = <%p>, json_t **json_object = <%p>", root, json_object)

	E_MCL_ERROR_CODE code = json_util_initialize(JSON_OBJECT, json_object);
	ASSERT_CODE_MESSAGE(MCL_OK == code, code, "json_object couldn't be allocated in memory.");

	json_util_add_item_to_array(root, *json_object);

	DEBUG_LEAVE("retVal = <%d>", code);
	return code;
}

static E_MCL_ERROR_CODE _check_file_path(const char *file_name)
{
    DEBUG_ENTRY("const char *file_name = <%p>", file_name)

    void *file_descriptor = MCL_NULL;
    E_MCL_ERROR_CODE code = file_util_fopen(file_name, "a", &file_descriptor);

    if (MCL_OK == code)
    {
        file_util_fclose(file_descriptor);
    }

    DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

static E_MCL_ERROR_CODE _custom_load_register_info(http_processor_t *http_processor)
{
	DEBUG_ENTRY("http_processor_t *http_processor = <%p>", http_processor)

	E_MCL_ERROR_CODE code;
	char *client_id;
	char *registration_access_token;
	char *registration_uri;

	if (MCL_SECURITY_SHARED_SECRET == http_processor->configuration->security_profile)
	{
		char *client_secret;
		code = http_processor->configuration->load_function.shared_secret(&client_id, &client_secret, &registration_access_token, &registration_uri);

		if(MCL_OK == code)
		{
		    string_initialize_dynamic(client_secret, 0, &(http_processor->security_handler->client_secret));
		}
    }
	else
	{
		code = http_processor->configuration->load_function.rsa(&client_id, &(http_processor->security_handler->rsa.public_key), &(http_processor->security_handler->rsa.private_key), &registration_access_token, &registration_uri);
	}

	if(MCL_OK == code)
	{
	    string_initialize_dynamic(client_id, 0, &(http_processor->security_handler->client_id));
	    string_initialize_dynamic(registration_access_token, 0, &(http_processor->security_handler->registration_access_token));
	    string_initialize_dynamic(registration_uri, 0, &(http_processor->security_handler->registration_client_uri));
	}

	DEBUG_LEAVE("retVal = <%d>", code);
	return code;
}

static E_MCL_ERROR_CODE _evaluate_response_codes(http_response_t *response)
{
    DEBUG_ENTRY("http_response_t *response = <%p>", response)

    E_MCL_ERROR_CODE code;
    switch (response->result_code)
    {
        case MCL_HTTP_RESULT_CODE_SUCCESS:
            code = MCL_OK;
            break;

        case MCL_HTTP_RESULT_CODE_CREATED:
            code = MCL_CREATED;
            break;

        case MCL_HTTP_RESULT_CODE_PARTIAL_CONTENT:
            code = MCL_PARTIAL_CONTENT;
            break;

        case MCL_HTTP_RESULT_CODE_BAD_REQUEST:
            code = MCL_BAD_REQUEST;
            break;

        case MCL_HTTP_RESULT_CODE_UNAUTHORIZED:
            code = MCL_UNAUTHORIZED;
            break;

        case MCL_HTTP_RESULT_CODE_FORBIDDEN:
            code = MCL_FORBIDDEN;
            break;

        case MCL_HTTP_RESULT_CODE_NOT_FOUND:
            code = MCL_NOT_FOUND;
            break;

        case MCL_HTTP_RESULT_CODE_INTERNAL_SERVER_ERR:
            code = MCL_SERVER_FAIL;
            break;

        case MCL_HTTP_RESULT_CODE_CONFLICT:
            code = MCL_CONFLICT;
            break;

        case MCL_HTTP_RESULT_CODE_PRECONDITION_FAILED:
            code = MCL_PRECONDITION_FAIL;
            break;

		case MCL_HTTP_RESULT_CODE_PAYLOAD_TOO_LARGE:
			code = MCL_REQUEST_PAYLOAD_TOO_LARGE;
			break;

        default :
            code = MCL_UNEXPECTED_RESULT_CODE;
			MCL_INFO("Server responded with unexpected HTTP status code %d.", response->result_code);
            break;
    }

    DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

static E_MCL_ERROR_CODE _generate_correlation_id_string(string_t **correlation_id)
{
	DEBUG_ENTRY("string_t **correlation_id = <%p>", correlation_id)

	mcl_uint16_t index;
    uint8_t random_bytes[CORRELATION_ID_BYTE_LENGTH] = {0};
    E_MCL_ERROR_CODE code = security_generate_random_bytes(random_bytes, CORRELATION_ID_BYTE_LENGTH);

    ASSERT_CODE(MCL_OK == code, code);
    code = string_initialize_new(MCL_NULL, CORRELATION_ID_BYTE_LENGTH * 2, correlation_id);

    ASSERT_CODE(MCL_OK == code, code);

    for(index = 0; index < CORRELATION_ID_BYTE_LENGTH; index++)
    {
        code = string_util_snprintf(&((*correlation_id)->buffer[index * 2]), 3, "%02x", random_bytes[index]);
        ASSERT_STATEMENT_CODE(MCL_OK == code, string_destroy(correlation_id), code);
    }

    DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

E_MCL_ERROR_CODE _save_registration_information(http_processor_t *http_processor)
{
    DEBUG_ENTRY("http_processor_t *http_processor = <%p>", http_processor)

    // Set default error code to MCL_OK assuming if no storage option is provided agent does not want to save data.
    E_MCL_ERROR_CODE result = MCL_OK;

    // Store registration information via custom functions if both function pointers are not null.
    if(MCL_NULL != http_processor->configuration->load_function.rsa && MCL_NULL != http_processor->configuration->save_function.rsa)
    {
        if (MCL_SECURITY_SHARED_SECRET == http_processor->configuration->security_profile)
        {
            result = http_processor->configuration->save_function.shared_secret(http_processor->security_handler->client_id->buffer, http_processor->security_handler->client_secret->buffer, http_processor->security_handler->registration_access_token->buffer, http_processor->security_handler->registration_client_uri->buffer);
        }
        else
        {
            result = http_processor->configuration->save_function.rsa(http_processor->security_handler->client_id->buffer, (char *)http_processor->security_handler->rsa.public_key, (char *)http_processor->security_handler->rsa.private_key, http_processor->security_handler->registration_access_token->buffer, http_processor->security_handler->registration_client_uri->buffer);
        }
    }
    // Store registration information via file system if a storage path is provided.
    else if (MCL_NULL != http_processor->configuration->store_path)
    {
        if (MCL_SECURITY_SHARED_SECRET == http_processor->configuration->security_profile)
        {
            result = storage_save_shared_secret_registration_information(http_processor->configuration->store_path, http_processor->security_handler->client_id,
                    http_processor->security_handler->client_secret, http_processor->security_handler->registration_access_token,
                    http_processor->security_handler->registration_client_uri);
        }
        else
        {
            result = storage_save_rsa_registration_information(http_processor->configuration->store_path, http_processor->security_handler->client_id,
                    (char *)http_processor->security_handler->rsa.public_key, (char *)http_processor->security_handler->rsa.private_key,
                    http_processor->security_handler->registration_access_token, http_processor->security_handler->registration_client_uri);
        }
    }

    DEBUG_LEAVE("retVal = <%d>", result);
    return result;
}

E_MCL_ERROR_CODE _compose_access_token_request_payload(http_processor_t *http_processor, string_t **request_payload)
{
    DEBUG_ENTRY("http_processor_t *http_processor = <%p>", http_processor)

    E_MCL_ERROR_CODE code;
    jwt_t *jwt = MCL_NULL;
    string_t *jwt_string = MCL_NULL;
    string_t *request_payload_head = MCL_NULL;
    const char *head = "grant_type=client_credentials&client_assertion_type=urn:ietf:params:oauth:client-assertion-type:jwt-bearer&client_assertion=";

    // Create a new self issued jwt.
    code = jwt_initialize(http_processor->security_handler, http_processor->configuration->security_profile, http_processor->configuration->tenant, &jwt);

    // Compose jwt string.
    if (MCL_OK == code)
    {
        jwt_string = jwt_get_token(jwt);
        jwt_destroy(&jwt);
        if (MCL_NULL == jwt_string)
        {
            code = MCL_FAIL;
        }
    }

    // Set the constant part of request payload.
    (MCL_OK == code) && (code = string_initialize_static(head, 0, &request_payload_head));

    // Form the final request payload.
    (MCL_OK == code) && (code = string_concatenate(request_payload_head, jwt_string, request_payload));
    string_destroy(&request_payload_head);
    string_destroy(&jwt_string);

    DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}
