/*!**********************************************************************
 *
 * @copyright Copyright (C) 2016 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 *
 *************************************************************************
 *
 * @file     http_response.h
 * @date     Jul 19, 2016
 * @brief    HTTP response module header file.
 *
 * This module analyzes and parses received HTTP response messages.
 *
 ************************************************************************/

#ifndef HTTP_RESPONSE_H_
#define HTTP_RESPONSE_H_

#include "string_array.h"

/**
 * @brief HTTP Result Codes
 *
 * Only most used result codes are defined. For non existing ones, the general group names will be used ( e.g. MCL_HTTP_RESULT_CODE_GENERAL_REDIRECTION_3XX )
 */
typedef enum E_MCL_HTTP_RESULT_CODE
{
    MCL_HTTP_RESULT_CODE_UNKNOWN = 0,                     //!< Unknown result code
    MCL_HTTP_RESULT_CODE_GENERAL_INFORMATION_1XX = 1,     //!< General Group Name for 1xx Error Codes
    MCL_HTTP_RESULT_CODE_CONTINUE = 100,                  //!< Actual code : 100
    MCL_HTTP_RESULT_CODE_GENERAL_SUCCESSFUL_2XX = 2,      //!< General Group Name for 2xx Error Codes
    MCL_HTTP_RESULT_CODE_SUCCESS = 200,                   //!< Actual code : 200
    MCL_HTTP_RESULT_CODE_CREATED = 201,                   //!< Actual code : 201
    MCL_HTTP_RESULT_CODE_PARTIAL_CONTENT = 206,           //!< Actual code : 206
    MCL_HTTP_RESULT_CODE_GENERAL_REDIRECTION_3XX = 3,     //!< General Group Name for 3xx Error Codes
    MCL_HTTP_RESULT_CODE_MOVED_PERMANENTLY = 301,         //!< Actual code : 301
    MCL_HTTP_RESULT_CODE_USE_PROXY = 305,                 //!< Actual code : 305
    MCL_HTTP_RESULT_CODE_GENERAL_CLIENT_ERROR_4XX = 4,    //!< General Group Name for 4xx Error Codes
    MCL_HTTP_RESULT_CODE_BAD_REQUEST = 400,               //!< Actual code : 400
    MCL_HTTP_RESULT_CODE_UNAUTHORIZED = 401,              //!< Actual code : 401
    MCL_HTTP_RESULT_CODE_FORBIDDEN = 403,                 //!< Actual code : 403
    MCL_HTTP_RESULT_CODE_NOT_FOUND = 404,                 //!< Actual code : 404
    MCL_HTTP_RESULT_CODE_METHOD_NOT_ALLOWED = 405,        //!< Actual code : 405
    MCL_HTTP_RESULT_CODE_PROXY_AUTH_REQUIRED = 407,       //!< Actual code : 407
    MCL_HTTP_RESULT_CODE_REQUEST_TIMEOUT = 408,           //!< Actual code : 408
    MCL_HTTP_RESULT_CODE_CONFLICT = 409,                  //!< Actual code : 409
    MCL_HTTP_RESULT_CODE_PRECONDITION_FAILED = 412,       //!< Actual code : 412
    MCL_HTTP_RESULT_CODE_PAYLOAD_TOO_LARGE = 413,		  //!< Actual code : 413
    MCL_HTTP_RESULT_CODE_GENERAL_SERVER_ERROR_5XX = 5,    //!< General Group Name for 5xx Error Codes
    MCL_HTTP_RESULT_CODE_INTERNAL_SERVER_ERR = 500,       //!< Actual code : 500
    MCL_HTTP_RESULT_CODE_NOT_IMPLEMENTED = 501,           //!< Actual code : 501
    MCL_HTTP_RESULT_CODE_BAD_GATEWAY = 502,               //!< Actual code : 502
    MCL_HTTP_RESULT_CODE_SERVICE_UNAVAILABLE = 503,       //!< Actual code : 503
    MCL_HTTP_RESULT_CODE_GATEWAY_TIMEOUT = 504,           //!< Actual code : 504
    MCL_HTTP_RESULT_CODE_HTTP_VERSION_NOT_SUPPORTED = 505 //!< Actual code : 505
} E_MCL_HTTP_RESULT_CODE;

/**
 * @brief HTTP Response Handle
 *
 * Contains HTTP Result code, headers and payload.
 */
typedef struct http_response_t
{
    string_array_t *header;             //!< Header of http response.
    mcl_uint8_t *payload;               //!< Payload of http response.
    mcl_size_t payload_size;            //!< Payload size of http response.
    E_MCL_HTTP_RESULT_CODE result_code; //!< Result code of http response.
} http_response_t;

/**
 * @brief HTTP Response Module Initialize function.
 *
 * Create and initializes an #http_response_t object and sets its result code if not received.
 *
 * @param [in] header String array for the received HTTP Header.
 * @param [in] payload The received HTTP Payload.
 * @param [in] payload_size Size of @p payload.
 * @param [in] result_code The received status code. If this is 0, #http_response_initialize will extract the result code from the message itself.
 * @param [out] http_response Handle of the initialized #http_response_t object.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_OUT_OF_MEMORY in case there is not enough memory in the system to proceed.</li>
 * </ul>
 */
E_MCL_ERROR_CODE http_response_initialize(string_array_t *header, mcl_uint8_t *payload, mcl_size_t payload_size, E_MCL_HTTP_RESULT_CODE result_code, http_response_t **http_response);

/**
 * @brief Get the value of a specified HTTP Header.
 *
 * @param [in]  http_response HTTP Response handle to be used.
 * @param [in]  header_name Name of the header whose value is requested.
 * @param [out] header_value Valur of the header will be stored in @p header_value.
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_OUT_OF_MEMORY in case there is not enough memory in the system to proceed.</li>
 * <li>#MCL_FAIL If header string has invalid format.</li>
 * </ul>
 */
E_MCL_ERROR_CODE http_response_get_header(http_response_t *http_response, char *header_name, string_t **header_value);

/**
 * @brief Get the payload section of the HTTP Response.
 *
 * @param [in] http_response HTTP Response handle to be used.
 * @return Returns the pointer to the payload of @p http_response.
 */
mcl_uint8_t *http_response_get_payload(http_response_t *http_response);

/**
 * @brief Get the result code of the HTTTP Response.
 *
 * @param [in] http_response HTTP Response handle to be used.
 * @return HTTP Result Code.
 */
E_MCL_HTTP_RESULT_CODE http_response_get_result_code(http_response_t *http_response);

/**
 * @brief To destroy the HTTP Response Handler.
 *
 * Will release the resources of HTTP Response Handler. After destroy operation, handler shouldn't be used.
 *
 * @param [in] http_response HTTP Response Handle.
 */
void http_response_destroy(http_response_t **http_response);

#endif //HTTP_RESPONSE_H_
