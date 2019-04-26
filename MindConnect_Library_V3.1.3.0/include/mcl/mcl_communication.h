/*!**********************************************************************
 *
 * @copyright Copyright (C) 2016 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 *
 *************************************************************************
 *
 * @file     mcl_communication.h
 * @date     Sep 8, 2016
 * @brief    Communication module interface header file.
 *
 * This module implements MCL communication interface.
 *
 ************************************************************************/

#ifndef MCL_COMMUNICATION_H_
#define MCL_COMMUNICATION_H_

#include "mcl/mcl_store.h"
#include "mcl/mcl_configuration.h"
#include "mcl/mcl_list.h"

#ifdef  __cplusplus
extern "C"
{
#endif

    /**
     * This data structure is the main handle for Agent - MindSphere communication.
     */
    typedef struct mcl_communication_t mcl_communication_t;

    /**
     * This function creates and initializes an object of type #mcl_communication_t
     * according to the configuration parameters passed in as an argument.
     * Memory is allocated for the @p communication object and configuration parameters
     * are copied to it.
     *
     * @param [in] configuration Data structure holding the configuration parameters for the @p communication object.
     * @param [out] communication Pointer of pointer to the new #mcl_communication_t object.
     * @return
     * <ul>
     * <li>#MCL_OK in case of success.</li>
     * <li>#MCL_TRIGGERED_WITH_NULL in case any mandatory input parameter is NULL.</li>
     * <li>#MCL_OUT_OF_MEMORY in case there is not enough memory in the system to proceed.</li>
     * <li>#MCL_INVALID_HOST_NAME in case the host name provided within @p configuration is either NULL or has zero length or its length exceeds maximum allowed length.</li>
     * <li>#MCL_INVALID_PROXY_HOST_NAME in case the proxy host name provided within @p configuration is not NULL but has zero length or its length exceeds maximum allowed length.</li>
     * <li>#MCL_INVALID_PROXY_USER_NAME in case proxy user name provided within @p configuration has zero length or its length exceeds maximum allowed length even though a valid proxy host name is provided and proxy user name is not NULL.</li>
     * <li>#MCL_INVALID_PROXY_PASSWORD in case the proxy password provided within @p configuration is either NULL or has zero length or its length exceeds maximum allowed length even though valid proxy host name and user name are provided.</li>
     * <li>#MCL_INVALID_PROXY_DOMAIN in case the proxy domain provided within @p configuration has zero length or its length exceeds maximum allowed length even though valid proxy host name, user name and password are provided.</li>
     * <li>#MCL_INVALID_SECURITY_PROFILE in case the security profile provided within @p configuration is not one of the supported profiles.</li>
     * <li>#MCL_INVALID_USER_AGENT in case the user agent provided within @p configuration is either NULL or its length exceeds maximum allowed length.</li>
     * <li>#MCL_INVALID_TENANT in case the tenant provided within @p configuration is NULL.</li>
     * <li>#MCL_INVALID_MAX_HTTP_PAYLOAD_SIZE in case the max http payload size provided within @p configuration is less than minimum or more than maximum allowed size.</li>
     * <li>#MCL_PATH_NOT_ACCESSIBLE in case the store path is not accessible.</li>
     * <li>#MCL_NO_ACCESS_TOKEN_PROVIDED if neither initial access token - initial registration URI pair nor the store path with registration information is provided.
     * <li>#MCL_INITIALIZATION_FAIL in case initialization procedure fails due to malfunction of the library.</li>
     * </ul>
     */
    extern MCL_EXPORT E_MCL_ERROR_CODE mcl_communication_initialize(mcl_configuration_t *configuration, mcl_communication_t **communication);

    /**
     * This function destroys the @c mcl_communication_t object and frees any memory allocated.
     *
     * @param [in] communication Preinitialized @c mcl_communication_t object to destroy.
     * @return
     * <ul>
     * <li>#MCL_OK in case of success.</li>
     * <li>#MCL_TRIGGERED_WITH_NULL in case input parameter is NULL.</li>
     * </ul>
     */
    extern MCL_EXPORT E_MCL_ERROR_CODE mcl_communication_destroy(mcl_communication_t **communication);

    /**
     * This function connects the agent to MindSphere which is called Onboarding.
     *
     * @param [in] communication Preinitialized @c mcl_communication_t object through which a connection will be established to MindSphere.
     * @return
     * <ul>
     * <li>#MCL_OK in case of success.</li>
     * <li>#MCL_TRIGGERED_WITH_NULL in case any input parameter is NULL.</li>
     * <li>#MCL_OUT_OF_MEMORY in case there is not enough memory in the system to proceed.</li>
     * <li>#MCL_NOT_INITIALIZED in case @p communication is not initialized.</li>
     * <li>#MCL_ALREADY_ONBOARDED in case the agent is already onboarded using @p communication.</li>
     * <li>#MCL_COULD_NOT_RESOLVE_PROXY if the proxy host name can not be resolved.</li>
     * <li>#MCL_COULD_NOT_RESOLVE_HOST if the remote host name can not be resolved.</li>
     * <li>#MCL_SSL_HANDSHAKE_FAIL if a problem occurs during SSL handshake.</li>
     * <li>#MCL_COULD_NOT_CONNECT if there is a connection error with the server.</li>
     * <li>#MCL_SERVER_CERTIFICATE_NOT_VERIFIED if the server can not be verified.</li>
     * <li>#MCL_IMPROPER_CERTIFICATE in case the certificate provided is in improper format and it can not be parsed.</li>
     * <li>#MCL_CA_CERTIFICATE_AUTHENTICATION_FAIL if the server certificate can not be authenticated by the root certificate.</li>
     * <li>#MCL_NETWORK_SEND_FAIL in case of an error in sending data to network.</li>
     * <li>#MCL_NETWORK_RECEIVE_FAIL in case of an error in receiving data from the network.</li>
     * <li>#MCL_UNAUTHORIZED if response status code of server is related to authorization.</li>
     * <li>#MCL_SERVER_FAIL if the server returns 500 response status code.</li>
     * <li>#MCL_FAIL in case of an internal error in MCL.</li>
     * </ul>
     */
    extern MCL_EXPORT E_MCL_ERROR_CODE mcl_communication_onboard(mcl_communication_t *communication);

    /**
     * This function is used by the agent which is already onboarded to MindSphere to generate a new authentication key using the
     * authentication key acquired earlier. Key rotation can be applied only when there is an authentication key available. Onboarding key
     * is not necessary for key rotation.
     *
     * @param [in] communication @c mcl_communication_t object which is initialized and onboarded to MindSphere.
     * @return
     * <ul>
     * <li>#MCL_OK in case of success.</li>
     * <li>#MCL_TRIGGERED_WITH_NULL if provided @p communication is NULL.</li>
     * <li>#MCL_NOT_INITIALIZED if @p communication is not initialized.</li>
     * <li>#MCL_NOT_ONBOARDED if the agent is not onboarded using @p communication.</li>
     * <li>#MCL_OUT_OF_MEMORY if there is not enough memory in the system to proceed.</li>
     * <li>#MCL_COULD_NOT_RESOLVE_PROXY if the proxy host name can not be resolved.</li>
     * <li>#MCL_COULD_NOT_RESOLVE_HOST if the remote host name can not be resolved.</li>
     * <li>#MCL_SSL_HANDSHAKE_FAIL if a problem occurs during SSL handshake.</li>
     * <li>#MCL_COULD_NOT_CONNECT if there is a connection error with the server.</li>
     * <li>#MCL_SERVER_CERTIFICATE_NOT_VERIFIED if the server can not be verified.</li>
     * <li>#MCL_IMPROPER_CERTIFICATE in case the certificate provided is in improper format and it can not be parsed.</li>
     * <li>#MCL_CA_CERTIFICATE_AUTHENTICATION_FAIL if the server certificate can not be authenticated by the root certificate.</li>
     * <li>#MCL_NETWORK_SEND_FAIL in case of an error in sending data to network.</li>
     * <li>#MCL_NETWORK_RECEIVE_FAIL in case of an error in receiving data from the network.</li>
     * <li>#MCL_UNAUTHORIZED if response status code of server is related to authorization.</li>
     * <li>#MCL_SERVER_FAIL if the server returns 500 response status code.</li>
     * <li>#MCL_FAIL in case of an internal error in MCL.</li>
     * </ul>
     */
    extern MCL_EXPORT E_MCL_ERROR_CODE mcl_communication_rotate_key(mcl_communication_t *communication);

    /**
     * This function is used by the agent which is already onboarded to update security information.
     *
     * @param [in] communication @c mcl_communication_t object which is initialized and onboarded to MindSphere.
     * @return
     * <ul>
     * <li>#MCL_OK in case of successful update.</li>
     * <li>#MCL_SECURITY_UP_TO_DATE if security information of the mcl_communication is already up to date.
     * <li>#MCL_FAIL in case of an internal error in MCL.</li>
     * </ul>
     */
    extern MCL_EXPORT E_MCL_ERROR_CODE mcl_communication_update_security_information(mcl_communication_t *communication);

/**
     * This function is used to get access token from MindSphere.
     * The access token contains the necessary authentication credentials for further exchange requests.
     * The agent can use this function to get a new access token when #mcl_communication_exchange function returns #MCL_UNAUTHORIZED or #MCL_NO_ACCESS_TOKEN_EXISTS error code.
     * The agent can use #mcl_communication_rotate_key function to renew its secret key if #mcl_communication_get_access_token function returns #MCL_BAD_REQUEST.
     * A new access token will be automatically obtained by MCL after successful onboarding or key rotation so that the agent does not need
     * to call #mcl_communication_get_access_token after a call to #mcl_communication_onboard and #mcl_communication_rotate_key functions.
     *
     * @param [in] communication @c mcl_communication_t object which is initialized and onboarded to MindSphere.
     * @return
     * <ul>
     * <li>#MCL_OK in case of success.</li>
     * <li>#MCL_TRIGGERED_WITH_NULL if provided @p communication is NULL.</li>
     * <li>#MCL_NOT_INITIALIZED if @p communication is not initialized.</li>
     * <li>#MCL_NOT_ONBOARDED if the agent is not onboarded using @p communication.</li>
     * <li>#MCL_OUT_OF_MEMORY if there is not enough memory in the system to proceed.</li>
     * <li>#MCL_COULD_NOT_RESOLVE_PROXY if the proxy host name can not be resolved.</li>
     * <li>#MCL_COULD_NOT_RESOLVE_HOST if the remote host name can not be resolved.</li>
     * <li>#MCL_SSL_HANDSHAKE_FAIL if a problem occurs during SSL handshake.</li>
     * <li>#MCL_COULD_NOT_CONNECT if there is a connection error with the server.</li>
     * <li>#MCL_SERVER_CERTIFICATE_NOT_VERIFIED if the server can not be verified.</li>
     * <li>#MCL_IMPROPER_CERTIFICATE in case the certificate provided is in improper format and it can not be parsed.</li>
     * <li>#MCL_CA_CERTIFICATE_AUTHENTICATION_FAIL if the server certificate can not be authenticated by the root certificate.</li>
     * <li>#MCL_NETWORK_SEND_FAIL in case of an error in sending data to network.</li>
     * <li>#MCL_NETWORK_RECEIVE_FAIL in case of an error in receiving data from the network.</li>
     * <li>#MCL_BAD_REQUEST if response status code of server is 400.</li>
     * <li>#MCL_UNAUTHORIZED if response status code of server is 401.</li>
     * <li>#MCL_SERVER_FAIL if response status code of server is 500.</li>
     * <li>#MCL_FAIL in case of an internal error in MCL.</li>
     * </ul>
     */
    extern MCL_EXPORT E_MCL_ERROR_CODE mcl_communication_get_access_token(mcl_communication_t *communication);

    /**
     * This function exchanges data in @p store to MindSphere.
     *
     * @param [in] communication Preinitialized @c mcl_communication_t object through which a connection is established to MindSphere.
     * @param [in] store Container for the data to be uploaded to MindSphere.
     * @param [out] reserved Reserved for future use.
     * @return
     * <ul>
     * <li>#MCL_OK in case of success.</li>
     * <li>#MCL_TRIGGERED_WITH_NULL in case any input parameter is NULL.</li>
     * <li>#MCL_OUT_OF_MEMORY in case there is not enough memory in the system to proceed.</li>
     * <li>#MCL_NOT_INITIALIZED in case @p communication is not initialized.</li>
     * <li>#MCL_NOT_ONBOARDED if the agent is not onboarded using @p communication.</li>
     * <li>#MCL_NO_ACCESS_TOKEN_EXISTS if @p communication does not have an access token.</li>
     * <li>#MCL_STORE_IS_EMPTY if @p store has no data.
     * <li>#MCL_COULD_NOT_RESOLVE_PROXY in case the proxy host name can not be resolved.</li>
     * <li>#MCL_COULD_NOT_RESOLVE_HOST in case the remote host name can not be resolved.</li>
     * <li>#MCL_SSL_HANDSHAKE_FAIL in case a problem occurs during SSL handshake.</li>
     * <li>#MCL_COULD_NOT_CONNECT in case there is a connection error.</li>
     * <li>#MCL_SERVER_CERTIFICATE_NOT_VERIFIED in case the server can not be verified.</li>
     * <li>#MCL_IMPROPER_CERTIFICATE in case the certificate provided is in improper format and it can not be parsed.</li>
     * <li>#MCL_CA_CERTIFICATE_AUTHENTICATION_FAIL in case the server certificate can not be authenticated by the root certificate.</li>
     * <li>#MCL_NETWORK_SEND_FAIL in case of an error in sending data to network.</li>
     * <li>#MCL_NETWORK_RECEIVE_FAIL in case of an error in receiving data from the network.</li>
     * <li>#MCL_UNAUTHORIZED if response status code of server is related to authorization.</li>
     * <li>#MCL_SERVER_FAIL if the server returns 500 response status code.</li>
     * <li>#MCL_FAIL in case of an internal error in MCL.</li>
     * </ul>
     */
    extern MCL_EXPORT E_MCL_ERROR_CODE mcl_communication_exchange(mcl_communication_t *communication, mcl_store_t *store, void **reserved);

	/**
	* This function exchanges data in @p store to MindSphere similar to #mcl_communication_exchange but additionally, 
	* it performs a series of steps, such as get access token and key rotation if exchange fails.
	*
	* @param [in] communication Preinitialized @c mcl_communication_t object through which a connection is established to MindSphere.
	* @param [in] store Container for the data to be uploaded to MindSphere.
    * @param [out] reserved Reserved for future use.
	* @return
	* <ul>
	* <li>#MCL_OK in case of success.</li>
	* <li>#MCL_TRIGGERED_WITH_NULL in case any input parameter is NULL.</li>
	* <li>#MCL_OUT_OF_MEMORY in case there is not enough memory in the system to proceed.</li>
	* <li>#MCL_NOT_INITIALIZED in case @p communication is not initialized.</li>
	* <li>#MCL_NOT_ONBOARDED if the agent is not onboarded using @p communication.</li>
	* <li>#MCL_NO_ACCESS_TOKEN_EXISTS if @p communication does not have an access token.</li>
	* <li>#MCL_STORE_IS_EMPTY if @p store has no data.</li>
	* <li>#MCL_COULD_NOT_RESOLVE_PROXY in case the proxy host name can not be resolved.</li>
	* <li>#MCL_COULD_NOT_RESOLVE_HOST in case the remote host name can not be resolved.</li>
	* <li>#MCL_SSL_HANDSHAKE_FAIL in case a problem occurs during SSL handshake.</li>
	* <li>#MCL_COULD_NOT_CONNECT in case there is a connection error.</li>
	* <li>#MCL_SERVER_CERTIFICATE_NOT_VERIFIED in case the server can not be verified.</li>
	* <li>#MCL_IMPROPER_CERTIFICATE in case the certificate provided is in improper format and it can not be parsed.</li>
	* <li>#MCL_CA_CERTIFICATE_AUTHENTICATION_FAIL in case the server certificate can not be authenticated by the root certificate.</li>
	* <li>#MCL_NETWORK_SEND_FAIL in case of an error in sending data to network.</li>
	* <li>#MCL_NETWORK_RECEIVE_FAIL in case of an error in receiving data from the network.</li>
	* <li>#MCL_UNAUTHORIZED if response status code of server is related to authorization.</li>
	* <li>#MCL_SERVER_FAIL if the server returns 500 response status code.</li>
	* <li>#MCL_FAIL in case of an internal error in MCL.</li>
	* </ul>
	*/
	extern MCL_EXPORT E_MCL_ERROR_CODE mcl_communication_process(mcl_communication_t *communication, mcl_store_t *store, void **reserved);

	/**
	* @brief Can be used to determine if @p communication handle is initialized.
	*
	* @param [in] communication The communication handle on which the initialization check will be performed.
	* @return MCL_TRUE if @p communication is initialized. Otherwise MCL_FALSE.
	*/
	extern MCL_EXPORT mcl_bool_t mcl_communication_is_initialized(mcl_communication_t *communication);

	/**
	* @brief Can be used to determine if @p communication handle is onboarded.
	*
	* @param [in] communication The communication handle on which the onboarding check will be performed.
	* @return MCL_TRUE if @p communication is onboarded. Otherwise MCL_FALSE.
	*/
	extern MCL_EXPORT mcl_bool_t mcl_communication_is_onboarded(mcl_communication_t *communication);

	/**
	* @brief Returns the lastly received oauth access token, NULL if server time is not received.
	*
	* @param [in] communication Preinitialized @c mcl_communication_t object through which a connection is established to MindSphere.
	* @param [out] token The lastly received oauth access token.
	* @return
	* <ul>
	* <li>#MCL_OK in case of success.</li>
	* <li>#MCL_OUT_OF_MEMORY if there is not enough memory in the system to proceed.</li>
	* <li>#MCL_NO_ACCESS_TOKEN_EXISTS if @p communication does not have an access token.</li>
	* <li>#MCL_FAIL in case of an internal error in MCL.</li>
	* </ul>
	*/
	extern MCL_EXPORT E_MCL_ERROR_CODE mcl_communication_get_last_access_token(mcl_communication_t *communication, char **token);

	/**
	* @brief Returns the time when lastly received oauth access token is received.
	*
	* @param [in] communication Preinitialized @c mcl_communication_t object through which a connection is established to MindSphere.
	* @param [out] token_time The epoch time when lastly received oauth access token is received.
	* @return
	* <ul>
	* <li>#MCL_OK in case of success.</li>
	* <li>#MCL_OUT_OF_MEMORY if there is not enough memory in the system to proceed.</li>
    * <li>#MCL_NO_SERVER_TIME if server time is not received.</li>
	* <li>#MCL_FAIL in case of an internal error in MCL.</li>
	* </ul>
	*/
	extern MCL_EXPORT E_MCL_ERROR_CODE mcl_communication_get_last_token_time(mcl_communication_t *communication, char **token_time);

#ifdef  __cplusplus
}
#endif

#endif //MCL_COMMUNICATION_H_
