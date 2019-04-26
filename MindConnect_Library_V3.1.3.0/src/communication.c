/*!**********************************************************************
*
 * @copyright Copyright (C) 2016 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 *
 *************************************************************************
 *
 * @file     communication.c
 * @date     Jul 15, 2016
 * @brief    Communication module implementation file.
 *
 ************************************************************************/

#include "communication.h"
#include "memory.h"
#include "log_util.h"
#include "definitions.h"
#include "mcl/mcl_communication.h"
#include "event_list.h"

#define USER_AGENT_HEADER_FORMAT "MCL/" MCL_VERSION_ " (%s)"

// This function is used to log configuration which is used to initialize communication.
static void _log_configuration(mcl_configuration_t *configuration);

E_MCL_ERROR_CODE mcl_communication_initialize(mcl_configuration_t *configuration, mcl_communication_t **communication)
{
    DEBUG_ENTRY("mcl_configuration_t *configuration = <%p>, mcl_communication_t **communication = <%p>", configuration, communication)

    E_MCL_ERROR_CODE return_code;
    mcl_size_t host_name_length = 0;
    mcl_size_t proxy_host_length = 0;
    mcl_size_t proxy_user_name_length = 0;
    mcl_size_t proxy_password_length = 0;
    mcl_size_t proxy_domain_length = 0;
    mcl_size_t user_agent_length = 0;
    mcl_size_t tenant_length = 0;
    mcl_size_t initial_access_token_length = 0;
    mcl_size_t store_path_length = 0;

    // Null check of configuration.
    ASSERT_NOT_NULL(configuration);

    // Null check for input argument **communication.
    ASSERT_NOT_NULL(communication);

    // Validate host name.
    ASSERT_CODE(MCL_NULL != configuration->mindsphere_hostname, MCL_INVALID_HOST_NAME);
    host_name_length = string_util_strnlen(configuration->mindsphere_hostname, MAXIMUM_HOST_NAME_LENGTH + 1);
    ASSERT_CODE((0 < host_name_length) && (MAXIMUM_HOST_NAME_LENGTH >= host_name_length), MCL_INVALID_HOST_NAME);

    // Check if proxy is used but do not return error if not used.
    if (MCL_NULL != configuration->proxy_hostname)
    {
        // Validate proxy host name.
        proxy_host_length = string_util_strnlen(configuration->proxy_hostname, MAXIMUM_HOST_NAME_LENGTH + 1);
        ASSERT_CODE((0 < proxy_host_length) && (MAXIMUM_HOST_NAME_LENGTH >= proxy_host_length), MCL_INVALID_PROXY_HOST_NAME);

        // Check if user name is provided but do not return error if not.
        if (MCL_NULL != configuration->proxy_username)
        {
            // Validate proxy user name.
            proxy_user_name_length = string_util_strnlen(configuration->proxy_username, MAXIMUM_PROXY_USER_NAME_LENGTH + 1);
            ASSERT_CODE((0 < proxy_user_name_length) && (MAXIMUM_PROXY_USER_NAME_LENGTH >= proxy_user_name_length), MCL_INVALID_PROXY_USER_NAME);

            // Validate proxy password.
            ASSERT_CODE(MCL_NULL != configuration->proxy_password, MCL_INVALID_PROXY_PASSWORD);
            proxy_password_length = string_util_strnlen(configuration->proxy_password, MAXIMUM_PROXY_PASSWORD_LENGTH + 1);
            ASSERT_CODE((0 < proxy_password_length) && (MAXIMUM_PROXY_PASSWORD_LENGTH >= proxy_password_length), MCL_INVALID_PROXY_PASSWORD);

            // Validate proxy domain.
            if (MCL_NULL != configuration->proxy_domain)
            {
                proxy_domain_length = string_util_strnlen(configuration->proxy_domain, MAXIMUM_PROXY_DOMAIN_LENGTH + 1);
                ASSERT_CODE((0 < proxy_domain_length) && (MAXIMUM_PROXY_DOMAIN_LENGTH >= proxy_domain_length), MCL_INVALID_PROXY_DOMAIN);
            }
        }
    }

    // Validate security profile.
    ASSERT_CODE(MCL_SECURITY_RSA_3072 >= configuration->security_profile, MCL_INVALID_SECURITY_PROFILE);

    // Validate User-Agent.
    ASSERT_CODE(MCL_NULL != configuration->user_agent, MCL_INVALID_USER_AGENT);
    user_agent_length = string_util_strnlen(configuration->user_agent, MAXIMUM_USER_AGENT_LENGTH + 1);
    ASSERT_CODE((0 < user_agent_length) && (MAXIMUM_USER_AGENT_LENGTH >= user_agent_length), MCL_INVALID_USER_AGENT);

    // Validate Tenant.
    ASSERT_CODE(MCL_NULL != configuration->tenant, MCL_INVALID_TENANT);
    tenant_length = string_util_strlen(configuration->tenant);

	// Validate max_http_payload_size.
	ASSERT_CODE_MESSAGE(MIN_HTTP_PAYLOAD_SIZE <= configuration->max_http_payload_size && configuration->max_http_payload_size <= MCL_MAXIMUM_HTTP_PAYLOAD_SIZE,
		MCL_INVALID_MAX_HTTP_PAYLOAD_SIZE, "max_http_payload_size is not in the range of  %d - %d (Inclusive).", MIN_HTTP_PAYLOAD_SIZE, MCL_MAXIMUM_HTTP_PAYLOAD_SIZE);

    // Allocate memory for mcl handle.
    MCL_NEW(*communication);
    ASSERT_CODE_MESSAGE(MCL_NULL != *communication, MCL_OUT_OF_MEMORY, "Memory can not be allocated for communication object.");

    (*communication)->state.initialized = MCL_FALSE;
    (*communication)->http_processor = MCL_NULL;
    (*communication)->configuration.mindsphere_hostname = MCL_NULL;
    (*communication)->configuration.mindsphere_port = 0;
    (*communication)->configuration.mindsphere_certificate = MCL_NULL;
    (*communication)->configuration.proxy_hostname = MCL_NULL;
    (*communication)->configuration.proxy_port = 0;
    (*communication)->configuration.proxy_type = MCL_PROXY_UNKNOWN;
    (*communication)->configuration.proxy_username = MCL_NULL;
    (*communication)->configuration.proxy_password = MCL_NULL;
    (*communication)->configuration.proxy_domain = MCL_NULL;
    (*communication)->configuration.security_profile = MCL_SECURITY_SHARED_SECRET;
    (*communication)->configuration.initial_access_token = MCL_NULL;
    (*communication)->configuration.tenant = MCL_NULL;
    (*communication)->configuration.store_path = MCL_NULL;
    (*communication)->configuration.load_function.rsa = MCL_NULL;
    (*communication)->configuration.save_function.rsa = MCL_NULL;
	(*communication)->configuration.access_token_endpoint = MCL_NULL;
	(*communication)->configuration.exchange_endpoint = MCL_NULL;
	(*communication)->configuration.registration_endpoint = MCL_NULL;
	(*communication)->configuration.enter_critical_section = MCL_NULL;
	(*communication)->configuration.leave_critical_section = MCL_NULL;

    // 16K performs a lot better and the practical minimum is about 400 bytes for libcurl.
    (*communication)->configuration.max_http_payload_size = DEFAULT_HTTP_PAYLOAD_SIZE;
    (*communication)->configuration.user_agent = MCL_NULL;

    // Create new string_t for the host name.
    return_code = string_initialize_new(configuration->mindsphere_hostname, host_name_length, &((*communication)->configuration.mindsphere_hostname));
    ASSERT_STATEMENT_CODE_MESSAGE(MCL_OK == return_code, mcl_communication_destroy(communication), return_code, "Memory can not be allocated for host name.");

    // Copy port number to mcl handle.
    (*communication)->configuration.mindsphere_port = configuration->mindsphere_port;

    // Create new string_t for the certificate.
    if (MCL_NULL != configuration->mindsphere_certificate)
    {
        return_code = string_initialize_new(configuration->mindsphere_certificate, 0, &((*communication)->configuration.mindsphere_certificate));
        ASSERT_STATEMENT_CODE_MESSAGE(MCL_OK == return_code, mcl_communication_destroy(communication), return_code, "Memory can not be allocated for certificate.");
    }

    // Copy security profile to mcl handle.
    (*communication)->configuration.security_profile = configuration->security_profile;

    (*communication)->configuration.max_http_payload_size = configuration->max_http_payload_size;

    // Copy http_request_timeout to mcl_handle.
    (*communication)->configuration.http_request_timeout = configuration->http_request_timeout;

    // Check if proxy is used but do not return error if not used.
    if (MCL_NULL != configuration->proxy_hostname)
    {
        // Create new string_t for the proxy host name.
        return_code = string_initialize_new(configuration->proxy_hostname, proxy_host_length, &((*communication)->configuration.proxy_hostname));
        ASSERT_STATEMENT_CODE_MESSAGE(MCL_OK == return_code, mcl_communication_destroy(communication), return_code, "Memory can not be allocated for proxy host name.");

        // Copy proxy port to mcl handle.
        (*communication)->configuration.proxy_port = configuration->proxy_port;

        // Copy proxy type to mcl handle.
        (*communication)->configuration.proxy_type = configuration->proxy_type;

        // Check if user name is provided but do not return error if not.
        if (MCL_NULL != configuration->proxy_username)
        {
            // Create new string_t for the proxy user name.
            return_code = string_initialize_new(configuration->proxy_username, proxy_user_name_length, &((*communication)->configuration.proxy_username));
            ASSERT_STATEMENT_CODE_MESSAGE(MCL_OK == return_code, mcl_communication_destroy(communication), return_code, "Memory can not be allocated for proxy user name.");

            // Create new string_t for the proxy password.
            return_code = string_initialize_new(configuration->proxy_password, proxy_password_length, &((*communication)->configuration.proxy_password));
            ASSERT_STATEMENT_CODE_MESSAGE(MCL_OK == return_code, mcl_communication_destroy(communication), return_code, "Memory can not be allocated for proxy password.");

            // Create new string_t for the proxy domain.
            if (MCL_NULL != configuration->proxy_domain)
            {
                return_code = string_initialize_new(configuration->proxy_domain, proxy_domain_length, &((*communication)->configuration.proxy_domain));
                ASSERT_STATEMENT_CODE_MESSAGE(MCL_OK == return_code, mcl_communication_destroy(communication), return_code, "Memory can not be allocated for proxy domain.");
            }
        }
    }

    // Create new string_t for the user_agent.
    // Construct user_agent header line once for all request send HTTP processor (the header line will not be changed until configuration has been changed)
    // example MCL/0.9.0.0 (Custom Agent v1.0)
    user_agent_length += 4 + MCL_VERSION_LENGTH_ + 3;

    return_code = string_initialize_new(MCL_NULL, user_agent_length, &((*communication)->configuration.user_agent));
    ASSERT_CODE_MESSAGE(MCL_OK == return_code, return_code, "Memory can not be allocated for the <User-Agent> header line.");

    return_code = string_util_snprintf((*communication)->configuration.user_agent->buffer, user_agent_length + 1, USER_AGENT_HEADER_FORMAT, configuration->user_agent);
    ASSERT_STATEMENT_CODE_MESSAGE(MCL_OK == return_code, mcl_communication_destroy(communication), return_code, "<User-Agent> header line can not be composed.");

    // Create string_t for initial access token if it is provided.
    if ((MCL_NULL != configuration->initial_access_token))
    {
        initial_access_token_length = string_util_strlen(configuration->initial_access_token);
        if (0 < initial_access_token_length)
        {
            return_code = string_initialize_new(configuration->initial_access_token, initial_access_token_length, &((*communication)->configuration.initial_access_token));
            ASSERT_STATEMENT_CODE_MESSAGE(MCL_OK == return_code, mcl_communication_destroy(communication), return_code,
                                          "Memory can not be allocated for initial access token.");
        }
    }

    return_code = string_initialize_new(configuration->tenant, tenant_length, &((*communication)->configuration.tenant));
    ASSERT_STATEMENT_CODE_MESSAGE(MCL_OK == return_code, mcl_communication_destroy(communication), return_code, "Memory can not be allocated for tenant.");

    // Create new string_t for the storage path.
    if (MCL_NULL != configuration->store_path)
    {
        store_path_length = string_util_strlen(configuration->store_path);
        if (0 < store_path_length)
        {
            return_code = string_initialize_new(configuration->store_path, store_path_length, &((*communication)->configuration.store_path));
            ASSERT_STATEMENT_CODE_MESSAGE(MCL_OK == return_code, mcl_communication_destroy(communication), return_code, "Memory can not be allocated for storage path.");
        }
    }

    // Copy custom load & save functions to mcl handle.
    (*communication)->configuration.load_function = configuration->load_function;
    (*communication)->configuration.save_function = configuration->save_function;

    // Set critical section functions.
    (*communication)->configuration.enter_critical_section = configuration->enter_critical_section;
    (*communication)->configuration.leave_critical_section = configuration->leave_critical_section;

    // Initialize http processor.
    return_code = http_processor_initialize(&((*communication)->configuration), &((*communication)->http_processor));
    ASSERT_STATEMENT_CODE_MESSAGE(MCL_OK == return_code, mcl_communication_destroy(communication), return_code, "Http processor initialization failed.");

    // Set state of the mcl handle to "initialized".
    (*communication)->state.initialized = MCL_TRUE;

    MCL_INFO("Communication structure is initialized.");
    _log_configuration(configuration);

    DEBUG_LEAVE("retVal = <%d>", MCL_OK);
    return MCL_OK;
}

