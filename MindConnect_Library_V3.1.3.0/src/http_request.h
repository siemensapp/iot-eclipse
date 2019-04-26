/*!**********************************************************************
 *
 * @copyright Copyright (C) 2016 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 *
 *************************************************************************
 *
 * @file     http_request.h
 * @date     Jun 28, 2016
 * @brief    HTTP request module header file.
 *
 * HTTP request messages are built with this module. It has the abilities like adding headers and adding boundaries
 * automatically when inserting a single or tuple. Manages the multipart/mixed message structure.
 *
 ************************************************************************/

#ifndef HTTP_REQUEST_H_
#define HTTP_REQUEST_H_

#include "http_definitions.h"
#include "string_array.h"

// "--".
#define BOUNDARY_SIGN_LENGTH 2
#define BOUNDARY_SIGN "--"

// ": ".
#define COLON_FORMAT_LENGTH 2

// When boundary length is changed, test should be updated with the new length. Otherwise length test fails.
#define BOUNDARY_LENGTH 22
#define BOUNDARY_CHARACTER_COUNT 62

#define GROWTH_FACTOR 1.5

// Length of the string "Content-Type: multipart/related;boundary=5c6d7e29ef6868d0eb73ck" + "\r\n".
#define CONTENT_TYPE_LINE_LENGTH  65

// Length of the string "Content-Type: "
#define CONTENT_TYPE_HEADER_LENGTH 14

// Length of the string "Content-ID: "
#define CONTENT_ID_HEADER_LENGTH 12

// Length of "\r\n".
#define NEW_LINE_LENGTH 2
#define NEW_LINE    "\r\n"

// Both opening and closing boundary line lengths are equal to boundary_line_length.
// opening_boundary = \r\n--5c6d7e29ef6868d0eb73\r\n , closing_boundary = \r\n--5c6d7e29ef6868d0eb73--
#define BOUNDARY_LINE_LENGTH (BOUNDARY_SIGN_LENGTH + BOUNDARY_LENGTH + NEW_LINE_LENGTH)

// For http_request_initialize(), to state resizing of its buffer is enabled or not :
#define HTTP_REQUEST_RESIZE_ENABLED MCL_TRUE
#define HTTP_REQUEST_RESIZE_DISABLED MCL_FALSE

/**
 * @brief HTTP Method Types
 *
 * Used when initializing #http_request_t struct to specifying the http method of the request.
 */
typedef enum E_MCL_HTTP_METHOD
{
    MCL_HTTP_GET,     //!< Http get method.
    MCL_HTTP_HEAD,    //!< Http head method.
    MCL_HTTP_POST,    //!< Http post method.
    MCL_HTTP_PUT,     //!< Http put method.
    MCL_HTTP_DELETE,  //!< Http delete method.
    MCL_HTTP_CONNECT, //!< Http connect method.
    MCL_HTTP_OPTIONS, //!< Http options method.
    MCL_HTTP_TRACE    //!< Http trace method.
} E_MCL_HTTP_METHOD;

/**
 * @brief HTTP Request Handle
 *
 * Used to build http request messages. This structure is also used for #http_client_send() operation.
 */
typedef struct http_request_t
{
    string_array_t *header;           //!< Header of http request.
    mcl_uint8_t *payload;             //!< Payload of http request.
    mcl_size_t payload_size;          //!< Payload size of http request.
    mcl_size_t payload_offset;        //!< Payload offset of http request.
    E_MCL_HTTP_METHOD method;         //!< Http method of http request.
    string_t *uri;                    //!< Uri of http request.
    string_t *boundary;               //!< Boundary of http request.
    mcl_size_t max_http_payload_size; //!< Maximum http payload size of http request.
    mcl_bool_t resize_enabled;        //!< The state or condition of being resizable.
    mcl_bool_t finalized;             //!< The state of http request.
} http_request_t;

typedef mcl_size_t (*payload_copy_callback_t)(void *destination, void *source, mcl_size_t size, void *user_context);

/**
 * @brief HTTP Request Initializer
 *
 * Creates, initializes and returns a #http_request_t. Owner is responsible of calling #http_request_destroy() when the job is done.
 *
 * @param [in] method HTTP method for this request.
 * @param [in] uri HTTP Request URI. Should be started with the root.
 * @param [in] header_size Required memory space for the header string.
 * @param [in] payload_size Required memory space for the payload string.
 * @param [in] resize_enabled Indicates that the http_request should resize its buffer in case necessary. Give this as MCL_FALSE in case the buffer is static.
 * @param [in] user_agent Value of HTTP header User-Agent for this request.
 * @param [in] max_http_payload_size Maximum payload size for one http request.
 * @param [out] http_request The newly initialized HTTP request.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_OUT_OF_MEMORY in case there is not enough memory in the system to proceed.</li>
 * <li>#MCL_FAIL in case of an internal error in MCL.</li>
 * </ul>
 */
