/*!**********************************************************************
 *
 * @copyright Copyright (C) 2016 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 *
 *************************************************************************
 *
 * @file     http_request.c
 * @date     Jun 28, 2016
 * @brief    HTTP request module implementation file.
 *
 ************************************************************************/

#include "data_types.h"
#include "http_request.h"
#include "log_util.h"
#include "definitions.h"
#include "memory.h"
#include "random.h"

/**
 * @brief Open and close boundary types.
 */
typedef enum E_MCL_BOUNDARY_TYPE
{
    MCL_CLOSE_BOUNDARY, //!< Closing boundary.
    MCL_OPEN_BOUNDARY   //!< Opening boundary.
} E_MCL_BOUNDARY_TYPE;

char *boundary_characters = "1234567890abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
mcl_size_t MCL_MAX_SIZE = (mcl_size_t)-1;

// 5 BOUNDARY_LINE_LENGTH : 1 Open main boundary, 3 open/close subboundary, 1 close main boundary
// 1 CONTENT_TYPE_LINE_LENGTH : For the multipart/related main content type line
// 1 MCL_NULL_CHAR_SIZE : For string terminating null char
// 2 CONTENT_TYPE_HEADER_LENGTH : For tuple sections "Content-Type: " parts.
// 4 NEW_LINE_LENGTH : 2 for each sub content-type headers.
mcl_size_t OVERHEAD_FOR_TUPLE = (5 * BOUNDARY_LINE_LENGTH) + CONTENT_TYPE_LINE_LENGTH + (2 * CONTENT_TYPE_HEADER_LENGTH) + (4 * NEW_LINE_LENGTH) + MCL_NULL_CHAR_SIZE;

// 2 BOUNDARY_LINE_LENGTH : 1 Open main boundary, 1 close main boundary
// 1 CONTENT_TYPE_LINE_LENGTH : For the multipart/related main content type line
// 1 MCL_NULL_CHAR_SIZE : For string terminating null char
// 1 CONTENT_TYPE_HEADER_LENGTH : For single sections "Content-Type: " part.
// 1 CONTENT_ID_HEADER_LENGTH : For single sections "Content-Id: " part.
// 2 NEW_LINE_LENGTH : 2 for sub content-type header.
mcl_size_t OVERHEAD_FOR_SINGLE = (2 * BOUNDARY_LINE_LENGTH) + CONTENT_TYPE_LINE_LENGTH + CONTENT_TYPE_HEADER_LENGTH + CONTENT_ID_HEADER_LENGTH + (2 * NEW_LINE_LENGTH)
                                 + MCL_NULL_CHAR_SIZE;

// 1 BOUNDARY_LINE_LENGTH : for open main boundary
// 1 MCL_NULL_CHAR_SIZE : For string terminating null char
mcl_size_t OVERHEAD_FOR_BOUNDARY = BOUNDARY_LINE_LENGTH + MCL_NULL_CHAR_SIZE;

// 1 BOUNDARY_LINE_LENGTH : for open subboundary
// 1 CONTENT_TYPE_HEADER_LENGTH : For sub boundary content type header
// 1 MCL_NULL_CHAR_SIZE : For string terminating null char
// 2 NEW_LINE_LENGTH : 1 for content type line, 1 for blank line after that
// 1 CONTENT_TYPE_LINE_LENGTH : For the multipart/related main content type line where the subboundary defined.
mcl_size_t OVERHEAD_FOR_TUPLE_SUBSECTION_WITH_BOUNDARY_DEFINITION = BOUNDARY_LINE_LENGTH + CONTENT_TYPE_HEADER_LENGTH + (2 * NEW_LINE_LENGTH) + CONTENT_TYPE_LINE_LENGTH
                                                                    + MCL_NULL_CHAR_SIZE;

// 1 BOUNDARY_LINE_LENGTH : for open subboundary
// 1 CONTENT_TYPE_HEADER_LENGTH : For sub boundary content type header
// 1 MCL_NULL_CHAR_SIZE : For string terminating null char
// 2 NEW_LINE_LENGTH : 1 for content type line, 1 for blank line after that
mcl_size_t OVERHEAD_FOR_TUPLE_SUBSECTION = BOUNDARY_LINE_LENGTH + CONTENT_TYPE_HEADER_LENGTH + (2 * NEW_LINE_LENGTH) + MCL_NULL_CHAR_SIZE;

// Private Function Prototypes:
static E_MCL_ERROR_CODE _generate_random_boundary(string_t **boundary);
static E_MCL_ERROR_CODE _add_boundary(mcl_uint8_t *payload, mcl_size_t *payload_offset, char *boundary, E_MCL_BOUNDARY_TYPE boundary_type);
static E_MCL_ERROR_CODE _resize_payload_buffer_if_necessary(mcl_size_t required_empty_size, http_request_t *http_request, mcl_bool_t finalize);
static void _add_meta(string_t *meta, http_request_t *http_request, mcl_size_t *payload_offset);
static void _add_payload(http_request_t *http_request, payload_copy_callback_t payload_copy_callback, void *user_context, mcl_uint8_t *payload, mcl_size_t payload_size,
                         mcl_size_t *payload_offset);
