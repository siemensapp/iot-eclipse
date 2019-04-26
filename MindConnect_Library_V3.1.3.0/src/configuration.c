/*!**********************************************************************
*
* @copyright Copyright (C) 2016 Siemens Aktiengesellschaft.\n
*            All rights reserved.
*
*************************************************************************
*
* @file     configuration.c
* @date     Aug 3, 2016
* @brief    Configuration module implementation file.
*
************************************************************************/

#include "data_types.h"
#include "memory.h"
#include "configuration.h"
#include "log_util.h"
#include "definitions.h"
#include "mcl/mcl_configuration.h"

E_MCL_ERROR_CODE mcl_configuration_initialize(mcl_configuration_t **configuration)
{
    DEBUG_ENTRY("mcl_configuration_t **configuration = <%p>", configuration)

    ASSERT_NOT_NULL(configuration);

    ASSERT_CODE_MESSAGE(MCL_NULL != MCL_NEW(*configuration), MCL_OUT_OF_MEMORY, "Not enough memory to allocate new configuration.");

    (*configuration)->mindsphere_hostname = MCL_NULL;
    (*configuration)->mindsphere_port = 0;
    (*configuration)->mindsphere_certificate = MCL_NULL;
    (*configuration)->proxy_hostname = MCL_NULL;
    (*configuration)->proxy_port = 0;
    (*configuration)->proxy_type = MCL_PROXY_UNKNOWN;
    (*configuration)->proxy_username = MCL_NULL;
    (*configuration)->proxy_password = MCL_NULL;
    (*configuration)->proxy_domain = MCL_NULL;
    (*configuration)->security_profile = MCL_SECURITY_SHARED_SECRET;
    (*configuration)->max_http_payload_size = DEFAULT_HTTP_PAYLOAD_SIZE;
    (*configuration)->http_request_timeout = DEFAULT_HTTP_REQUEST_TIMEOUT;
    (*configuration)->user_agent = MCL_NULL;
    (*configuration)->initial_access_token = MCL_NULL;
    (*configuration)->tenant = MCL_NULL;
    (*configuration)->store_path = MCL_NULL;
    (*configuration)->load_function.rsa = MCL_NULL;
    (*configuration)->save_function.rsa = MCL_NULL;
    (*configuration)->enter_critical_section = MCL_NULL;
    (*configuration)->leave_critical_section = MCL_NULL;

    DEBUG_LEAVE("retVal = <%d>", MCL_OK);
    return MCL_OK;
}

mcl_configuration_t *mcl_new_configuration()
{
    DEBUG_ENTRY("void")

    mcl_configuration_t *configuration = MCL_NULL;

    mcl_configuration_initialize(&configuration);

    DEBUG_LEAVE("retVal = <%p>", configuration);
    return configuration;
}

E_MCL_ERROR_CODE mcl_configuration_destroy(mcl_configuration_t **configuration)
{
    DEBUG_ENTRY("mcl_configuration_t **configuration = <%p>", configuration)

    // Make sure input argument is not NULL.
    ASSERT_NOT_NULL(configuration);

    if (MCL_NULL != *configuration)
    {
        MCL_FREE(*configuration);
    }

    DEBUG_LEAVE("retVal = <%d>", MCL_OK);
    return MCL_OK;
}
