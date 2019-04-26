/*!**********************************************************************
 *
 * @copyright Copyright (C) 2016 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 *
 *************************************************************************
 *
 * @file     http_processor.h
 * @date     Jul 19, 2016
 * @brief    HTTP processor module header file.
 *
 * This module has the center responsibility of running logic of MCL.
 * It manages the collection of the data stored in store_t, generation of HTTP request messages, performing
 * send/receive operations and parsing the HTTP response message.
 *
 ************************************************************************/

#ifndef HTTP_PROCESSOR_H_
#define HTTP_PROCESSOR_H_

#include "store.h"
#include "http_client.h"
#include "security_handler.h"
#include "jwt.h"
#include "file.h"
#include "event_list.h"

/**
 *  http processer handle struct
 */
typedef struct http_processor_t
{
    configuration_t *configuration;       //!< Configuration for mcl initialization.
    security_handler_t *security_handler; //!< Security handler.
    http_client_t *http_client;           //!< Http client handler.
} http_processor_t;

typedef struct http_processor_stream_callback_context_t
{
    http_processor_t *http_processor; //!< Http processer handle.
    store_t *store;                   //!< Holds references to data to exchange.
    http_request_t *request;          //!< It is used to build http request messages.
    mcl_uint8_t call_count;           //!< Call count of stream callback.
    E_MCL_ERROR_CODE previous_result; //!< Previous result of callback.
} http_processor_stream_callback_context_t;

typedef struct raw_data_context_t
{
    mcl_size_t total_size; //!< Total size of raw data context.
    mcl_size_t last_index; //!< Last index of raw data context.
} raw_data_context_t;

/**
 * @brief Http Processor Initialize function.
 *
 * Performs creation and initializations of following modules :
 *  - security_handler
 *  - jwt
 *  - http_client
 *
 * @param [in] configuration The configuration parameters to be used for initializing itself and other modules.
 * @param [out] http_processor The newly initialized HTTP processor.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_OUT_OF_MEMORY if there is not enough memory in the system to proceed.</li>
 * <li>#MCL_PATH_NOT_ACCESSIBLE if the store path is not accessible.</li>
 * <li>#MCL_NO_ACCESS_TOKEN_PROVIDED if neither initial access token - initial registration URI pair nor the store path with registration information is provided.
 * <li>#MCL_INITIALIZATION_FAIL if initialization procedure fails due to library malfunction.</li>
 * </ul>
 */
E_MCL_ERROR_CODE http_processor_initialize(configuration_t *configuration, http_processor_t **http_processor);

/**
 * This function is called by @c mcl_communication_onboard() and @c mcl_communication_rotate_key(). @see mcl_communication_onboard and @see mcl_communication_rotate_key for details.
 *
 * @param [in] http_processor HTTP Processor handle to be used.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
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
 * <li>#MCL_SERVER_FAIL if the the server returns 500 response status code.</li>
 * <li>#MCL_REGISTRATION_INFO_IS_NOT_SAVED if registration information can not be saved.</li>
 * <li>#MCL_FAIL in case of an internal error in MCL.</li>
 * </ul>
 */
E_MCL_ERROR_CODE http_processor_register(http_processor_t *http_processor);

/**
 * This function is called by @c mcl_communication_update_security_information(). @see mcl_communication_update_security_information for details.
 *
 * @param [in] http_processor HTTP Processor handle to be used.
 * @return
 * <ul>
 * <li>#MCL_OK in case of successful update.</li>
 * <li>#MCL_SECURITY_UP_TO_DATE if security information of the mcl_communication is already up to date.
 * <li>#MCL_FAIL in case of an internal error in MCL.</li>
 * </ul>
 */
E_MCL_ERROR_CODE http_processor_update_security_information(http_processor_t *http_processor);

/**
 * This function is called by @c mcl_communication_get_access_token(). @see mcl_communication_get_access_token for details.
 *
 * @param [in] http_processor HTTP Processor handle to be used.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
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
 * <li>#MCL_SERVER_FAIL if the response status code of server is 500.</li>
 * <li>#MCL_FAIL in case of an internal error in MCL.</li>
 * </ul>
 */
E_MCL_ERROR_CODE http_processor_get_access_token(http_processor_t *http_processor);

/**
 * @brief Exchange operation logic.
 *
 * Performs exchange operation on MindSphere.
 * After successfully sending the data from the received store object, data will be removed.
 *
 * @param [in] http_processor HTTP Processor handle to be used.
 * @param [in] store The data to be exchanged will be read from this store.
 * @param [out] reserved Reserved for future use.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_OUT_OF_MEMORY if there is not enough memory in the system to proceed.</li>
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
 * <li>#MCL_SERVER_FAIL if the the server returns 500 response status code.</li>
 * <li>#MCL_HTTP_REQUEST_FINALIZE_FAILED if adding the final closing boundary and/or JWT header to request fails.</li>
 * <li>#MCL_FAIL in case of an internal error in MCL.</li>
 * </ul>
 */
E_MCL_ERROR_CODE http_processor_exchange(http_processor_t *http_processor, store_t *store, void **reserved);

/**
 * @brief Exchange operation logic with streaming.
 *
 * Performs exchange operation on MindSphere using the http transfer-encoding method.
 * This method directly writes the data to the request buffer while performing the send operation,
 * this way uses less memory than normal exchange method, hence it is suitable sending large data
 * in resource constrained systems. Also this is the only way to send unknown size data, which represented
 * in MCL as #stream_data_t.
 *
 * @warning The store passed to this method must be a streamable store (#mcl_store_initialize() ) .
 *
 * @param [in] http_processor HTTP Processor handle to be used
 * @param [in] store The data to be exchanged will be read from this store.
 * @param [out] reserved Reserved for future use.
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_OUT_OF_MEMORY if there is not enough memory in the system to proceed.</li>
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
 * <li>#MCL_SERVER_FAIL if the the server returns 500 response status code.</li>
 * <li>#MCL_FAIL in case of an internal error in MCL.</li>
 * </ul>
 */
E_MCL_ERROR_CODE http_processor_stream(http_processor_t *http_processor, store_t *store, void **reserved);

/**
 * @brief To destroy the HTTP Processor Handler.
 *
 * Will release the resources of HTTP Processor. After destroy operation, handler shouldn't be used.
 *
 * @param [in] http_processor HTTP Processor handle to be destroyed.
 */
void http_processor_destroy(http_processor_t **http_processor);

#endif //HTTP_PROCESSOR_H_