E_MCL_ERROR_CODE mcl_communication_destroy(mcl_communication_t **communication)
{
    DEBUG_ENTRY("mcl_communication_t **communication = <%p>", communication)

    // Make sure input argument is not NULL.
    ASSERT_NOT_NULL(communication);

    if (MCL_NULL != *communication)
    {
        // Free configuration memory.
        string_destroy(&((*communication)->configuration.mindsphere_hostname));
        string_destroy(&((*communication)->configuration.mindsphere_certificate));
        string_destroy(&((*communication)->configuration.proxy_hostname));
        string_destroy(&((*communication)->configuration.proxy_username));
        string_destroy(&((*communication)->configuration.proxy_password));
        string_destroy(&((*communication)->configuration.proxy_domain));
        string_destroy(&((*communication)->configuration.user_agent));
        string_destroy(&((*communication)->configuration.initial_access_token));
        string_destroy(&((*communication)->configuration.tenant));
        string_destroy(&((*communication)->configuration.store_path));

        // Destroy Http processor.
        http_processor_destroy(&((*communication)->http_processor));

        // Free MCL handle.
        MCL_FREE(*communication);

        MCL_DEBUG("Communication handle is destroyed.");
    }
    else
    {
        MCL_DEBUG("Communication handle is already null.");
    }

    DEBUG_LEAVE("retVal = <%d>", MCL_OK);
    return MCL_OK;
}

