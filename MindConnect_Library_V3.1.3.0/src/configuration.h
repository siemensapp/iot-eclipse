/*!**********************************************************************
 *
 * @copyright Copyright (C) 2016 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 *
 *************************************************************************
 *
 * @file     configuration.h
 * @date     Sep 8, 2016
 * @brief    Configuration module header file.
 *
 * This module contains type definition of configuration.
 *
 ************************************************************************/

#ifndef CONFIGURATION_H_
#define CONFIGURATION_H_

#include "string_type.h"

/**
 * This data structure holds the configuration parameters after @c mcl_communication_t is initialized.
 */
typedef struct configuration_t
{
    string_t *mindsphere_hostname;              //!< Mindsphere hostname.
    mcl_uint16_t mindsphere_port;               //!< Mindsphere port no.
    string_t *mindsphere_certificate;           //!< Mindsphere certificate. Optional. If NULL, MCL will use default CA certificate store (if provided at build-time) for peer verification.
    string_t *proxy_hostname;                   //!< Proxy hostname. Optional.
    mcl_uint16_t proxy_port;                    //!< Proxy port no. Optional if proxy_hostname is not used.
    E_MCL_PROXY proxy_type;                     //!< Proxy type #E_MCL_PROXY. Optional if proxy_hostname is not used.
    string_t *proxy_username;                   //!< Proxy username. Optional if proxy_hostname is not used.
    string_t *proxy_password;                   //!< Proxy password. Optional if proxy_hostname and proxy_username are not used.
    string_t *proxy_domain;                     //!< Proxy domain. Optional if proxy_hostname and proxy_username are not used.
    E_MCL_SECURITY_PROFILE security_profile;    //!< Security levels #E_MCL_SECURITY_PROFILE.
    mcl_size_t max_http_payload_size;           //!< Not valid for streamable request. Default value is 16K Bytes. Minimum value is 400 Bytes and maximum value is the maximum value of mcl_size_t.
    mcl_uint32_t http_request_timeout;          //!< Timeout value (in seconds) for HTTP requests. Default timeout is 300 seconds.
    string_t *user_agent;                       //!< User agent.
    string_t *initial_access_token;             //!< Initial access token. Not used by the library if a registration access token is present in #store_path.
	string_t *registration_endpoint;			//!< Uri for registration endpoint
	string_t *access_token_endpoint;			//!< Uri for access token endpoint.
	string_t *exchange_endpoint;				//!< Uri for exchange endpoint.
    string_t *tenant;                           //!< Tenant name which is used in self issued JWT.
    string_t *store_path;                       //!< Path of the file the library uses to save registration artifacts such as registration access token, etc.
    mcl_load_registration_information_callback_t load_function;     //!< Custom function for loading registration information; if both load_function and save_function are non-null, custom functions will be used.
    mcl_save_registration_information_callback_t save_function;     //!< Custom function for saving registration information; if both load_function and save_function are non-null, custom functions will be used.
    mcl_enter_critical_section_callback_t enter_critical_section;   //!< Custom function for entering critical section (Optional, default is NULL).
    mcl_leave_critical_section_callback_t leave_critical_section;   //!< Custom function for leaving critical section (Optional, default is NULL).
} configuration_t;

#endif //CONFIGURATION_H_