static E_MCL_ERROR_CODE _add_content_info(http_request_t *http_request, char *content_info_name, char *content_info_value, mcl_size_t *payload_offset,
        mcl_size_t content_info_line_length, char *sub_boundary);
static void _add_blank_line(http_request_t *http_request, mcl_size_t *payload_offset);
static mcl_size_t _get_available_space(http_request_t *http_request, mcl_size_t overhead);

E_MCL_ERROR_CODE http_request_initialize(E_MCL_HTTP_METHOD method, string_t *uri, mcl_size_t header_size, mcl_size_t payload_size, mcl_bool_t resize_enabled,
        string_t *user_agent, mcl_size_t max_http_payload_size, http_request_t **http_request)
{
    DEBUG_ENTRY("E_MCL_HTTP_METHOD method = <%d>, string_t *uri = <%p>, mcl_size_t header_size = <%u>, mcl_size_t payload_size = <%u>, mcl_bool_t resize_enabled = <%p>, string_t *user_agent = <%p>, mcl_size_t max_http_payload_size = <%u>, http_request_t **http_request = <%p>",
                method, uri, header_size, payload_size, &resize_enabled, user_agent, max_http_payload_size, http_request)

	E_MCL_ERROR_CODE return_code;
    // Create new http_request object.
    MCL_NEW(*http_request);
    ASSERT_CODE_MESSAGE(MCL_NULL != *http_request, MCL_OUT_OF_MEMORY, "Memory can not be allocated for http request.");

    // Makes sure members of http_request_t are null in case of unexpected call to http_request_destroy() within assert macros.
    (*http_request)->resize_enabled = resize_enabled;
    (*http_request)->boundary = MCL_NULL;
    (*http_request)->header = MCL_NULL;
    (*http_request)->payload = MCL_NULL;
    (*http_request)->uri = MCL_NULL;

    // Set request method and size of the payload.
    (*http_request)->method = method;
    (*http_request)->payload_size = payload_size;
    (*http_request)->payload_offset = 0;
    (*http_request)->finalized = MCL_FALSE;
	(*http_request)->max_http_payload_size = max_http_payload_size;

    // Initialize a string array for the request header. User-Agent is for all HTTP requests the same : header_size + 1
    return_code = string_array_initialize(header_size + 1, &((*http_request)->header));
    ASSERT_STATEMENT_CODE_MESSAGE(return_code == MCL_OK, http_request_destroy(http_request), return_code, "String array for http request header can not be initialized.");

    // Add fix HTTP header: User-Agent = MCL/<MCL_VERSION> (<CUSTOM_USER_AGENT>)
    return_code = http_request_add_header(*http_request, &http_header_names[HTTP_HEADER_USER_AGENT], user_agent);
    ASSERT_STATEMENT_CODE_MESSAGE(MCL_OK == return_code, http_request_destroy(http_request), return_code, "Add header to request has been failed for <User-Agent>.");

    // Allocate memory for the payload if it is present.
    if (0 < payload_size)
    {
        (*http_request)->payload = MCL_MALLOC(payload_size);
        ASSERT_STATEMENT_CODE_MESSAGE(MCL_NULL != (*http_request)->payload, http_request_destroy(http_request), MCL_OUT_OF_MEMORY,
                                      "Memory can not be allocated for the payload of http request.");
    }

    // Allocate memory for the request URI.
    return_code = string_initialize(uri, &((*http_request)->uri));
    ASSERT_STATEMENT_CODE_MESSAGE(MCL_OK == return_code, http_request_destroy(http_request), return_code, "Memory can not be allocated for the URI of http request.");

    return_code = _generate_random_boundary(&((*http_request)->boundary));
    ASSERT_STATEMENT_CODE_MESSAGE(MCL_OK == return_code, http_request_destroy(http_request), return_code, "Random boundary generation failed.");

    DEBUG_LEAVE("retVal = <%d>", MCL_OK);
    return MCL_OK;
}

E_MCL_ERROR_CODE http_request_add_header(http_request_t *http_request, string_t *header_name, string_t *header_value)
{
	DEBUG_ENTRY("http_request_t *http_request = <%p>, string_t *header_name = <%p>, string_t *header_value = <%p>", http_request, header_name, header_value)

	E_MCL_ERROR_CODE return_code;
    mcl_size_t header_line_length;
	string_t *header_line = MCL_NULL;
    char *header_format = MCL_NULL;
    char *boundary = MCL_NULL;

    if ((MCL_OK == string_compare(header_name, &http_header_names[HTTP_HEADER_CONTENT_TYPE]))
            && (MCL_OK == string_compare(header_value, &content_type_values[CONTENT_TYPE_MULTIPART_MIXED])))
    {
        // Length of the header line, +26 is for the string ": " + ";boundary=" + ";charset=utf-8".
        header_line_length = header_name->length + header_value->length + http_request->boundary->length + 26;
        header_format = "%s: %s;boundary=%s;charset=utf-8";
        boundary = http_request->boundary->buffer;
    }
    else
    {
        // Length of the header line, +2 is for the string ": ".
        header_line_length = header_name->length + header_value->length + 2;
        header_format = "%s: %s";
    }

    // Initialize a string_t for the header line.
    return_code = string_initialize_new(MCL_NULL, header_line_length, &header_line);
    ASSERT_CODE_MESSAGE(MCL_OK == return_code, return_code, "Memory can not be allocated for the request header line.");

    // Compose the header line.
    return_code = string_util_snprintf(header_line->buffer, header_line_length + 1, header_format, header_name->buffer, header_value->buffer, boundary);
    ASSERT_STATEMENT_CODE_MESSAGE(MCL_OK == return_code, string_destroy(&header_line), return_code, "New header line can not be composed.");

    // Add header line to the array of header lines.
    return_code = string_array_add(http_request->header, header_line, MCL_TRUE);
    ASSERT_STATEMENT_CODE_MESSAGE(MCL_OK == return_code, string_destroy(&header_line), return_code, "New header line can not be added to request header array.");

    DEBUG_LEAVE("retVal = <%d>", MCL_OK);
    return MCL_OK;
}

