/*!**********************************************************************
 *
 * @copyright Copyright (C) 2016 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 *
 *************************************************************************
 *
 * @file     mcl_common.h
 * @date     Jul 18, 2016
 * @brief    Common module interface header file.
 *
 * This module contains common type definitions used in various MCL modules.
 *
 ************************************************************************/

#ifndef MCL_COMMON_H_
#define MCL_COMMON_H_

#include "mcl/mcl_config_setup.h"

#include <stddef.h>
#include <stdint.h>
#include <time.h>

#ifdef  __cplusplus
extern "C"
{
#endif

    #define MCL_LOG_UTIL_LEVEL_VERBOSE  1
    #define MCL_LOG_UTIL_LEVEL_DEBUG    2
    #define MCL_LOG_UTIL_LEVEL_INFO     3
    #define MCL_LOG_UTIL_LEVEL_WARN     4
    #define MCL_LOG_UTIL_LEVEL_ERROR    5
    #define MCL_LOG_UTIL_LEVEL_FATAL    6
    #define MCL_LOG_UTIL_LEVEL_NONE     0xFF

    typedef size_t mcl_size_t;
    typedef int8_t mcl_int8_t;
    typedef int16_t mcl_int16_t;
    typedef int32_t mcl_int32_t;
    typedef int64_t mcl_int64_t;
    typedef uint8_t mcl_uint8_t;
    typedef uint16_t mcl_uint16_t;
    typedef uint32_t mcl_uint32_t;
    typedef uint64_t mcl_uint64_t;
    typedef mcl_uint8_t mcl_bool_t;
    typedef time_t mcl_time_t;

    /**
     * @brief MCL bool type.
     */
    #define MCL_FALSE ((mcl_bool_t)0)
    #define MCL_TRUE ((mcl_bool_t)1)

    /**
     * @brief Http request payload size limit for MindSphere.
     * */
    #define MCL_MAXIMUM_HTTP_PAYLOAD_SIZE (10485760)

    /**
     * @brief Proxy type definitions. Used inside of #mcl_configuration_t.
     */
    typedef enum E_MCL_PROXY
    {
        MCL_PROXY_UNKNOWN = -1,       //!< Unknown proxy.
        MCL_PROXY_HTTP = 0,           //!< Http proxy.
        MCL_PROXY_HTTP_1_0 = 1,       //!< Http 1.0 proxy.
        MCL_PROXY_SOCKS4 = 4,         //!< SOCKS4 proxy.
        MCL_PROXY_SOCKS5 = 5,         //!< SOCKS5 proxy.
        MCL_PROXY_SOCKS4A = 6,        //!< SOCKS4a proxy.
        MCL_PROXY_SOCKS5_HOSTNAME = 7 //!< SOCKS5 hostname proxy.
    } E_MCL_PROXY;

    /**
     * @brief Definitions of different security profiles for onboarding.
     */
    typedef enum E_MCL_SECURITY_PROFILE
    {
        MCL_SECURITY_SHARED_SECRET, //!< Shared secret.
        MCL_SECURITY_RSA_3072       //!< RSA 3072.
    } E_MCL_SECURITY_PROFILE;

    /**
     * @brief Configuration types used in connection to the mindsphere.
     */
    typedef enum E_MCL_CONFIGURATION_TYPE
    {
        MCL_CONFIGURATION_MINDSPHERE_HOSTNAME = 0, //!< Mindsphere hostname is used for connection as configuration type.
        MCL_CONFIGURATION_MINDSPHERE_PORT,         //!< Mindsphere port is used for connection as configuration type.
        MCL_CONFIGURATION_MINDSPHERE_CERTIFICATE,  //!< Midsphere certificate is used for connection as configuration type.
        MCL_CONFIGURATION_PROXY_HOSTNAME,          //!< Proxy hostname is used for connection as configuration type.
        MCL_CONFIGURATION_PROXY_PORT,              //!< Proxy port is used for connection as configuration type.
        MCL_CONFIGURATION_PROXY_TYPE,              //!< Proxy type is used for connection as configuration type.
        MCL_CONFIGURATION_PROXY_USER_NAME,         //!< Proxy user name is used for connection as configuration type.
        MCL_CONFIGURATION_PASSWORD,                //!< Password is used for connection as configuration type.
        MCL_CONFIGURATION_DOMAIN,                  //!< Domain is used for connection as configuration type.
        MCL_CONFIGURATION_SECURITY_PROFILE         //!< Security profile is used for connection as configuration type.
    } E_MCL_CONFIGURATION_TYPE;

    /**
     * @brief Source type values for mcl entities.
     */
    typedef enum E_MCL_SOURCE_TYPE
    {
        MCL_SOURCE_TYPE_ASSET,       //!< Source type is asset.
        MCL_SOURCE_TYPE_APPLICATION, //!< Source type is application.
        MCL_SOURCE_TYPE_AGENT        //!< Source type is agent.
    } E_MCL_SOURCE_TYPE;

    /**
     * @brief Severity values for events.
     */
    typedef enum E_MCL_EVENT_SEVERITY
    {
        MCL_EVENT_SEVERITY_ERROR = 0,    //!< Severity level of event is ERROR.
        MCL_EVENT_SEVERITY_WARNING,      //!< Severity level of event is WARNING.
        MCL_EVENT_SEVERITY_INFORMATION,  //!< Severity level of event is INFORMATION.
        MCL_EVENT_SEVERITY_END
    } E_MCL_EVENT_SEVERITY;

    /**
     * @brief Optional parameters for event.
     */
    typedef enum E_MCL_EVENT_OPTION
    {
        MCL_EVENT_OPTION_CORRELATION_ID,    //!< Correlation id option.
        MCL_EVENT_OPTION_DESCRIPTION,       //!< Description option.
        MCL_EVENT_OPTION_DETAILS            //!< Details option.
    } E_MCL_EVENT_OPTION;

    /**
     * @brief MCL Error code definitions. Every function returning an error code uses this enum values.
     *
     * @warning When you add a new error code, do NOT forget to add it to error_code_strings[MCL_ERROR_CODE_END] in log_util.c as well.
     */
    typedef enum E_MCL_ERROR_CODE
    {
        // General error codes.
        MCL_OK = 0,                                  //!< Success.
        MCL_FAIL,                                    //!< Internal failure in MCL.
        MCL_TRIGGERED_WITH_NULL,                     //!< Received parameter is null.
        MCL_OUT_OF_MEMORY,                           //!< Memory allocation fail.
        MCL_INVALID_PARAMETER,                       //!< General invalid parameter fail.
        MCL_NO_FILE_SUPPORT,                         //!< The system does not support file handling.
        MCL_NO_STORAGE_MEDIUM,                       //!< The system does not have a storage medium.
        MCL_INVALID_LOG_LEVEL,                       //!< Given log level is invalid.
        MCL_OPERATION_IS_NOT_SUPPORTED,              //!< If a particular operation is not supported.
        MCL_FILE_CANNOT_BE_OPENED,                   //!< File can not be opened.
        MCL_JSON_NAME_DUPLICATION,                   //!< The same name can not be added in the same level of json object.
        MCL_INDEX_OUT_OF_RANGE,                      //!< If provided index is greater than collection size.
        MCL_ITEM_ALREADY_EXISTS,                     //!< Occurs when same item is tried to be added to a collection of unique items.
        MCL_INVALID_EVENT_SEVERITY_LEVEL,            //!< Event severity level is invalid.
        MCL_NO_SERVER_TIME,                          //!< Server time is not received from the server.
        MCL_NO_ACCESS_TOKEN_EXISTS,                  //!< No access token exists in #mcl_communication_t handle.

        // Error codes for configuration parameters.
        MCL_INVALID_HOST_NAME,                       //!< Host name given as a configuration parameter is either NULL or of inappropriate size.
        MCL_INVALID_CERTIFICATE,                     //!< MindSphere certificate given as a configuration parameter is either NULL or of inappropriate size.
        MCL_INVALID_PROXY_HOST_NAME,                 //!< MindSphere proxy host name given as a configuration parameter is either NULL or of inappropriate size.
        MCL_INVALID_PROXY_USER_NAME,                 //!< MindSphere proxy user name given as a configuration parameter is either NULL or of inappropriate size.
        MCL_INVALID_PROXY_PASSWORD,                  //!< MindSphere proxy password given as a configuration parameter is either NULL or of inappropriate size.
        MCL_INVALID_PROXY_DOMAIN,                    //!< MindSphere proxy domain given as a configuration parameter is either NULL or of inappropriate size.
        MCL_INVALID_SECURITY_PROFILE,                //!< Security profile given as a configuration parameter is invalid. Please check the security profile options.
        MCL_INVALID_USER_AGENT,                      //!< User-Agent for every HTTP request given as configuration parameter is either NULL or longer than 32 characters.
        MCL_INVALID_TENANT,                          //!< Tenant for JWT is NULL.
        MCL_INVALID_MAX_HTTP_PAYLOAD_SIZE,           //!< Max HTTP payload size for every HTTP request given as configuration parameter is inappropriate size.
        MCL_NO_ACCESS_TOKEN_PROVIDED,                //!< Neither initial access token - initial registration URI pair nor the store path with registration information is provided.
        MCL_PATH_NOT_ACCESSIBLE,                     //!< File provided in store path parameter in configuration structure is not accessible.

        // HTTPS error codes.
        MCL_COULD_NOT_RESOLVE_PROXY,                 //!< Proxy host name given as a configuration parameter could not be resolved.
        MCL_COULD_NOT_RESOLVE_HOST,                  //!< Host name given as a configuration parameter could not be resolved.
        MCL_COULD_NOT_CONNECT,                       //!< MCL failed to connect to the host or proxy.
        MCL_SSL_HANDSHAKE_FAIL,                      //!< A problem occured during SSL/TLS handshake.
        MCL_NETWORK_SEND_FAIL,                       //!< A problem occured when sending data to the network.
        MCL_NETWORK_RECEIVE_FAIL,                    //!< A problem occured when receiving data from the network.
        MCL_SERVER_CERTIFICATE_NOT_VERIFIED,         //!< Mindsphere certificate was not verified.
        MCL_CA_CERTIFICATE_AUTHENTICATION_FAIL,      //!< The server certificate can not be authenticated by the root certificate.
        MCL_IMPROPER_CERTIFICATE,                    //!< The server certificate provided is in improper format and it can not be parsed.
        MCL_REQUEST_TIMEOUT,                         //!< The server did not respond within a timeout period.
        MCL_SERVER_FAIL,                             //!< Internal server error.
        MCL_CREATED,                                 //!< If the response of server is 201.
        MCL_PARTIAL_CONTENT,                         //!< If the response of server is 206.
        MCL_BAD_REQUEST,                             //!< If the response of server is 400.
        MCL_UNAUTHORIZED,                            //!< If the response of server is 401.
        MCL_FORBIDDEN,                               //!< If the response of server is 403.
        MCL_NOT_FOUND,                               //!< If the response of server is 404.
        MCL_CONFLICT,                                //!< If the response of server is 409.
        MCL_PRECONDITION_FAIL,                       //!< If the response of server is 412.
		MCL_REQUEST_PAYLOAD_TOO_LARGE,				 //!< If the response of server is 413.
        MCL_UNEXPECTED_RESULT_CODE,                  //!< If the response of server is unexpected.

        // Status error codes.
        MCL_NOT_INITIALIZED,                          //!< MCL is not initialized, mcl_communication_initialize() function must be called first.
        MCL_NOT_ONBOARDED,                            //!< Agent is not onboarded to the server yet and does not possess an authentication key.
        MCL_ALREADY_ONBOARDED,                        //!< Agent is already onboarded to the server, hence the library did not try to onboard again.
        MCL_STORE_IS_EMPTY,                           //!< The store trying to be exchanged has no data inside.
        MCL_EXCHANGE_STREAMING_IS_ACTIVE,             //!< The streaming is active and shouldn't be interrupted.
        MCL_SECURITY_UP_TO_DATE,                      //!< Security information of the mcl_communication is already up to date.
        MCL_CANNOT_ENTER_CRITICAL_SECTION,            //!< If agent cannot enter critical section.

        // Operational error codes.
        MCL_INITIALIZATION_FAIL,                      //!< Initialization of MCL fails.
        MCL_STORE_ITEM_EXCEEDS_MAX_HTTP_REQUEST_SIZE, //!< Store which is not streamable has an item that does not fit in a single http request.
        MCL_UNDEFINED_ENDPOINT,                       //!< Unexpected endpoint type is received by the function to create a URI.
        MCL_BAD_CONTENT_ENCODING,                     //!< If given content for Base64 encoding is bad.
        MCL_NON_EXISTING_JSON_CHILD,                  //!< Json child which we try to get doesn't exist in the parent Json object.
        MCL_SHA256_CALCULATION_FAIL,                  //!< If SHA256 calculation fails.
        MCL_SERVER_PROOF_FAIL,                        //!< Server proof received with the server nonce mismatch.
        MCL_REGISTRATION_INFO_IS_NOT_SAVED,           //!< Registration information is not saved.
        MCL_REGISTRATION_INFO_IS_NOT_LOADED,          //!< Registration information is not loaded.
        MCL_BUFFER_OVERFLOW_ERROR,                    //!< Indicates that a provided buffer does not have enough size for the operation.

        // Internal error codes.
        MCL_HTTP_REQUEST_NO_MORE_SPACE,               //!< There is no more space left in http message buffer. No data has been written in current call.
        MCL_LIMIT_EXCEEDED,                           //!< No more space is left to add an additional object.
        MCL_ARRAY_IS_EMPTY,                           //!< There is no element in the array.
        MCL_PARTIALLY_WRITTEN,                        //!< Part of the data in store is written to the http request. There is still data left needs to be written.
        MCL_HTTP_REQUEST_FINALIZE_FAILED,             //!< Adding final closing boundary and Authentication header is failed.
        MCL_ERROR_CODE_END                            //!< End of error codes.
        //Do NOT add new error codes after MCL_ERROR_CODE_END!!!
    } E_MCL_ERROR_CODE;

    /**
     *
     * @brief Callback function prototype to load registration information for Shared Secret security profile.
     *
     * @param [out] client_id Client ID.
     * @param [out] client_secret Client secret.
     * @param [out] registration_access_token Registration access token.
     * @param [out] registration_uri Registration URI.
     * @return
     * <ul>
     * <li>#MCL_OK in case of success, all parameters must be dynamically allocated.</li>
     * <li>#MCL_REGISTRATION_INFO_IS_NOT_LOADED if registration information can not be loaded, all parameters must be set to NULL.</li>
     * </ul>
     */
    typedef E_MCL_ERROR_CODE (*mcl_load_shared_secret_callback_t)(char **client_id, char **client_secret, char **registration_access_token, char **registration_uri);

    /**
     *
     * @brief Callback function prototype to save registration information for Shared Secret security profile.
     *
     * @param [in] client_id Client ID.
     * @param [in] client_secret Client secret.
     * @param [in] registration_access_token Registration access token.
     * @param [in] registration_uri Registration URI.
     * @return
     * <ul>
     * <li>#MCL_OK in case of success.</li>
     * <li>#MCL_REGISTRATION_INFO_IS_NOT_SAVED if registration information can not be saved.</li>
     * </ul>
     */
    typedef E_MCL_ERROR_CODE (*mcl_save_shared_secret_callback_t)(const char *client_id, const char *client_secret, const char *registration_access_token,
        const char *registration_uri);

    /**
     * @brief Callback function prototype to load registration information for RSA security profile.
     *
     * @param [out] client_id Client ID.
     * @param [out] public_key Public key.
     * @param [out] private_key Private key.
     * @param [out] registration_access_token Registration access token.
     * @param [out] registration_uri Registration URI.
     * @return
     * <ul>
     * <li>#MCL_OK in case of success, all parameters must be dynamically allocated.</li>
     * <li>#MCL_REGISTRATION_INFO_IS_NOT_LOADED if registration information can not be loaded, all parameters must be set to NULL.</li>
     * </ul>
     */
    typedef E_MCL_ERROR_CODE (*mcl_load_rsa_callback_t)(char **client_id, char **public_key, char **private_key, char **registration_access_token, char **registration_uri);

    /**
     * @brief Callback function prototype to save registration information for RSA security profile.
     *
     * @param [in] client_id Client ID.
     * @param [in] public_key Public key.
     * @param [in] private_key Private key.
     * @param [in] registration_access_token Registration access token.
     * @param [in] registration_uri Registration URI.
     * @return
     * <ul>
     * <li>#MCL_OK in case of success.</li>
     * <li>#MCL_REGISTRATION_INFO_IS_NOT_SAVED if registration information can not be saved.</li>
     * </ul>
     */
    typedef E_MCL_ERROR_CODE (*mcl_save_rsa_callback_t)(const char *client_id, const char *public_key, const char *private_key, const char *registration_access_token,
        const char *registration_uri);

    /**
     * This union is used to assign a callback function for loading registration information (whether RSA or shared secret).
     */
    typedef union mcl_load_registration_information_callback_t
    {
        mcl_load_shared_secret_callback_t shared_secret; //!< Callback type to load shared secret.
        mcl_load_rsa_callback_t rsa;                     //!< Callback type to load RSA key.
    } mcl_load_registration_information_callback_t;

    /**
     * This union is used to assign a callback function for saving registration information (whether RSA or shared secret).
     */
    typedef union mcl_save_registration_information_callback_t
    {
        mcl_save_shared_secret_callback_t shared_secret; //!< Callback type to save shared secret.
        mcl_save_rsa_callback_t rsa;                     //!< Callback type to save RSA key.
    } mcl_save_registration_information_callback_t;

    /**
     *
     * @brief Callback function prototype to enter critical section (onboarding, key rotation, updating security information).
     *
     * @return
     * <ul>
     * <li>#MCL_OK in case of success.</li>
     * <li>#MCL_CANNOT_ENTER_CRITICAL_SECTION if cannot enter critical section.</li>
     * </ul>
     */
    typedef E_MCL_ERROR_CODE (*mcl_enter_critical_section_callback_t)(void);

    /**
     *
     * @brief Callback function prototype to leave critical section (onboarding, key rotation, updating security information).
     *
     */
    typedef void (*mcl_leave_critical_section_callback_t)(void);

#ifdef  __cplusplus
}
#endif

#endif //MCL_COMMON_H_