E_MCL_ERROR_CODE mcl_communication_onboard(mcl_communication_t *communication)
{
    DEBUG_ENTRY("mcl_communication_t *communication = <%p>", communication)

    E_MCL_ERROR_CODE result;
    mcl_bool_t critical_section_callbacks_are_used;

    // Null check for the input arguments.
    ASSERT_NOT_NULL(communication);

    critical_section_callbacks_are_used = MCL_NULL != communication->configuration.enter_critical_section && MCL_NULL != communication->configuration.leave_critical_section;
    if(critical_section_callbacks_are_used)
    {
        result = communication->configuration.enter_critical_section();
        ASSERT_CODE(MCL_OK == result, result);
    }

    result = MCL_NOT_INITIALIZED;

    if (mcl_communication_is_initialized(communication))
    {
        if (!mcl_communication_is_onboarded(communication))
        {
            // Start the onboarding procedure.
            result = http_processor_register(communication->http_processor);
            if (MCL_OK == result)
            {
                MCL_INFO("Agent is successfully onboarded.");
            }
            else
            {
                MCL_ERROR("Onboarding of agent failed.");
            }
        }
        else
        {
            MCL_INFO("Agent is already onboarded.");
            result = MCL_ALREADY_ONBOARDED;
        }

        if ((MCL_OK == result ) || (MCL_ALREADY_ONBOARDED == result))
        {
            if (MCL_OK == http_processor_get_access_token(communication->http_processor))
            {
                MCL_INFO("New access token is obtained.");
            }
            else
            {
                MCL_INFO("New access token can not be obtained.");
            }
        }
    }

    if(critical_section_callbacks_are_used)
    {
        communication->configuration.leave_critical_section();
    }

    DEBUG_LEAVE("retVal = <%d>", result);
    return result;
}