E_MCL_ERROR_CODE http_request_add_single(http_request_t *http_request, string_t *content_type, string_t *content_id, string_t *meta_string)
{
	DEBUG_ENTRY("http_request_t *http_request = <%p>, string_t *content_type = <%p>, string_t *content_id = <%p>, string_t *meta_string = <%p>", http_request, content_type,
		content_id, meta_string)
	
	E_MCL_ERROR_CODE return_code;
	mcl_size_t payload_offset_local;
    mcl_size_t required_empty_size = OVERHEAD_FOR_SINGLE + content_type->length + content_id->length + meta_string->length + NEW_LINE_LENGTH + MCL_NULL_CHAR_SIZE;

    // Check if the empty space in payload buffer is enough.If not so, resize payload buffer if possible.
    return_code = _resize_payload_buffer_if_necessary(required_empty_size, http_request, MCL_FALSE);
    ASSERT_CODE_MESSAGE(MCL_OK == return_code, return_code, "Inadequate memory in payload buffer for HTTP message.");

    payload_offset_local = http_request->payload_offset;

    // Add opening main boundary.
    return_code = _add_boundary(http_request->payload, &payload_offset_local, http_request->boundary->buffer, MCL_OPEN_BOUNDARY);
    ASSERT_CODE_MESSAGE(MCL_OK == return_code, return_code, "Boundary couldn't be composed.");

    // Add content_type.
    return_code = _add_content_info(http_request, http_header_names[HTTP_HEADER_CONTENT_TYPE].buffer, content_type->buffer, &payload_offset_local,
                                    CONTENT_TYPE_HEADER_LENGTH + content_type->length + NEW_LINE_LENGTH, MCL_NULL);
    ASSERT_CODE_MESSAGE(MCL_OK == return_code, return_code, "content_type couldn't be composed.");

    // Add content_id.
    return_code = _add_content_info(http_request, http_header_names[HTTP_HEADER_CONTENT_ID].buffer, content_id->buffer, &payload_offset_local,
                                    CONTENT_ID_HEADER_LENGTH + content_id->length + NEW_LINE_LENGTH, MCL_NULL);
    ASSERT_CODE_MESSAGE(MCL_OK == return_code, return_code, "content_id couldn't be composed.");

    // Add blank line.
    _add_blank_line(http_request, &payload_offset_local);

    // Add meta.
    _add_meta(meta_string, http_request, &payload_offset_local);

    // If single is added successfully, update payload_offset with local one.
    http_request->payload_offset = payload_offset_local;

    DEBUG_LEAVE("retVal = <%d>", MCL_OK);
    return MCL_OK;
}

