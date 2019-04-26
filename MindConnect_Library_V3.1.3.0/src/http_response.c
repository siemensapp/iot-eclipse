/*!**********************************************************************
 *
 * @copyright Copyright (C) 2016 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 *
 *************************************************************************
 *
 * @file     http_response.c
 * @date     Jul 19, 2016
 * @brief    HTTP response module implementation file.
 *
 ************************************************************************/

#include "http_response.h"
#include "definitions.h"
#include "memory.h"
#include "log_util.h"

E_MCL_ERROR_CODE http_response_initialize(string_array_t *header, mcl_uint8_t *payload, mcl_size_t payload_size, E_MCL_HTTP_RESULT_CODE result_code, http_response_t **http_response)
{
    DEBUG_ENTRY("string_array_t *header = <%p>, mcl_uint8_t *payload = <%p>, mcl_size_t payload_size = <%u>, E_MCL_HTTP_RESULT_CODE result_code = <%d>, http_response_t **http_response = <%p>",
                header, payload, payload_size, result_code, http_response)

    // Create a new http response object.
    MCL_NEW(*http_response);
    ASSERT_CODE_MESSAGE(MCL_NULL != *http_response, MCL_OUT_OF_MEMORY, "Memory can not be allocated for http response object.");

    // Set members of the http_response object.
    (*http_response)->header = header;
    (*http_response)->payload = payload;
    (*http_response)->payload_size = payload_size;
    (*http_response)->result_code = result_code;

    DEBUG_LEAVE("retVal = <%d>", MCL_OK);
    return MCL_OK;
}

E_MCL_ERROR_CODE http_response_get_header(http_response_t *http_response, char *header_name, string_t **header_value)
{
    DEBUG_ENTRY("http_response_t *http_response = <%p>, char *header_name = <%s>", http_response, header_name)

	E_MCL_ERROR_CODE result = MCL_FAIL;
    mcl_size_t header_count = http_response->header->index;
	mcl_size_t index;

    for (index = 0; index < header_count; index++)
    {
		mcl_size_t match_index = 0;
		mcl_bool_t found;
		string_t *header = string_array_get(http_response->header, index);

        ASSERT_CODE(MCL_NULL != header, MCL_FAIL);

        found = string_util_find_case_insensitive(header->buffer, header_name, &match_index);

		// If correct header is found then get the value
        if (MCL_TRUE == found)
        {
            list_t *tokens = MCL_NULL;
            result = string_split(header, ':', &tokens);
            ASSERT_CODE_MESSAGE(MCL_OK == result, result, "string_split() failed for %s", header->buffer);

			// We expect token number to be 2. (key:value splits to 2)
			if (2 != tokens->count)
			{
				MCL_DEBUG("Number of colons that header contains is not 1.");
				result = MCL_FAIL;
			}
			else
			{
	            result = string_initialize(tokens->head->next->data, header_value);
            }

			list_destroy_with_content(&tokens, (list_item_destroy_callback)string_destroy);
            break;
        }
    }

    DEBUG_LEAVE("retVal = <%d>", result);
    return result;
}

mcl_uint8_t *http_response_get_payload(http_response_t *http_response)
{
    DEBUG_ENTRY("http_response_t *http_response = <%p>", http_response)

    DEBUG_LEAVE("retVal = <%p>", http_response->payload);
    return http_response->payload;
}

E_MCL_HTTP_RESULT_CODE http_response_get_result_code(http_response_t *http_response)
{
    DEBUG_ENTRY("http_response_t *http_response = <%p>", http_response)

    DEBUG_LEAVE("retVal = <%d>", http_response->result_code);
    return http_response->result_code;
}

void http_response_destroy(http_response_t **http_response)
{
    DEBUG_ENTRY("http_response_t **http_response = <%p>", http_response)

    if (MCL_NULL != *http_response)
    {
        // Destroy http_response together with its members.
        string_array_destroy(&((*http_response)->header));
        MCL_FREE((*http_response)->payload);
        MCL_FREE(*http_response);

        MCL_DEBUG("Http response is successfully destroyed.");
    }
    else
    {
        MCL_DEBUG("Http response is already NULL.");
    }

    DEBUG_LEAVE("retVal = void");
}
