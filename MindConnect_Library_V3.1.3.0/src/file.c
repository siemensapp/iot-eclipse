/*!**********************************************************************
*
* @copyright Copyright (C) 2016 Siemens Aktiengesellschaft.\n
*            All rights reserved.
*
*************************************************************************
*
* @file     file.c
* @date     Jul 27, 2016
* @brief    This source file implements the functions in file.h.
*
************************************************************************/

#include "file.h"
#include "definitions.h"
#include "memory.h"
#include "log_util.h"
#include "file_util.h"
#include "time_util.h"
// TODO: #include "mcl/mcl_file.h"

E_MCL_ERROR_CODE file_initialize(const char *version, const char *file_path, const char *file_name, const char *file_type, const char *routing, file_t **file)
{
    DEBUG_ENTRY("const char *version = <%p>, const char *file_path = <%p>, const char *file_name = <%p>, const char *file_type = <%p>, const char *routing = <%p>, file_t **file = <%p>",
                version, file_path, file_name, file_type, routing, file)

    // Allocate memory for a new file item.
    MCL_NEW(*file);
    ASSERT_CODE_MESSAGE(MCL_NULL != *file, MCL_OUT_OF_MEMORY, "Memory couldn't be allocated for file item.");

    // Set all fields to MCL_NULL first.
    (*file)->meta.content_id = MCL_NULL;
    (*file)->meta.type = MCL_NULL;
    (*file)->meta.version = MCL_NULL;
    (*file)->meta.details.routing = MCL_NULL;
    (*file)->meta.payload.type = MCL_NULL;
    (*file)->meta.payload.version = MCL_NULL;
    (*file)->meta.payload.details.file_details.file_name = MCL_NULL;
    (*file)->meta.payload.details.file_details.creation_date = MCL_NULL;
    (*file)->meta.payload.details.file_details.file_type = MCL_NULL;
    (*file)->payload.buffer = MCL_NULL;
    (*file)->descriptor = MCL_NULL;

    // If file name is null then this is a type of file of which the content and details will not be read from file system.
    // Instead, this type of file will have it's buffer inside it's payload.
    if (MCL_NULL != file_path)
    {
		E_MCL_ERROR_CODE code;
		mcl_stat_t file_attributes;
		mcl_bool_t is_regular_file;

        // Set meta.type.
        code = string_initialize_static(meta_field_values[META_FIELD_TYPE_ITEM].buffer, meta_field_values[META_FIELD_TYPE_ITEM].length, &(*file)->meta.type);
        ASSERT_STATEMENT_CODE_MESSAGE(MCL_OK == code, file_destroy(file), code, "String initialization failed for meta.type.");

        // Set meta.version.
        code = string_initialize_static(meta_field_values[META_FIELD_VERSION_CURRENT].buffer, meta_field_values[META_FIELD_VERSION_CURRENT].length, &(*file)->meta.version);
        ASSERT_STATEMENT_CODE_MESSAGE(MCL_OK == code, file_destroy(file), code, "String initialization failed for meta.version.");

        // Set meta.details.routing which is optional.
        if (MCL_NULL != routing)
        {
            code = string_initialize_new(routing, 0, &(*file)->meta.details.routing);
            ASSERT_STATEMENT_CODE_MESSAGE(MCL_OK == code, file_destroy(file), code, "String initialization failed for meta.details.routing.");
        }

        // Set meta.payload.type.
        code = string_initialize_static(meta_field_values[META_FIELD_PAYLOAD_TYPE_FILE].buffer, meta_field_values[META_FIELD_PAYLOAD_TYPE_FILE].length,
                                        &((*file)->meta.payload.type));
        ASSERT_STATEMENT_CODE_MESSAGE(MCL_OK == code, file_destroy(file), code, "String initialization failed for meta.payload.type.");

        // Set meta.payload.version.
        code = string_initialize_new(version, 0, &(*file)->meta.payload.version);
        ASSERT_STATEMENT_CODE_MESSAGE(MCL_OK == code, file_destroy(file), code, "String initialization failed for meta.payload.version.");

        // Open file if file_name is not null.
        code = file_util_fopen(file_path, "rb", &(*file)->descriptor);
        ASSERT_STATEMENT_CODE_MESSAGE(MCL_OK == code, file_destroy(file), MCL_FILE_CANNOT_BE_OPENED, "File can not be opened for reading.");

        // Set file name.
        code = string_initialize_new(file_name, 0, &(*file)->meta.payload.details.file_details.file_name);
        ASSERT_STATEMENT_CODE_MESSAGE(MCL_OK == code, file_destroy(file), code, "String initialization failed for meta.payload.details.file_details.file_name.");

        // Set file type which is optional.
        if (MCL_NULL != file_type)
        {
            code = string_initialize_new(file_type, 0, &(*file)->meta.payload.details.file_details.file_type);
            ASSERT_STATEMENT_CODE_MESSAGE(MCL_OK == code, file_destroy(file), code, "String initialization failed for meta.payload.details.file_details.file_type.");
        }

        // Get file attributes.
        code = file_util_fstat((*file)->descriptor, &file_attributes);
        ASSERT_STATEMENT_CODE_MESSAGE(MCL_OK == code, file_destroy(file), code, "File attributes can not be accessed.");

        // Check if regular file.
        is_regular_file = file_util_check_if_regular_file(&file_attributes);
        ASSERT_STATEMENT_CODE_MESSAGE(MCL_TRUE == is_regular_file, file_destroy(file), MCL_FILE_CANNOT_BE_OPENED, "File is not a regular file.");

        // Set file size.
        (*file)->payload.size = file_attributes.st_size;

        // Set file creation date.
        code = time_util_convert_to_iso_8601_format(&file_attributes.st_ctime, &(*file)->meta.payload.details.file_details.creation_date);
        ASSERT_STATEMENT_CODE_MESSAGE(MCL_OK == code, file_destroy(file), code, "String initialization failed for meta.payload.details.file_details.creation_date.");
    }

    DEBUG_LEAVE("retVal = <%d>", MCL_OK);
    return MCL_OK;
}