E_MCL_ERROR_CODE http_request_add_tuple(http_request_t *http_request, string_t *meta, string_t *meta_content_type, payload_copy_callback_t payload_copy_callback,
        void *user_context, void *payload, mcl_size_t payload_size, string_t *payload_content_type)
{
	DEBUG_ENTRY("http_request_t *http_request = <%p>, string_t *meta = <%p>, string_t *meta_content_type = <%p>, payload_copy_callback_t payload_copy_callback = <%p>, void *user_context = <%p>, void *payload = <%p>, mcl_size_t payload_size = <%u>, string_t *payload_content_type = <%p>",
		http_request, meta, meta_content_type, payload_copy_callback, user_context, payload, payload_size, payload_content_type)

	E_MCL_ERROR_CODE return_code;
	string_t *sub_boundary = MCL_NULL;
	mcl_size_t payload_offset_local;
    mcl_size_t required_empty_size = OVERHEAD_FOR_TUPLE + meta_content_type->length + payload_content_type->length + payload_size + meta->length;

    // Check if the empty space in payload buffer is enough.If not so, resize payload buffer if possible.
    return_code = _resize_payload_buffer_if_necessary(required_empty_size, http_request, MCL_FALSE);
    ASSERT_CODE_MESSAGE(MCL_OK == return_code, return_code, "Inadequate memory in payload buffer for HTTP message.");

    payload_offset_local = http_request->payload_offset;

    // Add opening main boundary.
    return_code = _add_boundary(http_request->payload, &payload_offset_local, http_request->boundary->buffer, MCL_OPEN_BOUNDARY);
    ASSERT_CODE_MESSAGE(MCL_OK == return_code, return_code, "Boundary couldn't be composed.");

    // Create sub_boundary.
    return_code = _generate_random_boundary(&sub_boundary);
    ASSERT_CODE_MESSAGE(MCL_OK == return_code, return_code, "sub_boundary couldn't be composed.");

    // Add tuple content_type.
    return_code = _add_content_info(http_request, http_header_names[HTTP_HEADER_CONTENT_TYPE].buffer, content_type_values[CONTENT_TYPE_MULTIPART_RELATED].buffer,
                                    &payload_offset_local, CONTENT_TYPE_LINE_LENGTH, sub_boundary->buffer);
    ASSERT_STATEMENT_CODE_MESSAGE(MCL_OK == return_code, string_destroy(&sub_boundary), return_code, "content_type couldn't be composed.");

    // Add blank line.
    _add_blank_line(http_request, &payload_offset_local);

    // Add open sub_boundary.
    return_code = _add_boundary(http_request->payload, &payload_offset_local, sub_boundary->buffer, MCL_OPEN_BOUNDARY);
    ASSERT_STATEMENT_CODE_MESSAGE(MCL_OK == return_code, string_destroy(&sub_boundary), return_code, "sub_boundary couldn't be composed.");

    // Add content_type.
    return_code = _add_content_info(http_request, http_header_names[HTTP_HEADER_CONTENT_TYPE].buffer, meta_content_type->buffer, &payload_offset_local,
                                    CONTENT_TYPE_HEADER_LENGTH + meta_content_type->length + NEW_LINE_LENGTH, MCL_NULL);
    ASSERT_STATEMENT_CODE_MESSAGE(MCL_OK == return_code, string_destroy(&sub_boundary), return_code, "content_type couldn't be composed.");

    // Add blank line.
    _add_blank_line(http_request, &payload_offset_local);

    // Add meta.
    _add_meta(meta, http_request, &payload_offset_local);

    // Add open sub_boundary.
    return_code = _add_boundary(http_request->payload, &payload_offset_local, sub_boundary->buffer, MCL_OPEN_BOUNDARY);
    ASSERT_STATEMENT_CODE_MESSAGE(MCL_OK == return_code, string_destroy(&sub_boundary), return_code, "sub_boundary couldn't be composed.");

    // Add payload content-type.
    return_code = _add_content_info(http_request, http_header_names[HTTP_HEADER_CONTENT_TYPE].buffer, payload_content_type->buffer, &payload_offset_local,
                                    CONTENT_TYPE_HEADER_LENGTH + payload_content_type->length + NEW_LINE_LENGTH, MCL_NULL);
    ASSERT_STATEMENT_CODE_MESSAGE(MCL_OK == return_code, string_destroy(&sub_boundary), return_code, "payload content_type couldn't be composed.");

    // Add blank line.
    _add_blank_line(http_request, &payload_offset_local);

    // Add payload.
    _add_payload(http_request, payload_copy_callback, user_context, payload, payload_size, &payload_offset_local);

    // Add blank line.
    _add_blank_line(http_request, &payload_offset_local);

    // Add close sub_boundary.
    return_code = _add_boundary(http_request->payload, &payload_offset_local, sub_boundary->buffer, MCL_CLOSE_BOUNDARY);
    ASSERT_STATEMENT_CODE_MESSAGE(MCL_OK == return_code, string_destroy(&sub_boundary), return_code, "sub_boundary couldn't be composed.");

    // Add memory size to be used to payload_offset.
    http_request->payload_offset = payload_offset_local;

    // Destroy local sub_boundary.
    string_destroy(&sub_boundary);

    DEBUG_LEAVE("retVal = <%d>", MCL_OK);
    return MCL_OK;
}

E_MCL_ERROR_CODE http_request_start_tuple(http_request_t *http_request)
{
	DEBUG_ENTRY("http_request_t *http_request = <%p>", http_request)

	E_MCL_ERROR_CODE return_code;

	// Only 1 open boundary is necessary
    mcl_size_t required_empty_size = OVERHEAD_FOR_BOUNDARY;
	mcl_size_t payload_offset_local;

    // Check if the empty space in payload buffer is enough.If not so, resize payload buffer if possible.
    return_code = _resize_payload_buffer_if_necessary(required_empty_size, http_request, MCL_FALSE);
    ASSERT_CODE_MESSAGE(MCL_OK == return_code, return_code, "Inadequate memory in payload buffer for HTTP message.");

    payload_offset_local = http_request->payload_offset;

    // Add opening main boundary.
    return_code = _add_boundary(http_request->payload, &payload_offset_local, http_request->boundary->buffer, MCL_OPEN_BOUNDARY);
    ASSERT_CODE_MESSAGE(MCL_OK == return_code, return_code, "Boundary couldn't be composed.");

    // Add memory size to be used to payload_offset.
    http_request->payload_offset = payload_offset_local;

    DEBUG_LEAVE("retVal = <%d>", MCL_OK);
    return MCL_OK;
}

