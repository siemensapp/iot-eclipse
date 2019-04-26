/*!**********************************************************************
*
* @copyright Copyright (C) 2016 Siemens Aktiengesellschaft.\n
*            All rights reserved.
*
*************************************************************************
*
* @file     stream_data.c
* @date     Jul 27, 2016
* @brief    Stream data module implementation file.
*
************************************************************************/

#include "mcl/mcl_common.h"
#include "mcl/mcl_custom_data.h"
#include "stream_data.h"
#include "log_util.h"
#include "memory.h"
#include "definitions.h"

E_MCL_ERROR_CODE stream_data_initialize(const char *version, const char *type, const char *routing, mcl_stream_data_read_callback_t stream_data_read_callback,
        void *user_context, stream_data_t **stream_data)
{
	DEBUG_ENTRY("const char *version = <%s>, const char *type = <%s>, const char *routing = <%p>, mcl_stream_data_read_callback_t stream_data_read_callback = <%p>, void *user_context = <%p>, stream_data_t **stream_data = <%p>",
		version, type, routing, stream_data_read_callback, user_context, stream_data)
	
	E_MCL_ERROR_CODE result;

    MCL_NEW(*stream_data);
    ASSERT_CODE_MESSAGE(MCL_NULL != *stream_data, MCL_OUT_OF_MEMORY, "Memory can not be allocated for stream data.");

    (*stream_data)->user_context = user_context;
    (*stream_data)->read_callback = stream_data_read_callback;

    result = custom_data_initialize(version, type, routing, &((*stream_data)->base));

    ASSERT_STATEMENT_CODE_MESSAGE(MCL_OK == result, stream_data_destroy(stream_data), result, "Base custom data meta initialization failed !");

    DEBUG_LEAVE("retVal = <%d>", result);
    return result;
}

E_MCL_ERROR_CODE mcl_stream_data_set_meta_details(mcl_stream_data_t *stream_data, const mcl_json_t *details)
{
	DEBUG_ENTRY("mcl_stream_data_t *stream_data = <%p>, const mcl_json_t *details = <%p>", stream_data, details)

	E_MCL_ERROR_CODE result;

    ASSERT_NOT_NULL(stream_data);

    result = mcl_custom_data_set_meta_details(stream_data->base, details);

    DEBUG_LEAVE("retVal = <%d>", result);
    return result;
}

void stream_data_destroy(stream_data_t **stream_data)
{
    DEBUG_ENTRY("stream_data_t **stream_data = <%p>", stream_data)

    custom_data_destroy(&((*stream_data)->base));

    MCL_FREE(*stream_data);

    DEBUG_LEAVE("retVal = void");
}