E_MCL_ERROR_CODE mcl_communication_rotate_key(mcl_communication_t *communication)
{
    DEBUG_ENTRY("mcl_communication_t *communication = <%p>", communication)

    E_MCL_ERROR_CODE result;
    mcl_bool_t critical_section_callbacks_are_used;

    // Null check for the input arguments.
    ASSERT_NOT_NULL(communication);

    critical_section_callbacks_are_used = MCL_NULL != communication->configuration.enter_critical_section && MCL_NULL != communication->configuration.leave_critical_section;
    if (critical_section_callbacks_are_used)
    {
        result = communication->configuration.enter_critical_section();
        ASSERT_CODE(MCL_OK == result, result);
    }

    result = MCL_NOT_INITIALIZED;

    // Check if MCL is initialized or not.
    if (mcl_communication_is_initialized(communication))
    {
        // Check if the agent is onboarded or not.
        if (mcl_communication_is_onboarded(communication))
        {
            // Perform key rotation.
            MCL_INFO("Key rotation started.");
            result = http_processor_register(communication->http_processor);
            if (MCL_OK == result)
            {
                MCL_INFO("Key successfully rotated.");

                if (MCL_OK == http_processor_get_access_token(communication->http_processor))
                {
                    MCL_INFO("New access token is obtained.");
                }
                else
                {
                    MCL_INFO("New access token can not be obtained.");
                }
            }
            else
            {
                MCL_ERROR("Key rotation failed.");
            }
        }
        else
        {
            result = MCL_NOT_ONBOARDED;
        }
    }

    if(critical_section_callbacks_are_used)
    {
        communication->configuration.leave_critical_section();
    }

    DEBUG_LEAVE("retVal = <%d>", result);
    return result;
}