E_MCL_ERROR_CODE http_request_start_tuple_sub_section(http_request_t *http_request, string_t *content_type, string_t *content_id, string_t **sub_boundary)
{
	DEBUG_ENTRY("http_request_t *http_request = <%p>, string_t *content_type = <%p>, string_t *content_id = <%p>, string_t **sub_boundary = <%p>", http_request, content_type,
		content_id, sub_boundary)

	E_MCL_ERROR_CODE return_code;
	mcl_size_t payload_offset_local;

    // mcl_size_t content_type_line_length = http_header_names[HTTP_HEADER_CONTENT_TYPE].length + COLON_FORMAT_LENGTH + content_type->length + NEW_LINE_LENGTH;
    mcl_size_t required_empty_size = 0;
    if (MCL_NULL == *sub_boundary)
    {
        // since it is NULL, a new subboundary will be generated and added to the reqeust for multipart/related line :
        required_empty_size = OVERHEAD_FOR_TUPLE_SUBSECTION_WITH_BOUNDARY_DEFINITION + content_type->length;
    }
    else
    {
        required_empty_size = OVERHEAD_FOR_TUPLE_SUBSECTION + content_type->length;
    }

    // Check if the empty space in payload buffer is enough.If not so, resize payload buffer if possible.
    return_code = _resize_payload_buffer_if_necessary(required_empty_size, http_request, MCL_FALSE);
    ASSERT_CODE_MESSAGE(MCL_OK == return_code, return_code, "Inadequate memory in payload buffer for HTTP message.");

    payload_offset_local = http_request->payload_offset;

    // Create sub_boundary if it is not created :
    if (MCL_NULL == *sub_boundary)
    {
        return_code = _generate_random_boundary(sub_boundary);
        ASSERT_CODE_MESSAGE(MCL_OK == return_code, return_code, "sub_boundary couldn't be composed.");

        // Add tuple content_type.
        return_code = _add_content_info(http_request, http_header_names[HTTP_HEADER_CONTENT_TYPE].buffer, content_type_values[CONTENT_TYPE_MULTIPART_RELATED].buffer,
                                        &payload_offset_local, CONTENT_TYPE_LINE_LENGTH, (*sub_boundary)->buffer);
        ASSERT_CODE_MESSAGE(MCL_OK == return_code, return_code, "content_type couldn't be composed.");
    }

    // Add open sub_boundary.
    return_code = _add_boundary(http_request->payload, &payload_offset_local, (*sub_boundary)->buffer, MCL_OPEN_BOUNDARY);
    ASSERT_CODE_MESSAGE(MCL_OK == return_code, return_code, "sub_boundary couldn't be composed.");

    // Add content_type.
    return_code = _add_content_info(http_request, http_header_names[HTTP_HEADER_CONTENT_TYPE].buffer, content_type->buffer, &payload_offset_local,
                                    CONTENT_TYPE_HEADER_LENGTH + content_type->length + NEW_LINE_LENGTH, MCL_NULL);
    ASSERT_CODE_MESSAGE(MCL_OK == return_code, return_code, "content_type couldn't be composed.");

    // Add blank line.
    _add_blank_line(http_request, &payload_offset_local);

    // Add memory size to be used to payload_offset.
    http_request->payload_offset = payload_offset_local;

    DEBUG_LEAVE("retVal = <%d>", MCL_OK);
    return MCL_OK;
}

E_MCL_ERROR_CODE http_request_end_tuple_sub_section(http_request_t *http_request, string_t *sub_boundary)
{
	DEBUG_ENTRY("http_request_t *http_request = <%p>, string_t **sub_boundary = <%p>", http_request, sub_boundary)

	E_MCL_ERROR_CODE return_code;
	mcl_size_t payload_offset_local;
    mcl_size_t required_empty_size = BOUNDARY_LINE_LENGTH + MCL_NULL_CHAR_SIZE;

    // Check if the empty space in payload buffer is enough. If not so, resize payload buffer if possible.
    return_code = _resize_payload_buffer_if_necessary(required_empty_size, http_request, MCL_FALSE);
    ASSERT_CODE_MESSAGE(MCL_OK == return_code, return_code, "Inadequate memory in payload buffer for HTTP message.");

    payload_offset_local = http_request->payload_offset;

    // Add close sub_boundary.
    return_code = _add_boundary(http_request->payload, &payload_offset_local, sub_boundary->buffer, MCL_CLOSE_BOUNDARY);
    ASSERT_CODE_MESSAGE(MCL_OK == return_code, return_code, "sub_boundary couldn't be composed.");

    // Add memory size to be used to payload_offset.
    http_request->payload_offset = payload_offset_local;

    DEBUG_LEAVE("retVal = <%d>", MCL_OK);
    return MCL_OK;
}

