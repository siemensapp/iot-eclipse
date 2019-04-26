/*!**********************************************************************
 *
 * @copyright Copyright (C) 2016 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 *
 *************************************************************************
 *
 * @file     http_client.h
 * @date     Jul 20, 2016
 * @brief    HTTP client module header file.
 *
 * HTTP client is responsible of sending and receiving data through TCP/IP.
 * It uses http_request_t and http_response_t for sending and returning the messages.
 *
 ************************************************************************/

#ifndef HTTP_CLIENT_H_
#define HTTP_CLIENT_H_

#include "http_response.h"
#include "http_request.h"
#include "configuration.h"

#define SSL_CERTIFICATE_TYPE_PEM "PEM"

typedef mcl_size_t (*http_client_read_callback)(void *buffer, mcl_size_t size, mcl_size_t count, void *user_context);

typedef struct http_client_send_callback_info_t
{
    http_client_read_callback read_callback;
    void *user_context;
} http_client_send_callback_info_t;

/**
 * @brief HTTP Client Handler definition. Members are implementation specific and are declared in implementation file.
 */
typedef struct http_client_t http_client_t;

/**
 * @brief HTTP Client initializer.
 *
 * Initializes the underlying implementation (like libcurl) with the given configuration data.
 *
 * @param [in] configuration The configuration data. This is the same configuration data given to #configuration_t when initializing it.
 * @param [out] http_client Handle for the http client initialized.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_OUT_OF_MEMORY in case there is not enough memory in the system to proceed.</li>
 * <li>#MCL_INITIALIZATION_FAIL in case initialization of http client fails.</li>
 * </ul>
 */
E_MCL_ERROR_CODE http_client_initialize(configuration_t *configuration, http_client_t **http_client);

/**
 * @brief Send/Receive function.
 *
 * Using underlying implementation, it sends the given data to the pre-configured destination and returns the response.
 * This function is blocking until response received or timeout occurred.
 *
 * @param [in] http_client HTTP Client Handler.
 * @param [in] http_request HTTP Request object.
 * @param [in] callback_info Struct holding callback information. Refer to its definition @p http_client_send_callback_info_t.
 * @param [out] http_response HTTP Response object.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_OUT_OF_MEMORY in case there is not enough memory in the system to proceed.</li>
 * <li>#MCL_COULD_NOT_RESOLVE_PROXY in case the proxy host name can not be resolved.</li>
 * <li>#MCL_COULD_NOT_RESOLVE_HOST in case the remote host name can not be resolved.</li>
 * <li>#MCL_SSL_HANDSHAKE_FAIL in case a problem occurs during SSL handshake.</li>
 * <li>#MCL_COULD_NOT_CONNECT in case there is a connection error.</li>
 * <li>#MCL_SERVER_CERTIFICATE_NOT_VERIFIED in case the server can not be verified.</li>
 * <li>#MCL_IMPROPER_CERTIFICATE in case the certificate provided is in improper format and it can not be parsed.</li>
 * <li>#MCL_CA_CERTIFICATE_AUTHENTICATION_FAIL in case the server certificate can not be authenticated by the root certificate.</li>
 * <li>#MCL_NETWORK_SEND_FAIL in case of an error in sending data to network.</li>
 * <li>#MCL_NETWORK_RECEIVE_FAIL in case of an error in receiving data from the network.</li>
 * <li>#MCL_FAIL in case of an internal error in MCL.</li>
 * </ul>
 */
E_MCL_ERROR_CODE http_client_send(http_client_t *http_client, http_request_t *http_request, http_client_send_callback_info_t *callback_info, http_response_t **http_response);

/**
 * @brief To destroy the HTTP Client Handler.
 *
 * Will release the resources of HTTP Request. After destroy operation, handler shouldn't be used.
 *
 * @param [in] http_client HTTP Client Handler.
 */
void http_client_destroy(http_client_t **http_client);

/**
 * @brief To get the implementation specific code for returning from callback function in order to terminate the send operation.
 *
 * @return Returns the termination code.
 */
mcl_size_t http_client_get_callback_termination_code();

#endif //HTTP_CLIENT_H_