E_MCL_ERROR_CODE mcl_communication_update_security_information(mcl_communication_t *communication)
{
    DEBUG_ENTRY("mcl_communication_t *communication = <%p>", communication)

    E_MCL_ERROR_CODE result;
    mcl_bool_t critical_section_callbacks_are_used;

    ASSERT_NOT_NULL(communication);

    critical_section_callbacks_are_used = MCL_NULL != communication->configuration.enter_critical_section && MCL_NULL != communication->configuration.leave_critical_section;
    if (critical_section_callbacks_are_used)
    {
        result = communication->configuration.enter_critical_section();
        ASSERT_CODE(MCL_OK == result, result);
    }

    result = MCL_NOT_INITIALIZED;

    // Check if MCL is initialized or not.
    if (mcl_communication_is_initialized(communication))
    {
        // Check if the agent is onboarded.
        if (mcl_communication_is_onboarded(communication))
        {
            // Update security information.
            result = http_processor_update_security_information(communication->http_processor);
        }
        else
        {
            result = MCL_NOT_ONBOARDED;
        }
    }

    if (MCL_OK == result)
    {
        if (MCL_OK == http_processor_get_access_token(communication->http_processor))
        {
            MCL_INFO("Security information is updated, new access token has been acquired.");
        }
        else
        {
            MCL_INFO("Security information is updated, new access token could not be acquired.");
        }
    }

    if(critical_section_callbacks_are_used)
    {
        communication->configuration.leave_critical_section();
    }

    DEBUG_LEAVE("retVal = <%d>", result);
    return result;
}

E_MCL_ERROR_CODE mcl_communication_get_access_token(mcl_communication_t *communication)
{
    DEBUG_ENTRY("mcl_communication_t *communication = <%p>", communication)

    E_MCL_ERROR_CODE result;

    ASSERT_NOT_NULL(communication);

    ASSERT_CODE_MESSAGE(MCL_TRUE == mcl_communication_is_initialized(communication), MCL_NOT_INITIALIZED, "Received communication handle is not initialized!");
    ASSERT_CODE_MESSAGE(MCL_TRUE == mcl_communication_is_onboarded(communication), MCL_NOT_ONBOARDED, "Onboard operation is not performed yet on this mcl_communication handle!");

    result = http_processor_get_access_token(communication->http_processor);

    DEBUG_LEAVE("retVal = <%d>", result);
    return result;
}

