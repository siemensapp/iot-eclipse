/*!**********************************************************************
*
* @copyright Copyright (C) 2016 Siemens Aktiengesellschaft.\n
*            All rights reserved.
*
*************************************************************************
*
* @file     mcl_configuration.h
* @date     Aug 3, 2016
* @brief    Configuration module interface header file.
*
* This module is used for configuring MCL.
*
************************************************************************/

#ifndef MCL_CONFIGURATION_H_
#define MCL_CONFIGURATION_H_

#include "mcl/mcl_common.h"

#ifdef  __cplusplus
extern "C"
{
#endif

    /**
     * This data structure holds the configuration parameters which are used to initialize @c mcl_communication_t data structure.
     * @warning @p load_function and @p save_function are NULL by default.
     * @warning Keeping any of these callback functions as NULL will result in registration information being saved to (and read from) the file (specified by store_path, if file system is available) with no encryption.
     * @warning It is recommended to assign valid functions for both reading and saving registration information.
     */
    typedef struct mcl_configuration_t
    {
        char *mindsphere_hostname;                                      //!< Mindsphere hostname.
        mcl_uint16_t mindsphere_port;                                   //!< Mindsphere port no.
        char *mindsphere_certificate;                                   //!< Mindsphere certificate. Optional. If set to NULL, MCL will use default CA certificate store (if provided at build-time) for peer verification.
        char *proxy_hostname;                                           //!< Proxy hostname. Optional.
        mcl_uint16_t proxy_port;                                        //!< Proxy port no. Optional if proxy_hostname is not used.
        E_MCL_PROXY proxy_type;                                         //!< Proxy type #E_MCL_PROXY. Optional if proxy_hostname is not used.
        char *proxy_username;                                           //!< Proxy username. Optional if proxy_hostname is not used.
        char *proxy_password;                                           //!< Proxy password. Optional if proxy_hostname and proxy_username are not used.
        char *proxy_domain;                                             //!< Proxy domain. Optional if proxy_hostname and proxy_username are not used.
        E_MCL_SECURITY_PROFILE security_profile;                        //!< Security levels #E_MCL_SECURITY_PROFILE.
        mcl_size_t max_http_payload_size;                               //!< Not valid for streamable request. Default value is 16K Bytes. Minimum value is 400 Bytes and maximum value is the maximum value of mcl_size_t.
        mcl_uint32_t http_request_timeout;                              //!< Timeout value (in seconds) for HTTP requests. Default timeout is 300 seconds.
        char *user_agent;                                               //!< User agent.
        char *initial_access_token;                                     //!< Initial access token. Not used by the library if a registration access token is present in #store_path.
        char *tenant;                                                   //!< Tenant name which is used in self issued JWT.
        char *store_path;                                               //!< Path of the file the library uses to save registration artifacts such as registration access token, etc.
        mcl_load_registration_information_callback_t load_function;     //!< Custom function for loading registration information; if both load_function and save_function are non-null, custom functions will be used.
        mcl_save_registration_information_callback_t save_function;     //!< Custom function for saving registration information; if both load_function and save_function are non-null, custom functions will be used.
        mcl_enter_critical_section_callback_t enter_critical_section;   //!< Custom function for entering critical section (Optional, default is NULL).
        mcl_leave_critical_section_callback_t leave_critical_section;   //!< Custom function for leaving critical section (Optional, default is NULL).
    } mcl_configuration_t;

    /**
     * This function initializes a new #mcl_configuration_t data structure with default values and returns error code.
     *
     * @param [out] configuration Data structure holding the configuration parameters.
     * @return
     * <ul>
     * <li>#MCL_OK in case of success.</li>
     * <li>#MCL_TRIGGERED_WITH_NULL in case @p configuration is NULL.</li>
     * <li>#MCL_OUT_OF_MEMORY in case there is not enough memory in the system to proceed.</li>
     * </ul>
     */
    extern MCL_EXPORT E_MCL_ERROR_CODE mcl_configuration_initialize(mcl_configuration_t **configuration);

    /**
     * This function creates a new #mcl_configuration_t data structure with default values
     * and returns a pointer to the new data structure.
     *
     * @return Pointer to #mcl_configuration_t data structure.
     */
    extern MCL_EXPORT mcl_configuration_t *mcl_new_configuration();

    /**
     * This function destroys the #mcl_configuration_t type object.
     *
     * @param [in] configuration Data structure holding the configuration parameters.
     * @return
     * <ul>
     * <li>#MCL_OK in case of success.</li>
     * <li>#MCL_TRIGGERED_WITH_NULL in case @p configuration is NULL.</li>
     * </ul>
     */
    extern MCL_EXPORT E_MCL_ERROR_CODE mcl_configuration_destroy(mcl_configuration_t **configuration);

#ifdef  __cplusplus
}
#endif

#endif //MCL_CONFIGURATION_H_