E_MCL_ERROR_CODE http_request_add_raw_data(http_request_t *http_request, payload_copy_callback_t copy_callback, void *user_context, void *data, mcl_size_t data_size,
        mcl_size_t *actual_written_size)
{
	DEBUG_ENTRY("http_request_t *http_request = <%p>, payload_copy_callback_t copy_callback = <%p>, void *user_context = <%p>, void *data = <%p>, mcl_size_t data_size = <%u>, mcl_size_t *actual_written_size = <%p>",
		http_request, copy_callback, user_context, data, data_size, actual_written_size)

	E_MCL_ERROR_CODE return_code;
	mcl_size_t payload_offset_local;

    // Check if the empty space in payload buffer is enough.If not so, resize payload buffer if possible.
    return_code = _resize_payload_buffer_if_necessary(data_size, http_request, MCL_FALSE);
    ASSERT_CODE_MESSAGE(MCL_OK == return_code, return_code, "Inadequate memory in payload buffer for HTTP message.");

    payload_offset_local = http_request->payload_offset;

    // Add data.
    _add_payload(http_request, copy_callback, user_context, (mcl_uint8_t *)data, data_size, &payload_offset_local);

    // save actually written count if requested:
    if (MCL_NULL != actual_written_size)
    {
        *actual_written_size = payload_offset_local - http_request->payload_offset;
    }

    // Add memory size to be used to payload_offset.
    http_request->payload_offset = payload_offset_local;

    DEBUG_LEAVE("retVal = <%d>", MCL_OK);
    return MCL_OK;
}

mcl_size_t http_request_get_available_space_for_raw_data(http_request_t *http_request)
{
    DEBUG_ENTRY("http_request_t *http_request = <%p>", http_request)

    mcl_size_t empty_size = _get_available_space(http_request, 0);

    DEBUG_LEAVE("retVal = <%u>", empty_size);
    return empty_size;
}

mcl_size_t http_request_get_available_space_for_tuple(http_request_t *http_request)
{
    DEBUG_ENTRY("http_request_t *http_request = <%p>", http_request)

    mcl_size_t empty_size = _get_available_space(http_request, OVERHEAD_FOR_TUPLE);

    DEBUG_LEAVE("retVal = <%u>", empty_size);
    return empty_size;
}

mcl_size_t http_request_get_available_space_for_single(http_request_t *http_request)
{
    DEBUG_ENTRY("http_request_t *http_request = <%p>", http_request)

    // 5 is main and tuples opening and closing boudary count:
    mcl_size_t empty_size = _get_available_space(http_request, OVERHEAD_FOR_SINGLE);

    DEBUG_LEAVE("retVal = <%u>", empty_size);
    return empty_size;
}

E_MCL_ERROR_CODE http_request_finalize(http_request_t *http_request)
{
	DEBUG_ENTRY("http_request_t *http_request = <%p>", http_request)

	E_MCL_ERROR_CODE return_code;
    mcl_size_t required_empty_size = 2 * BOUNDARY_SIGN_LENGTH + BOUNDARY_LENGTH;

    // Check if the empty space in payload buffer is enough.If not so, resize only as much as required.
    // If there is more memory space unused, release and shrink the payload buffer.
    return_code = _resize_payload_buffer_if_necessary(required_empty_size, http_request, MCL_TRUE);
    ASSERT_CODE_MESSAGE(MCL_OK == return_code, MCL_HTTP_REQUEST_FINALIZE_FAILED, "Inadequate memory in payload buffer for HTTP message.");

    // Add front sign of closing main boundary.
    string_util_memcpy(&http_request->payload[http_request->payload_offset], BOUNDARY_SIGN, BOUNDARY_SIGN_LENGTH);
    http_request->payload_offset += BOUNDARY_SIGN_LENGTH;

    // Add closing main boundary.
    string_util_memcpy(&http_request->payload[http_request->payload_offset], http_request->boundary->buffer, BOUNDARY_LENGTH);
    http_request->payload_offset += BOUNDARY_LENGTH;

    // Add back sign of closing main boundary.
    string_util_memcpy(&http_request->payload[http_request->payload_offset], BOUNDARY_SIGN, BOUNDARY_SIGN_LENGTH);
    http_request->payload_offset += BOUNDARY_SIGN_LENGTH;

    DEBUG_LEAVE("retVal = <%d>", MCL_OK);
    return MCL_OK;
}

void http_request_destroy(http_request_t **http_request)
{
    DEBUG_ENTRY("http_request_t **http_request = <%p>", http_request)

    if (MCL_NULL != *http_request)
    {
        // Free http request with its members.
        string_array_destroy(&((*http_request)->header));
        string_destroy(&((*http_request)->uri));
        string_destroy(&((*http_request)->boundary));
        MCL_FREE((*http_request)->payload);
        MCL_FREE(*http_request);

        MCL_DEBUG("Http request is destroyed successfully.");
    }
    else
    {
        MCL_DEBUG("Http request is already NULL.");
    }

    DEBUG_LEAVE("retVal = void");
}

static mcl_size_t _get_available_space(http_request_t *http_request, mcl_size_t overhead)
{
    mcl_size_t limit = (http_request->resize_enabled) ? http_request->max_http_payload_size : http_request->payload_size;

    // We do following check first to make sure the calculation is ok.
    // It is for the case : payload_offset + overhead is larger than the MAX_SIZE
    // and looped to a small value since it will exceeds the mcl_size_t limit.

    // MCL_MAX_SIZE is always greater than offset. Can be safely written :
    if ((MCL_MAX_SIZE - http_request->payload_offset > overhead) && (limit > http_request->payload_offset + overhead))
    {
        // since we're sure now that the overhead+offset is less than the limit, we can extract it from the limit to calculate the empty size :
        return limit - http_request->payload_offset - overhead;
    }

    // else, it means there is no space.
    return 0;
}