#if MCL_FILE_DOWNLOAD_ENABLED

E_MCL_ERROR_CODE mcl_file_get_name(mcl_file_t *file, char **file_name)
{
    DEBUG_ENTRY("mcl_file_t *file = <%p>, char **file_name = <%p>", file, file_name)

    ASSERT_NOT_NULL(file);
    ASSERT_NOT_NULL(file_name);

    *file_name = file->meta.payload.details.file_details.file_name->buffer;

    DEBUG_LEAVE("retVal = <%d>", MCL_OK);
    return MCL_OK;
}

E_MCL_ERROR_CODE mcl_file_get_size(mcl_file_t *file, mcl_size_t *file_size)
{
    DEBUG_ENTRY("mcl_file_t *file = <%p>, mcl_size_t *file_size = <%p>", file, file_size)

    ASSERT_NOT_NULL(file);
    ASSERT_NOT_NULL(file_size);

    *file_size = file->payload.size;

    DEBUG_LEAVE("retVal = <%d>", MCL_OK);
    return MCL_OK;
}

E_MCL_ERROR_CODE mcl_file_get_buffer(mcl_file_t *file, mcl_uint8_t **buffer)
{
    DEBUG_ENTRY("mcl_file_t *file = <%p>, mcl_uint8_t **buffer = <%p>", file, buffer)

    ASSERT_NOT_NULL(file);
    ASSERT_NOT_NULL(buffer);

    *buffer = file->payload.buffer;

    DEBUG_LEAVE("retVal = <%d>", MCL_OK);
    return MCL_OK;
}

void mcl_file_destroy(mcl_file_t **file)
{
	DEBUG_ENTRY("mcl_file_t **file = <%p>", file)

	file_destroy(file);

	DEBUG_LEAVE("retVal = void");
}

#endif

void file_destroy(file_t **file)
{
    DEBUG_ENTRY("file_t **file = <%p>", file)

    if (MCL_NULL != *file)
    {
        file_util_fclose((*file)->descriptor);
        string_destroy(&((*file)->meta.content_id));
        string_destroy(&((*file)->meta.type));
        string_destroy(&((*file)->meta.version));
        string_destroy(&((*file)->meta.details.routing));
        string_destroy(&((*file)->meta.payload.type));
        string_destroy(&((*file)->meta.payload.version));
        string_destroy(&((*file)->meta.payload.details.file_details.file_name));
        string_destroy(&((*file)->meta.payload.details.file_details.creation_date));
        string_destroy(&((*file)->meta.payload.details.file_details.file_type));
        MCL_FREE(*file);
    }

    DEBUG_LEAVE("retVal = void");
}