E_MCL_ERROR_CODE mcl_communication_exchange(mcl_communication_t *communication, mcl_store_t *store, void **reserved)
{
    DEBUG_ENTRY("mcl_communication_t *communication = <%p>, mcl_store_t *store = <%p>, void **reserved = <%p>", communication, store, reserved)

    E_MCL_ERROR_CODE result;

    // Initial checks :
    ASSERT_NOT_NULL(communication);
    ASSERT_NOT_NULL(store);

	ASSERT_CODE_MESSAGE(MCL_TRUE == mcl_communication_is_initialized(communication), MCL_NOT_INITIALIZED, "Received communication handle is not initialized!");
	ASSERT_CODE_MESSAGE(MCL_TRUE == mcl_communication_is_onboarded(communication), MCL_NOT_ONBOARDED, "Onboard operation is not performed yet on this mcl_communication handle!");
	ASSERT_CODE_MESSAGE(MCL_NULL != communication->http_processor->security_handler->access_token, MCL_NO_ACCESS_TOKEN_EXISTS, "No access token exists.");

    // Trigger http_processor to perform exchange based on store type :
    if (MCL_TRUE == store->streamable)
    {
        result = http_processor_stream(communication->http_processor, store, NULL);
    }
    else
    {
        result = http_processor_exchange(communication->http_processor, store, NULL);
    }

    DEBUG_LEAVE("retVal = <%d>", result);
    return result;
}

E_MCL_ERROR_CODE mcl_communication_process(mcl_communication_t *communication, mcl_store_t *store, void **reserved)
{
    DEBUG_ENTRY("mcl_communication_t *communication = <%p>, mcl_store_t *store = <%p>, void **reserved = <%p>", communication, store, reserved)

	E_MCL_ERROR_CODE result;
	
	result = mcl_communication_exchange(communication, store, NULL);

	if (MCL_UNAUTHORIZED == result || MCL_NO_ACCESS_TOKEN_EXISTS == result)
	{
		MCL_INFO("Exchange has failed. Trying to get / renew access token.");
		result = mcl_communication_get_access_token(communication);
		if (MCL_OK == result)
		{
			MCL_INFO("Access token is obtained. Trying to exchange again.");
			result = mcl_communication_exchange(communication, store, NULL);
			if (MCL_OK == result)
			{
				MCL_INFO("Store is successfully exchanged.");
			}
			else
			{
				MCL_INFO("Exchange has failed.");
			}
		}
		else if (MCL_BAD_REQUEST == result)
		{
			MCL_INFO("Access token can not be obtained. Trying to rotate key.");
			result = mcl_communication_rotate_key(communication);
			ASSERT_CODE_MESSAGE(MCL_OK == result, result, "Key rotation failed.");
			
			MCL_INFO("Key rotated. Trying to exchange again.");
			result = mcl_communication_exchange(communication, store, NULL);
			if (MCL_OK == result)
			{
				MCL_INFO("Store is successfully exchanged.");
			}
			else
			{
				MCL_INFO("Exchange has failed.");
			}
		}
		else
		{
			MCL_INFO("Access token can not be obtained. Exchange is terminating.");
		}

	}

	DEBUG_LEAVE("retVal = <%d>", result);
	return result;
}

#if MCL_FILE_DOWNLOAD_ENABLED
E_MCL_ERROR_CODE mcl_communication_download(mcl_communication_t *communication, mcl_uint8_t *buffer, mcl_size_t buffer_size, mcl_size_t start_byte, mcl_size_t end_byte,
        char *file_id, mcl_file_t **file)
{
    DEBUG_ENTRY("mcl_communication_t *communication = <%p>, mcl_uint8_t *buffer = <%p>, mcl_size_t buffer_size = <%u>, mcl_size_t start_byte = <%u>, mcl_size_t end_byte = <%u>, char *file_id = <%p>, mcl_file_t **file = <%p>",
                communication, buffer, buffer_size, start_byte, end_byte, file_id, file)

    ASSERT_NOT_NULL(file);
    *file = MCL_NULL;

    ASSERT_NOT_NULL(communication);
    ASSERT_NOT_NULL(buffer);
    ASSERT_NOT_NULL(file_id);

	ASSERT_CODE_MESSAGE(MCL_TRUE == mcl_communication_is_initialized(communication), MCL_NOT_INITIALIZED, "Received communication handle is not initialized!");
	ASSERT_CODE_MESSAGE(MCL_TRUE == mcl_communication_is_onboarded(communication), MCL_NOT_ONBOARDED, "Onboard operation is not performed yet on this mcl_communication handle!");

	// If start and end bytes are given zero then whole file is going to be downloaded
    mcl_bool_t with_range = !((0 == start_byte) && (0 == end_byte));

    // Check if range values are valid and buffer size is enough.
    ASSERT_CODE_MESSAGE(!with_range || ((end_byte - start_byte <= buffer_size)), MCL_INVALID_PARAMETER, "start_byte: <%u> - end_byte: <%u> is not a valid range.", start_byte,
                        end_byte);

    string_t *file_id_string = MCL_NULL;

    E_MCL_ERROR_CODE result = string_initialize_new(file_id, 0, &file_id_string);
    ASSERT_CODE_MESSAGE(MCL_OK == result, result, "Memory cannot be allocated for file_uri");

    result = http_processor_download(communication->http_processor, buffer, buffer_size, start_byte, end_byte, file_id_string, with_range, file);
    string_destroy(&file_id_string);

    DEBUG_LEAVE("retVal = <%d>", result);
    return result;
}

