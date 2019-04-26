/*!**********************************************************************
*
* @copyright Copyright (C) 2016 Siemens Aktiengesellschaft.\n
*            All rights reserved.
*
*************************************************************************
*
* @file     storage.c
* @date     Oct 6, 2016
* @brief    Storage module implementation file.
*
************************************************************************/

#include "storage.h"
#include "definitions.h"
#include "memory.h"
#include "log_util.h"
#include "file_util.h"
#include "string_util.h"

// Get public key from file in PEM format.
static E_MCL_ERROR_CODE _get_public_key(char *buffer, mcl_size_t buffer_size, void *file_descriptor);

// Get private key from file in PEM format.
static E_MCL_ERROR_CODE _get_private_key(char *buffer, mcl_size_t buffer_size, void *file_descriptor);

E_MCL_ERROR_CODE storage_save_shared_secret_registration_information(string_t *path, string_t *client_id, string_t *client_secret, string_t *registration_access_token,
        string_t *registration_uri)
{
    DEBUG_ENTRY("string_t *path = <%p>, string_t *client_id = <%p>, string_t *client_secret = <%p>, string_t *registration_access_token = <%p>, string_t *registration_uri = <%p>", path, client_id, client_secret, registration_access_token, registration_uri)

#if (1 == HAVE_STORAGE_MEDIUM_)

#if (1 == HAVE_FILE_SYSTEM_)

    E_MCL_ERROR_CODE code;

    void *file_descriptor = MCL_NULL;
    mcl_bool_t ok = MCL_OK == (file_util_fopen(path->buffer, "w", &file_descriptor));

    // TODO : Check and save security profile when RSA is implemented.
    ok = ok && (MCL_OK == file_util_fputs(client_id->buffer, file_descriptor));
    ok = ok && (MCL_OK == file_util_fputs("\n", file_descriptor));
    ok = ok && (MCL_OK == file_util_fputs(client_secret->buffer, file_descriptor));
    ok = ok && (MCL_OK == file_util_fputs("\n", file_descriptor));
    ok = ok && (MCL_OK == file_util_fputs(registration_access_token->buffer, file_descriptor));
    ok = ok && (MCL_OK == file_util_fputs("\n", file_descriptor));
    ok = ok && (MCL_OK == file_util_fputs(registration_uri->buffer, file_descriptor));
    ok = ok && (MCL_OK == file_util_fputs("\n", file_descriptor));

    if (ok)
    {
        code = MCL_OK;
        MCL_INFO("Registration information is successfully saved.");
    }
    else
    {
        code = MCL_REGISTRATION_INFO_IS_NOT_SAVED;
        MCL_WARN("Registration information can not be saved.");
    }

    file_util_fclose(file_descriptor);

#else
    E_MCL_ERROR_CODE code = MCL_REGISTRATION_INFO_IS_NOT_SAVED;
    MCL_INFO("There is no file system support.");
#endif

#else
    E_MCL_ERROR_CODE code = MCL_REGISTRATION_INFO_IS_NOT_SAVED;
    MCL_INFO("There is no storage medium in the system.");
#endif

    DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

E_MCL_ERROR_CODE storage_save_rsa_registration_information(string_t *path, string_t *client_id, char *public_key, char *private_key, string_t *registration_access_token,
        string_t *registration_uri)
{
    DEBUG_ENTRY("string_t *path = <%p>, string_t *client_id = <%p>, char *public_key = <%s>, char *private_key = <%s>, string_t *registration_access_token = <%p>, string_t *registration_uri = <%p>", path, client_id, public_key, private_key, registration_access_token, registration_uri)

#if (1 == HAVE_STORAGE_MEDIUM_)

#if (1 == HAVE_FILE_SYSTEM_)

    E_MCL_ERROR_CODE code;

    void *file_descriptor = MCL_NULL;
    mcl_bool_t ok = MCL_OK == (file_util_fopen(path->buffer, "w", &file_descriptor));

    // TODO : Check and save security profile when RSA is implemented.
    ok = ok && (MCL_OK == file_util_fputs(client_id->buffer, file_descriptor));
    ok = ok && (MCL_OK == file_util_fputs("\n", file_descriptor));
    ok = ok && (MCL_OK == file_util_fputs(public_key, file_descriptor));
    ok = ok && (MCL_OK == file_util_fputs(private_key, file_descriptor));
    ok = ok && (MCL_OK == file_util_fputs(registration_access_token->buffer, file_descriptor));
    ok = ok && (MCL_OK == file_util_fputs("\n", file_descriptor));
    ok = ok && (MCL_OK == file_util_fputs(registration_uri->buffer, file_descriptor));
    ok = ok && (MCL_OK == file_util_fputs("\n", file_descriptor));

    if (ok)
    {
        code = MCL_OK;
        MCL_INFO("Registration information is successfully saved.");
    }
    else
    {
        code = MCL_REGISTRATION_INFO_IS_NOT_SAVED;
        MCL_WARN("Registration information can not be saved.");
    }

    file_util_fclose(file_descriptor);

#else
    E_MCL_ERROR_CODE code = MCL_REGISTRATION_INFO_IS_NOT_SAVED;
    MCL_INFO("There is no file system support.");
#endif

#else
    E_MCL_ERROR_CODE code = MCL_REGISTRATION_INFO_IS_NOT_SAVED;
    MCL_INFO("There is no storage medium in the system.");
#endif

    DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

E_MCL_ERROR_CODE storage_load_shared_secret_registration_information(string_t *path, string_t **client_id, string_t **client_secret, string_t **registration_access_token,
        string_t **registration_uri)
{
    DEBUG_ENTRY("string_t *path = <%p>, string_t **client_id = <%p>, string_t **client_secret = <%p>, string_t **registration_access_token = <%p>, string_t **registration_uri = <%p>", path, client_id, client_secret, registration_access_token, registration_uri)

#if (1 == HAVE_STORAGE_MEDIUM_)

#if (1 == HAVE_FILE_SYSTEM_)

    E_MCL_ERROR_CODE code;
	void *file_descriptor = MCL_NULL;
	mcl_size_t string_length = 0;
	mcl_bool_t ok;

    mcl_size_t buffer_size = 1024;
    char *buffer = MCL_MALLOC(buffer_size);
    ASSERT_CODE_MESSAGE(MCL_NULL != buffer, MCL_OUT_OF_MEMORY, "Memory can not be allocated for file read buffer.");

    code = file_util_fopen(path->buffer, "r", &file_descriptor);
    ASSERT_STATEMENT_CODE_MESSAGE(MCL_OK == code, MCL_FREE(buffer), MCL_REGISTRATION_INFO_IS_NOT_LOADED, "File does not exist.");

    ok = MCL_OK == file_util_fgets(buffer, buffer_size, file_descriptor);
    if (ok)
    {
        string_length = string_util_strlen(buffer);
        ok = (MCL_OK == string_initialize_new(buffer, string_length - 1, client_id));
    }

    ok = ok && (MCL_OK == file_util_fgets(buffer, buffer_size, file_descriptor));
    if (ok)
    {
        string_length = string_util_strlen(buffer);
        ok = (MCL_OK == string_initialize_new(buffer, string_length - 1, client_secret));
    }

    ok = ok && (MCL_OK == file_util_fgets(buffer, buffer_size, file_descriptor));
    if (ok)
    {
        string_length = string_util_strlen(buffer);
        ok = (MCL_OK == string_initialize_new(buffer, string_length - 1, registration_access_token));
    }

    ok = ok && (MCL_OK == file_util_fgets(buffer, buffer_size, file_descriptor));
    if (ok)
    {
        string_length = string_util_strlen(buffer);
        ok = (MCL_OK == string_initialize_new(buffer, string_length - 1, registration_uri));
    }

    if (ok)
    {
        code = MCL_OK;
        MCL_DEBUG("Registration information is successfully loaded.");
    }
    else
    {
        code = MCL_REGISTRATION_INFO_IS_NOT_LOADED;
        MCL_DEBUG("Registration information can not be loaded.");
        string_destroy(client_id);
        string_destroy(client_secret);
        string_destroy(registration_access_token);
        string_destroy(registration_uri);
    }

    file_util_fclose(file_descriptor);
    MCL_FREE(buffer);

#else
    E_MCL_ERROR_CODE code = MCL_NO_FILE_SUPPORT;
    MCL_INFO("There is no file system support.");
#endif

#else
    E_MCL_ERROR_CODE code = MCL_NO_STORAGE_MEDIUM;
    MCL_INFO("There is no storage medium in the system.");
#endif

    DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

E_MCL_ERROR_CODE storage_load_rsa_registration_information(string_t *path, string_t **client_id, char **public_key, char **private_key, string_t **registration_access_token,
        string_t **registration_uri)
{
    DEBUG_ENTRY("string_t *path = <%p>, string_t **client_id = <%p>, char **public_key = <%p>, char **private_key = <%p>, string_t **registration_access_token = <%p>, string_t **registration_uri = <%p>", path, client_id, public_key, private_key, registration_access_token, registration_uri)

#if (1 == HAVE_STORAGE_MEDIUM_)

#if (1 == HAVE_FILE_SYSTEM_)

    E_MCL_ERROR_CODE code;
	mcl_size_t buffer_size = 3000;
	mcl_size_t string_length = 0;
	mcl_bool_t ok;
	char *buffer;
	void *file_descriptor = MCL_NULL;

    *client_id = MCL_NULL;
    *public_key = MCL_NULL;
    *private_key = MCL_NULL;
    *registration_access_token = MCL_NULL;
    *registration_uri = MCL_NULL;

    buffer = MCL_MALLOC(buffer_size);
    ASSERT_CODE_MESSAGE(MCL_NULL != buffer, MCL_OUT_OF_MEMORY, "Memory can not be allocated for file read buffer.");

    code = file_util_fopen(path->buffer, "r", &file_descriptor);
    ASSERT_STATEMENT_CODE_MESSAGE(MCL_OK == code, MCL_FREE(buffer), MCL_REGISTRATION_INFO_IS_NOT_LOADED, "File does not exist.");

    ok = MCL_OK == file_util_fgets(buffer, buffer_size, file_descriptor);
    if (ok)
    {
        string_length = string_util_strlen(buffer);
        ok = (MCL_OK == string_initialize_new(buffer, string_length - 1, client_id));
    }

    ok = ok && (MCL_OK == _get_public_key(buffer, buffer_size, file_descriptor));
    if (ok)
    {
        *public_key = string_util_strdup(buffer);
        ok = (MCL_NULL != *public_key);
    }

    ok = ok && (MCL_OK == _get_private_key(buffer, buffer_size, file_descriptor));
    if (ok)
    {
        *private_key = string_util_strdup(buffer);
        ok = (MCL_NULL != *private_key);
    }

    ok = ok && (MCL_OK == file_util_fgets(buffer, buffer_size, file_descriptor));
    if (ok)
    {
        string_length = string_util_strlen(buffer);
        ok = (MCL_OK == string_initialize_new(buffer, string_length - 1, registration_access_token));
    }

    ok = ok && (MCL_OK == file_util_fgets(buffer, buffer_size, file_descriptor));
    if (ok)
    {
        string_length = string_util_strlen(buffer);
        ok = (MCL_OK == string_initialize_new(buffer, string_length - 1, registration_uri));
    }

    if (ok)
    {
        code = MCL_OK;
        MCL_DEBUG("Registration information is successfully loaded.");
    }
    else
    {
        code = MCL_REGISTRATION_INFO_IS_NOT_LOADED;
        MCL_DEBUG("Registration information can not be loaded.");
        string_destroy(client_id);
        MCL_FREE(*public_key);
        MCL_FREE(*private_key);
        string_destroy(registration_access_token);
        string_destroy(registration_uri);
    }

    file_util_fclose(file_descriptor);
    MCL_FREE(buffer);

#else
    E_MCL_ERROR_CODE code = MCL_NO_FILE_SUPPORT;
    MCL_INFO("There is no file system support.");
#endif

#else
    E_MCL_ERROR_CODE code = MCL_NO_STORAGE_MEDIUM;
    MCL_INFO("There is no storage medium in the system.");
#endif

    DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

static E_MCL_ERROR_CODE _get_public_key(char *buffer, mcl_size_t buffer_size, void *file_descriptor)
{
    DEBUG_ENTRY("char *buffer = <%s>, mcl_size_t buffer_size = <%u>, void *file_descriptor = <%p>", buffer, buffer_size, file_descriptor)

    E_MCL_ERROR_CODE code;
    E_MCL_ERROR_CODE compare_result;

    const char *begin_line = "-----BEGIN PUBLIC KEY-----\n";
    mcl_size_t begin_line_length = 27;

    const char *end_line = "-----END PUBLIC KEY-----\n";
    mcl_size_t end_line_length = 25;

    mcl_size_t length = 0;
    mcl_size_t buffer_offset = 0;

    // Get first line.
    if (MCL_OK == (code = file_util_fgets(buffer, buffer_size, file_descriptor)))
    {
        // Check if it is the beginning line.
        compare_result = string_util_strncmp(buffer, begin_line, begin_line_length);

        if (MCL_OK == compare_result)
        {
           // Begin line is found.
           buffer_offset += begin_line_length;

           // Concatenate key to buffer until end line is reached.
           do
           {
               // Get next line.
               code = file_util_fgets(buffer + buffer_offset, buffer_size - buffer_offset, file_descriptor);

               if (MCL_OK == code)
               {
                   // Check if end line is reached.
                   compare_result = string_util_strncmp(buffer + buffer_offset, end_line, end_line_length);
                   length = string_util_strlen(buffer + buffer_offset);
				   buffer_offset += length;
				   if (buffer_offset >= buffer_size - 1)
				   {
					   code = MCL_FAIL;
					   break;
				   }
               }
               else
               {
                   // Can not read any more data from file.
                   break;
               }
           } while (MCL_OK != compare_result);
        }
        else
        {
            // Can not find the beginning line.
            code = MCL_FAIL;
        }
    }

    DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

static E_MCL_ERROR_CODE _get_private_key(char *buffer, mcl_size_t buffer_size, void *file_descriptor)
{
    DEBUG_ENTRY("char *buffer = <%s>, mcl_size_t buffer_size = <%u>, void *file_descriptor = <%p>", buffer, buffer_size, file_descriptor)

    E_MCL_ERROR_CODE code;
    E_MCL_ERROR_CODE compare_result;

    const char *begin_line = "-----BEGIN RSA PRIVATE KEY-----\n";
    mcl_size_t begin_line_length = 32;

    const char *end_line = "-----END RSA PRIVATE KEY-----\n";
    mcl_size_t end_line_length = 30;

    mcl_size_t length = 0;
    mcl_size_t buffer_offset = 0;

    // Get first line.
    if (MCL_OK == (code = file_util_fgets(buffer, buffer_size, file_descriptor)))
    {
        // Check if it is the beginning line.
        compare_result = string_util_strncmp(buffer, begin_line, begin_line_length);

        if (MCL_OK == compare_result)
        {
           // Begin line is found.
           buffer_offset += begin_line_length;

           // Concatenate key to buffer until end line is reached.
           do
           {
               // Get next line.
               code = file_util_fgets(buffer + buffer_offset, buffer_size - buffer_offset, file_descriptor);

               if (MCL_OK == code)
               {
                   // Check if end line is reached.
                   compare_result = string_util_strncmp(buffer + buffer_offset, end_line, end_line_length);
                   length = string_util_strlen(buffer + buffer_offset);
                   buffer_offset += length;
               }
               else
               {
                   // Can not read any more data from file.
                   break;
               }
           } while (MCL_OK != compare_result);
        }
        else
        {
            // Can not find the beginning line.
            code = MCL_FAIL;
        }
    }

    DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}