static E_MCL_ERROR_CODE _generate_random_boundary(string_t **boundary)
{
	DEBUG_ENTRY("string_t **boundary = <%p>", boundary)

	E_MCL_ERROR_CODE code;
	mcl_size_t index;
    code = string_initialize_new(MCL_NULL, BOUNDARY_LENGTH, boundary);
    ASSERT_CODE_MESSAGE(MCL_OK == code, code, "String initialize fail for boundary.");

    // Take (BOUNDARY_LENGTH) random characters from boundary_characters[] array and fill boundary buffer.
    for (index = 0; index < BOUNDARY_LENGTH; index++)
    {
        mcl_uint32_t random_index = 0;
        code = random_generate_number(&random_index);
        if (MCL_OK == code)
        {
            (*boundary)->buffer[index] = boundary_characters[random_index % BOUNDARY_CHARACTER_COUNT];
        }
        else
        {
            string_destroy(boundary);
            break;
        }
    }

    DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

static E_MCL_ERROR_CODE _add_boundary(mcl_uint8_t *payload, mcl_size_t *payload_offset, char *boundary, E_MCL_BOUNDARY_TYPE boundary_type)
{
	DEBUG_ENTRY("mcl_uint8_t *payload = <%p>, mcl_size_t *payload_offset = <%p>, char *boundary = <%s>, E_MCL_BOUNDARY_TYPE boundary_type = <%d>", payload, payload_offset,
		boundary, boundary_type)

	E_MCL_ERROR_CODE return_code;
    char *boundary_format = MCL_NULL;
    mcl_size_t boundary_line_total_length;

    // Boundary format selection for either open or close.
    if (MCL_OPEN_BOUNDARY == boundary_type)
    {
        boundary_format = "--%s\r\n";
        boundary_line_total_length = BOUNDARY_LINE_LENGTH;
    }
    else
    {
        boundary_format = "--%s--\r\n";
        boundary_line_total_length = BOUNDARY_LINE_LENGTH + BOUNDARY_SIGN_LENGTH;
    }

    return_code = string_util_snprintf((char *)&payload[*payload_offset], boundary_line_total_length + MCL_NULL_CHAR_SIZE, boundary_format, boundary);
    ASSERT_CODE_MESSAGE(MCL_OK == return_code, return_code, "Boundary couldn't be composed.");
    *payload_offset += boundary_line_total_length;

    DEBUG_LEAVE("retVal = <%d>", MCL_OK);
    return MCL_OK;
}

static E_MCL_ERROR_CODE _resize_payload_buffer_if_necessary(mcl_size_t required_empty_size, http_request_t *http_request, mcl_bool_t finalize)
{
    DEBUG_ENTRY("mcl_size_t required_empty_size = <%u>, http_request_t *http_request = <%p>, mcl_bool_t finalize = <%d>", required_empty_size, http_request, finalize)

    // Maximum allowed available size can be used after extending the payload buffer up to MAX_PAYLOAD_SIZE.
    mcl_size_t max_allowed_available_size = 0;
    if (MCL_TRUE == http_request->resize_enabled)
    {
        max_allowed_available_size = http_request->max_http_payload_size - http_request->payload_offset;
    }
    else
    {
        // since resize is not available, available size calculation must be done on current payload size :
        max_allowed_available_size = http_request->payload_size - http_request->payload_offset;
    }

    if (MCL_TRUE == finalize)
    {
        // If we need extra memory space, extend payload buffer and If there is more space unused, then shrink the payload size.
        // else return no more space error.
        if (required_empty_size <= max_allowed_available_size)
        {
            if (MCL_TRUE == http_request->resize_enabled)
            {
                MCL_RESIZE(http_request->payload, http_request->payload_offset + required_empty_size);
                ASSERT_CODE_MESSAGE(MCL_NULL != http_request->payload, MCL_OUT_OF_MEMORY, "http_request->payload couldn't be resized as required_empty_size!");
                http_request->payload_size = http_request->payload_offset + required_empty_size;
            }
            else
            {
                MCL_DEBUG("Resizing is disabled. Not shrinking extra payload space.");
            }
        }
        else
        {
            // if the empty space is even lower than the required_empty_size, return error code.
            MCL_ERROR_RETURN(MCL_HTTP_REQUEST_NO_MORE_SPACE, "Inadequate memory in payload buffer for HTTP message.");
        }
    }
    else
    {
        // Current available size.
        mcl_size_t available_size = http_request->payload_size - http_request->payload_offset;

        // Check if any resize operation is necessary (required_empty_size > available_size).
        if (required_empty_size > available_size)
        {
            // +1 is to round floating number to a next upper integer.
            mcl_size_t new_size = 0;
            if (http_request->payload_size == 0)
            {
                new_size = (mcl_size_t)(((1.0 * required_empty_size) * GROWTH_FACTOR) + 1.0);
            }
            else
            {
                new_size = (mcl_size_t)(((1.0 * required_empty_size / http_request->payload_size + 1.0) * GROWTH_FACTOR) * http_request->payload_size);
            }

            if ((new_size <= http_request->max_http_payload_size) && http_request->resize_enabled)
            {
                MCL_RESIZE(http_request->payload, new_size);
                ASSERT_CODE_MESSAGE(MCL_NULL != http_request->payload, MCL_OUT_OF_MEMORY, "http_request->payload couldn't be resized as new_size!");
                http_request->payload_size = new_size;
            }
            else if ((required_empty_size <= max_allowed_available_size) && http_request->resize_enabled)
            {
                // if new_size is higher than MAX_PAYLOAD_SIZE, check if the empty space is enough for only required_empty_size, than resize with MAX_PAYLOAD_SIZE.
                MCL_RESIZE(http_request->payload, http_request->max_http_payload_size);
                ASSERT_CODE_MESSAGE(MCL_NULL != http_request->payload, MCL_OUT_OF_MEMORY, "http_request->payload couldn't be resized as MAX_PAYLOAD_SIZE!");
                http_request->payload_size = http_request->max_http_payload_size;
            }
            else
            {
                // if the empty space is even lower than the required_empty_size, return error code.
                MCL_ERROR_RETURN(MCL_HTTP_REQUEST_NO_MORE_SPACE, "Inadequate memory in payload buffer for HTTP message.");
            }
        }
    }

    DEBUG_LEAVE("retVal = <%d>", MCL_OK);
    return MCL_OK;
}

static void _add_meta(string_t *meta, http_request_t *http_request, mcl_size_t *payload_offset)
{
    DEBUG_ENTRY("string_t *meta = <%p>, http_request_t *http_request = <%p>, mcl_size_t *payload_offset = <%p>", meta, http_request, payload_offset)

    // Compose meta_string.
    string_util_memcpy(&http_request->payload[*payload_offset], (mcl_uint8_t *)meta->buffer, meta->length);
    *payload_offset += meta->length;

    // Add blank line.
    _add_blank_line(http_request, payload_offset);

    DEBUG_LEAVE("retVal = void");
}

static void _add_payload(http_request_t *http_request, payload_copy_callback_t payload_copy_callback, void *user_context, mcl_uint8_t *payload, mcl_size_t payload_size,
                         mcl_size_t *payload_offset)
{
    DEBUG_ENTRY("http_request_t *http_request = <%p>, payload_copy_callback_t payload_copy_callback = <%p>, void *user_context = <%p>, mcl_uint8_t *payload = <%p>, mcl_size_t payload_size = <%u>, mcl_size_t *payload_offset = <%p>",
                http_request, payload_copy_callback, user_context, payload, payload_size, payload_offset)

    // Compose payload.
    mcl_size_t count = payload_copy_callback(&http_request->payload[*payload_offset], payload, payload_size, user_context);

    // Count might be different than the payload_size we requested:
    *payload_offset += count;

    DEBUG_LEAVE("retVal = void");
}

static E_MCL_ERROR_CODE _add_content_info(http_request_t *http_request, char *content_info_name, char *content_info_value, mcl_size_t *payload_offset,
        mcl_size_t content_info_line_length, char *sub_boundary)
{
    DEBUG_ENTRY("http_request_t *http_request = <%p>, char *content_info_name = <%s>, char *content_info_value = <%s>, mcl_size_t *payload_offset = <%p>, mcl_size_t content_info_line_length = <%u>, char *sub_boundary = <%s>",
                http_request, content_info_name, content_info_value, payload_offset, content_info_line_length, sub_boundary)

	E_MCL_ERROR_CODE return_code;
    char *content_info_format = MCL_NULL;

    if (MCL_NULL == sub_boundary)
    {
        // Meta and payload format.
        content_info_format = "%s: %s\r\n";
    }
    else
    {
        // Multipart format.
        content_info_format = "%s: %s;boundary=%s\r\n";
    }

    return_code = string_util_snprintf((char *)&http_request->payload[*payload_offset], content_info_line_length + MCL_NULL_CHAR_SIZE, (const char *)content_info_format,
                                       content_info_name, content_info_value, sub_boundary);
    ASSERT_CODE_MESSAGE(MCL_OK == return_code, return_code, "Content info couldn't be composed.");
    *payload_offset += content_info_line_length;

    DEBUG_LEAVE("retVal = <%d>", MCL_OK);
    return MCL_OK;
}

static void _add_blank_line(http_request_t *http_request, mcl_size_t *payload_offset)
{
    DEBUG_ENTRY("http_request_t *http_request = <%p>, mcl_size_t *payload_offset = <%p>", http_request, payload_offset)

    // Compose blank line.
    string_util_memcpy(&http_request->payload[*payload_offset], NEW_LINE, NEW_LINE_LENGTH);
    *payload_offset += NEW_LINE_LENGTH;
    http_request->payload[*payload_offset] = MCL_NULL_CHAR;

    DEBUG_LEAVE("retVal = void");
}