E_MCL_ERROR_CODE http_request_initialize(E_MCL_HTTP_METHOD method, string_t *uri, mcl_size_t header_size, mcl_size_t payload_size, mcl_bool_t resize_enabled,
        string_t *user_agent, mcl_size_t max_http_payload_size, http_request_t **http_request);

/**
 * @brief To be used to add an HTTP header to the request with it's value.
 *
 * @param [in] http_request HTTP Request handle to be used.
 * @param [in] header_name Header name.
 * @param [in] header_value Header value.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_OUT_OF_MEMORY in case there is not enough memory in the system to proceed.</li>
 * <li>#MCL_FAIL in case of an internal error in MCL.</li>
 * </ul>
 */
E_MCL_ERROR_CODE http_request_add_header(http_request_t *http_request, string_t *header_name, string_t *header_value);

/**
 * @brief To be used to add a single to the HTTP Request
 *
 * Please refer to the MindConnect API Specification document for the detailed information on Singles.
 *
 * With this function; a new meta section will be added in the HTTP Request message body without opening a multipart/related section
 * and the given meta string will be inserted in this meta section.
 *
 * Note: Content-Type header should be included in the received meta string.
 *
 * @param [in] http_request HTTP Request Handle to be used.
 * @param [in] content_type HTTP Content Type for this single. Sample : "application/vnd.siemens.mindsphere.meta+json".
 * @param [in] content_id A unique random id string containing 36 chars.
 * @param [in] meta_string The pre-generated meta string to add.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_OUT_OF_MEMORY in case there is not enough memory in the system to proceed.</li>
 * <li>#MCL_HTTP_REQUEST_NO_MORE_SPACE in case @p http_request has no more space for adding new content.</li>
 * <li>#MCL_FAIL in case of an internal error in MCL.</li>
 * </ul>
 */
E_MCL_ERROR_CODE http_request_add_single(http_request_t *http_request, string_t *content_type, string_t *content_id, string_t *meta_string);

/**
 * @brief To be used to add a tuple to the HTTP Request
 *
 * Please refer to the MindConnect API Specification document for the detailed information on tuples.
 *
 * With this function; a new multipart/related section will be opened in the HTTP Request message body.
 * The received meta string will be inserted in the fist section and the payload to the second section.
 * The boundary id will be generated for this section and managed.
 *
 * Note: Content-Type header should be included in the received meta and payload strings.
 *
 * @param [in] http_request HTTP Request Handle to be used.
 * @param [in] meta Meta string to be added.
 * @param [in] meta_content_type HTTP Content Type header for meta section of the tuple. Sample : "application/vnd.siemens.mindsphere.meta+json".
 * @param [in] payload_copy_callback Callback function to be used to copy the payload to @p http_request.
 * @param [in] user_context User context pointer to pass to the callback function.
 * @param [in] payload Payload string to be added.
 * @param [in] payload_size Size of the payload string to be added.
 * @param [in] payload_content_type HTTP Content Type header for the payload section of the tuple. Sample : "application/octet-stream ".
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_OUT_OF_MEMORY in case there is not enough memory in the system to proceed.</li>
 * <li>#MCL_HTTP_REQUEST_NO_MORE_SPACE in case @p http_request has no more space for adding new content.</li>
 * <li>#MCL_FAIL in case of an internal error in MCL.</li>
 * </ul>
 */
E_MCL_ERROR_CODE http_request_add_tuple(http_request_t *http_request, string_t *meta, string_t *meta_content_type, payload_copy_callback_t payload_copy_callback,
        void *user_context, void *payload, mcl_size_t payload_size, string_t *payload_content_type);

/**
 * @brief To start a new tuple structure inside the http request body.
 *
 * Caller should pay attention for the status of the http request body. It is the callers responsibility to maintain integrity.
 *
 * @param http_request HTTP Request Handle to be used.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_OUT_OF_MEMORY in case there is not enough memory in the system to proceed.</li>
 * <li>#MCL_HTTP_REQUEST_NO_MORE_SPACE in case @p http_request has no more space for adding new content.</li>
 * <li>#MCL_FAIL in case of an internal error in MCL.</li>
 * </ul>
 */
E_MCL_ERROR_CODE http_request_start_tuple(http_request_t *http_request);