#endif

mcl_bool_t mcl_communication_is_initialized(mcl_communication_t *communication)
{
    DEBUG_ENTRY("mcl_communication_t *communication = <%p>", communication)

    mcl_bool_t is_initialized;
        
    ASSERT_NOT_NULL(communication);

    is_initialized = communication->state.initialized;

    DEBUG_LEAVE("retVal = <%p>", &is_initialized);
    return is_initialized;
}

mcl_bool_t mcl_communication_is_onboarded(mcl_communication_t *communication)
{
    DEBUG_ENTRY("mcl_communication_t *communication = <%p>", communication)

    mcl_bool_t is_onboarded;

	ASSERT_NOT_NULL(communication);

    is_onboarded = (MCL_NULL == communication->http_processor->security_handler->registration_access_token) ? MCL_FALSE : MCL_TRUE;

    DEBUG_LEAVE("retVal = <%d>", is_onboarded);
    return is_onboarded;
}

E_MCL_ERROR_CODE mcl_communication_get_last_access_token(mcl_communication_t *communication, char **token)
{
    DEBUG_ENTRY("mcl_communication_t *communication = <%p>, char **token = <%p>", communication, token)

    E_MCL_ERROR_CODE code;
    mcl_size_t token_length;

	ASSERT_NOT_NULL(communication);
	ASSERT_NOT_NULL(token);
	ASSERT_CODE_MESSAGE(MCL_NULL != communication->http_processor->security_handler->access_token, MCL_NO_ACCESS_TOKEN_EXISTS, "No access token found.");

	token_length = communication->http_processor->security_handler->access_token->length + 1;
	*token = MCL_CALLOC(token_length, 1);
	ASSERT_CODE_MESSAGE(MCL_NULL != *token, MCL_OUT_OF_MEMORY, "Could not allocate memory for token.");

	code = string_util_snprintf(*token, token_length, "%s", communication->http_processor->security_handler->access_token->buffer);
	ASSERT_STATEMENT_CODE_MESSAGE(MCL_OK == code, MCL_FREE(*token), MCL_FAIL, "string_util_snprintf failed.");

	DEBUG_LEAVE("retVal = <%d>", MCL_OK);
	return MCL_OK;
}

E_MCL_ERROR_CODE mcl_communication_get_last_token_time(mcl_communication_t *communication, char **token_time)
{
    DEBUG_ENTRY("mcl_communication_t *communication = <%p>, char **token_time = <%p>", communication, token_time)

    E_MCL_ERROR_CODE code;
    mcl_size_t token_time_length;

	ASSERT_NOT_NULL(token_time);
	*token_time = MCL_NULL;

	ASSERT_NOT_NULL(communication);

	if(MCL_NULL == communication->http_processor->security_handler->last_token_time)
	{
	    MCL_ERROR_RETURN(MCL_NO_SERVER_TIME, "Last token time is null.");
	}

	token_time_length = communication->http_processor->security_handler->last_token_time->length + 1;
	*token_time = MCL_CALLOC(token_time_length, 1);
	ASSERT_CODE_MESSAGE(MCL_NULL != *token_time, MCL_OUT_OF_MEMORY, "Could not allocate memory for token_time.");

	code = string_util_snprintf(*token_time, token_time_length, "%s", communication->http_processor->security_handler->last_token_time->buffer);
	ASSERT_STATEMENT_CODE_MESSAGE(MCL_OK == code, MCL_FREE(*token_time), code, "string_util_snprintf failed.");

	DEBUG_LEAVE("retVal = <%d>", code);
	return code;
}