/**
 *
 * @brief To start a sub tuple section inside of the http request body.
 *
 * To accommodate tuple needs two sub sections must be started consequently. One is for meta and one is for payload.
 * It is the callers responsibility to maintain integrity.
 *
 * @param [in] http_request HTTP Request Handle to be used.
 * @param [in] content_type HTTP Content Type header for this section of the tuple. Sample : "application/vnd.siemens.mindsphere.meta+json".
 * @param [in] content_id A unique random id.
 * @param [out] sub_boundary The sub boundary to be used for this tuple section. If a value for this param is not provided ( as NULL ),
 * it will be created and returned to the caller to be used in consecutive calls.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_OUT_OF_MEMORY in case there is not enough memory in the system to proceed.</li>
 * <li>#MCL_HTTP_REQUEST_NO_MORE_SPACE in case @p http_request has no more space for adding new content.</li>
 * <li>#MCL_FAIL in case of an internal error in MCL.</li>
 * </ul>
 */
E_MCL_ERROR_CODE http_request_start_tuple_sub_section(http_request_t *http_request, string_t *content_type, string_t *content_id, string_t **sub_boundary);

/**
 *
 * @brief To end a sub tuple section previously started with @p http_request_start_tuple_sub_section().
 *
 * @param [in] http_request HTTP Request Handle to be used
 * @param [in] sub_boundary The sub boundary to be used for this tuple section.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_OUT_OF_MEMORY in case there is not enough memory in the system to proceed.</li>
 * <li>#MCL_HTTP_REQUEST_NO_MORE_SPACE in case @p http_request has no more space for adding new content.</li>
 * <li>#MCL_FAIL in case of an internal error in MCL.</li>
 * </ul>
 */
E_MCL_ERROR_CODE http_request_end_tuple_sub_section(http_request_t *http_request, string_t *sub_boundary);

/**
 * @brief To add raw data into an http_request.
 *
 * To accommodate server format needs, caller must make sure that the format is suitable. Usually called between start and end tuple sections.
 *
 * @param [in] http_request HTTP Request Handle to be used.
 * @param [in] copy_callback Callback function to be used to copy the payload to @p http_request.
 * @param [in] user_context User context pointer to pass to the callback function.
 * @param [in] data Data to be added. Should be something that the callback can operate on. It will be passed to callback function.
 * @param [in] data_size Size of the data.
 * @param [out] actual_written_size Actually written data size.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_OUT_OF_MEMORY in case there is not enough memory in the system to proceed.</li>
 * <li>#MCL_HTTP_REQUEST_NO_MORE_SPACE in case @p http_request has no more space for adding new content.</li>
 * <li>#MCL_FAIL in case of an internal error in MCL.</li>
 * </ul>
 */
E_MCL_ERROR_CODE http_request_add_raw_data(http_request_t *http_request, payload_copy_callback_t copy_callback, void *user_context, void *data, mcl_size_t data_size,
        mcl_size_t *actual_written_size);

/**
 * @brief To be used to get the available space left in the request buffer in order to add a tuple.
 *
 * Please refer to the MindConnect API Specification document for the detailed information on tuples.
 *
 * @param [in] http_request HTTP Request Handle to be used.
 * @return Returns the free available space after deducting the necessary overhead for the tuple add operation.
 */
mcl_size_t http_request_get_available_space_for_tuple(http_request_t *http_request);

/**
 * @brief To be used to get the available space left in the request buffer in order to add a raw data.
 *
 * Differently than other get available space functions, this one doesn't calculate the overhead.
 *
 * @param [in] http_request HTTP Request Handle to be used.
 * @return Returns the available size.
 */
mcl_size_t http_request_get_available_space_for_raw_data(http_request_t *http_request);

/**
 * @brief To be used to get the available space left in the request buffer in order to add a single.
 *
 * Please refer to the MindConnect API Specification document for the detailed information on singles.
 * Returns the free available space after deducting the necessary overhead for the single add operation.
 *
 * @param [in] http_request HTTP Request Handle to be used.
 * @return Returns the available size.
 */
mcl_size_t http_request_get_available_space_for_single(http_request_t *http_request);

/**
 * @brief Adds closing boundary to the payload and resizes the payload buffer to release unused memory space.
 *
 * @param [in] http_request HTTP Request Handle to be used.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_OUT_OF_MEMORY in case there is not enough memory in the system to proceed.</li>
 * <li>#MCL_HTTP_REQUEST_NO_MORE_SPACE in case @p http_request has no more space for adding new content.</li>
 * <li>#MCL_FAIL in case of an internal error in MCL.</li>
 * </ul>
 */
E_MCL_ERROR_CODE http_request_finalize(http_request_t *http_request);

/**
 * @brief To destroy the HTTP Request Handler.
 *
 * Will release the resources of HTTP Request. After destroy operation, handler shouldn't be used.
 *
 * @param [in] http_request HTTP Request handle to be freed.
 */
void http_request_destroy(http_request_t **http_request);

#endif //HTTP_REQUEST_H_