static void _log_configuration(mcl_configuration_t *configuration)
{
    DEBUG_ENTRY("mcl_configuration_t *configuration = <%p>", configuration)

    // This function will be called in success case, no need to check mandatory fields.
    MCL_INFO("Mindsphere Hostname: %s", configuration->mindsphere_hostname);
    MCL_INFO("Mindsphere Port: %u", configuration->mindsphere_port);

    if (MCL_NULL != configuration->mindsphere_certificate)
    {
        MCL_INFO("Mindsphere Certificate: Provided");
    }
    else
    {
        MCL_INFO("Mindsphere Certificate: Not provided");
    }

    if (MCL_NULL != configuration->proxy_hostname)
    {
        MCL_INFO("Proxy Hostname: %s", configuration->proxy_hostname);
        MCL_INFO("Proxy Port: %u", configuration->proxy_port);

        switch (configuration->proxy_type)
        {
            case MCL_PROXY_HTTP:
                MCL_INFO("Proxy Type: MCL_PROXY_HTTP");
                break;

            case MCL_PROXY_HTTP_1_0:
                MCL_INFO("Proxy Type: MCL_PROXY_HTTP_1_0");
                break;

            case MCL_PROXY_SOCKS4:
                MCL_INFO("Proxy Type: MCL_PROXY_SOCKS4");
                break;

            case MCL_PROXY_SOCKS5:
                MCL_INFO("Proxy Type: MCL_PROXY_SOCKS5");
                break;

            case MCL_PROXY_SOCKS4A:
                MCL_INFO("Proxy Type: MCL_PROXY_SOCKS4A");
                break;

            case MCL_PROXY_SOCKS5_HOSTNAME:
                MCL_INFO("Proxy Type: MCL_PROXY_SOCKS5_HOSTNAME");
                break;

            default:
                MCL_INFO("Proxy Type: MCL_PROXY_UNKNOWN");
                break;
        }

        if (MCL_NULL != configuration->proxy_username)
        {
            MCL_INFO("Proxy Username: %s", configuration->proxy_username);
            MCL_INFO("Proxy Password: %s", configuration->proxy_password);

            if (MCL_NULL != configuration->proxy_domain)
            {
                MCL_INFO("Proxy Domain: %s", configuration->proxy_domain);
            }
        }
    }
    else
    {
        MCL_INFO("Proxy: Not used");
    }

    if (MCL_SECURITY_SHARED_SECRET == configuration->security_profile)
    {
        MCL_INFO("Security Profile: MCL_SECURITY_SHARED_SECRET");
    }
    // Invalid parameter check is already done.
    else
    {
        MCL_INFO("Security Profile: MCL_SECURITY_RSA_3072");
    }

    MCL_INFO("Maximum HTTP Payload Size: %u bytes", configuration->max_http_payload_size);
    MCL_INFO("HTTP Request Timeout: %u seconds", configuration->http_request_timeout);
    MCL_INFO("User Agent: %s", configuration->user_agent);

    if (MCL_NULL != configuration->initial_access_token)
    {
        MCL_INFO("Initial Access Token: Provided");
    }
    else
    {
        MCL_INFO("Initial Access Token: Not provided");
    }

    MCL_INFO("Tenant: %s", configuration->tenant);

    if (MCL_NULL != configuration->load_function.rsa && MCL_NULL != configuration->save_function.rsa)
    {
        MCL_INFO("Security Information: Callback functions will be used");
    }
    else if(MCL_NULL != configuration->store_path)
    {
        MCL_INFO("Security Information: File system will be used (not safe)");
        MCL_INFO("Store Path: %s", configuration->store_path);
    }
    else
    {
        MCL_INFO("Security Information: Will not be saved");
    }

    if (MCL_NULL != configuration->enter_critical_section && MCL_NULL != configuration->leave_critical_section)
    {
        MCL_INFO("Critical Section: Callback functions are provided");
    }
    else
    {
        MCL_INFO("Critical Section: Callback functions are not provided");
    }

    DEBUG_LEAVE("retVal = void");
}
