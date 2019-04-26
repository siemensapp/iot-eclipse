/*!**********************************************************************
*
* @copyright Copyright (C) 2016 Siemens Aktiengesellschaft.\n
*            All rights reserved.
*
*************************************************************************
*
* @file     string_type.c
* @date     Aug 2, 2016
* @brief    String type module implementation file.
*
************************************************************************/

#include "string_type.h"
#include "memory.h"
#include "log_util.h"
#include "definitions.h"

const char *hex_table = "0123456789ABCDEF";

/// Private Function Prototypes:
E_MCL_ERROR_CODE _initialize(string_t *string, const char *value, mcl_size_t value_length);

E_MCL_ERROR_CODE string_initialize(const string_t *other, string_t **string)
{
    VERBOSE_ENTRY("const string_t *other = <%p>, string_t **string = <%p>", other, string)

	E_MCL_ERROR_CODE result;
    MCL_NEW(*string);
    ASSERT_CODE_MESSAGE(MCL_NULL != *string, MCL_OUT_OF_MEMORY, "Not enough memory to allocate for string_t!");

    (*string)->type = MCL_STRING_COPY_DESTROY;

    result = _initialize(*string, other->buffer, other->length);

    if (MCL_OK != result)
    {
        MCL_ERROR("_initialize_with_option() failed!. Freeing the string_t!");
        MCL_FREE(*string);
    }

    VERBOSE_LEAVE("retVal = <%d>", result);
    return result;
}

E_MCL_ERROR_CODE string_initialize_new(const char *value, mcl_size_t value_length, string_t **string)
{
    VERBOSE_ENTRY("const char *value = <%s>, mcl_size_t value_length = <%u>, string_t **string = <%p>", value, value_length, string)

	E_MCL_ERROR_CODE result;
    MCL_NEW(*string);
    ASSERT_CODE_MESSAGE(MCL_NULL != *string, MCL_OUT_OF_MEMORY, "Not enough memory to allocate for string_t!");

    (*string)->type = MCL_STRING_COPY_DESTROY;

    result = _initialize(*string, value, value_length);

    if (MCL_OK != result)
    {
        MCL_ERROR("_initialize_with_option() failed!. Freeing the string_t!");
        MCL_FREE(*string);
    }

    VERBOSE_LEAVE("retVal = <%d>", result);
    return result;
}

E_MCL_ERROR_CODE string_initialize_dynamic(const char *value, mcl_size_t value_length, string_t **string)
{
    VERBOSE_ENTRY("const char *value = <%s>, mcl_size_t value_length = <%u>, string_t **string = <%p>", value, value_length, string)

	E_MCL_ERROR_CODE result;
    MCL_NEW(*string);
    ASSERT_CODE_MESSAGE(MCL_NULL != *string, MCL_OUT_OF_MEMORY, "Not enough memory to allocate for string_t!");

    (*string)->type = MCL_STRING_NOT_COPY_DESTROY;

    result = _initialize(*string, value, value_length);

    if (MCL_OK != result)
    {
        MCL_ERROR("_initialize_with_option() failed!. Freeing the string_t!");
        MCL_FREE(*string);
    }

    VERBOSE_LEAVE("retVal = <%d>", result);
    return result;
}

E_MCL_ERROR_CODE string_initialize_static(const char *value, mcl_size_t value_length, string_t **string)
{
    VERBOSE_ENTRY("const char *value = <%s>, mcl_size_t value_length = <%u>, string_t **string = <%p>", value, value_length, string)

	E_MCL_ERROR_CODE result;
    MCL_NEW(*string);
    ASSERT_CODE_MESSAGE(MCL_NULL != *string, MCL_OUT_OF_MEMORY, "Not enough memory to allocate for string_t!");

    (*string)->type = MCL_STRING_NOT_COPY_NOT_DESTROY;

    result = _initialize(*string, value, value_length);

    if (MCL_OK != result)
    {
        MCL_ERROR("_initialize_with_option() failed!. Freeing the string_t!");
        MCL_FREE(*string);
    }

    VERBOSE_LEAVE("retVal = <%d>", result);
    return result;
}

E_MCL_ERROR_CODE string_set(string_t *string, const char *value, mcl_size_t value_length)
{
	VERBOSE_ENTRY("string_t *string = <%p>, const char *value = <%s>, mcl_size_t value_length = <%u>", string, value, value_length)
		
	E_MCL_ERROR_CODE result;
	
    // first call release to release the buffer :
    string_release(string);

    result = _initialize(string, value, value_length);

    ASSERT_CODE_MESSAGE(MCL_OK == result, result, "initialize_with_option failed! Not freeing the string but it's buffer has already been freed!");

    VERBOSE_LEAVE("retVal = <%d>", MCL_OK);
    return MCL_OK;
}

E_MCL_ERROR_CODE string_compare(const string_t *string, const string_t *other)
{
    DEBUG_ENTRY("const string_t *string = <%p>, const string_t *other = <%p>", string, other)

    E_MCL_ERROR_CODE result = string_compare_with_cstr(string, other->buffer);

    DEBUG_LEAVE("retVal = <%d>", result);
    return result;
}

E_MCL_ERROR_CODE string_compare_with_cstr(const string_t *string, const char *other)
{
    DEBUG_ENTRY("const string_t *string = <%p>, const char *other = <%s>", string, other)

	E_MCL_ERROR_CODE result = MCL_FAIL;
    mcl_size_t length = 0;

    if ((MCL_NULL != string) && (MCL_NULL != string->buffer))
    {
        if (MCL_NULL != other)
        {
            // first check if both are pointing the same memory space
            if (string->buffer == other)
            {
                MCL_DEBUG("string->buffer and other are pointing to the same memory space");
                result = MCL_OK;
            }
            else
            {
                length = string_util_strlen(other);
                if (length == string->length)
                {
                    result = string_util_strncmp(string->buffer, other, string->length);
                }
            }
        }
        else
        {
            MCL_DEBUG("Received compare string (other) is NULL.");
        }
    }
    else
    {
        MCL_DEBUG("Received string object or it's buffer is NULL");
    }

    DEBUG_LEAVE("retVal = <%d>", result);
    return result;
}

E_MCL_ERROR_CODE string_split(string_t *string, const char token, list_t **string_list)
{
    DEBUG_ENTRY("string_t *string = <%p>, const char token = <%c>, list_t **string_list = <%p>", string, token, string_list)

    E_MCL_ERROR_CODE result = MCL_OK;
	list_t *list = MCL_NULL;

	// buf might be at most as string->length ( C99 allows this )
	char *temp_buffer = string->buffer;
	mcl_size_t temp_length = 0;
	string_t *current_part;
	mcl_size_t index;

    result = list_initialize(string_list);
    ASSERT_CODE_MESSAGE(MCL_OK == result, result, "mcl_list initialize failed!");

    list = *string_list;

    for (index = 0; index < string->length; index++)
    {
        if (string->buffer[index] != token)
        {
            temp_length++;
        }
        else
        {
            // add contents of temp_buffer into the list ( if any ):
            if (temp_length > 0)
            {
                if (MCL_OK != (result = string_initialize_new(temp_buffer, temp_length, &current_part)))
                {
                    MCL_ERROR("string_initialize_new failed!");
                    temp_length = 0;

                    break;
                }

                if (MCL_OK != (result = list_add(list, current_part)))
                {
                    MCL_ERROR("Current split part of the string couldn't be added to the list!");
                    string_destroy(&current_part);
                    temp_length = 0;

                    break;
                }

                MCL_DEBUG("Current split part has been added to the list = <%s>", current_part->buffer);

                // now reset the temp buffers for the next one :
                temp_buffer += temp_length + 1;
                temp_length = 0;
            }
            else
            {
                // this means current char is token but there is no unlisted parts before.
                // we need to shift temp_buffer pointer to the next one :
                temp_buffer++;
            }
        }
    }

    // check if there is still something to add :
    if ((MCL_OK == result) && ((temp_length > 0) && (MCL_NULL_CHAR != *temp_buffer)))
    {
        if (MCL_OK != (result = string_initialize_new(temp_buffer, temp_length, &current_part)))
        {
            MCL_ERROR("string_initialize_new failed!");
        }
        else
        {
            if (MCL_OK != (result = list_add(list, current_part)))
            {
                MCL_ERROR("Last split part of the string couldn't be added to the list!");
                string_destroy(&current_part);
            }
            else
            {
                MCL_DEBUG("Last split part has been added to the list = <%s>", current_part->buffer);
            }
        }
    }

    // If something wrong happened, destroy the content of the list before return
    if ((MCL_OK != result) && (MCL_NULL != *string_list))
    {
        MCL_DEBUG("Result is NOT successful. Clearing the content of the list if there is any");
        list_destroy_with_content(string_list, (list_item_destroy_callback)string_destroy);
    }

    DEBUG_LEAVE("retVal = <%p>", result);
    return result;
}

E_MCL_ERROR_CODE string_convert_binary_to_hex(const mcl_uint8_t *buffer, mcl_size_t buffer_size, string_t **hex_data)
{
    VERBOSE_ENTRY("const mcl_uint8_t *buffer = <%p>, mcl_size_t buffer_size = <%u>, string_t **hex_data = <%p>", buffer, buffer_size, hex_data)

    // initialize with empty string
    E_MCL_ERROR_CODE code = string_initialize_new(MCL_NULL, 0, hex_data);
	mcl_size_t i;
    ASSERT_CODE_MESSAGE(MCL_OK == code, code, "Initialization of hex string failed!");

    // allocate space for hex result (zero terminated)
    (*hex_data)->length = 2 * buffer_size;
    (*hex_data)->buffer = MCL_CALLOC(1, (*hex_data)->length + 1);
    ASSERT_CODE_MESSAGE(MCL_NULL != (*hex_data)->buffer, MCL_OUT_OF_MEMORY, "Memory to store hex string couldn't be allocated!");

    // convert byte to hex using table
    for (i = 0; i < buffer_size; i++)
    {
        ((*hex_data)->buffer)[i * 2 + 0] = hex_table[(buffer[i] >> 4) & 0x0F];
        ((*hex_data)->buffer)[i * 2 + 1] = hex_table[buffer[i] & 0x0F];
    }
    ((*hex_data)->buffer)[buffer_size * 2] = MCL_NULL_CHAR;

    VERBOSE_LEAVE("retVal = <%d>", MCL_OK);
    return MCL_OK;
}

void string_release(string_t *string)
{
    VERBOSE_ENTRY("string_t *string = <%p>", string)

    if (MCL_NULL != string->buffer)
    {
        // free the buffer according to the type :
        if ((MCL_STRING_COPY_DESTROY == string->type) || (MCL_STRING_NOT_COPY_DESTROY == string->type))
        {
            MCL_VERBOSE("Type is MCL_STRING_COPY_DESTROY or MCL_STRING_NOT_COPY_DESTROY. Freeing the buffer.");
            MCL_FREE(string->buffer);
        }
        else
        {
            MCL_VERBOSE("Type is MCL_STRING_NOT_COPY_NOT_DESTROY. Not freeing the buffer.");
            string->buffer = MCL_NULL;
        }

        string->length = 0;
    }
    else
    {
        MCL_VERBOSE("String buffer is already NULL!");
    }

    VERBOSE_LEAVE("retVal = void");
}

void string_destroy(string_t **string)
{
    VERBOSE_ENTRY("string_t **string = <%p>", string)

    if (MCL_NULL != *string)
    {
        string_release(*string);
        MCL_FREE(*string);
    }

    VERBOSE_LEAVE("retVal = void");
}

E_MCL_ERROR_CODE _initialize(string_t *string, const char *value, mcl_size_t value_length)
{
    VERBOSE_ENTRY("string_t *string = <%p>, const char *value = <%s>, mcl_size_t value_length = <%u>", string, value, value_length)

    // set the buffer and length :
    if (MCL_NULL != value)
    {
		mcl_size_t length = 0;

		MCL_VERBOSE("Not NULL value received. String will be initialized with it.");

        if (value_length > 0)
        {
            length = value_length;
        }
        else
        {
            // calculate length :
            length = string_util_strlen(value);
        }

        // copy the value into string buffer if requested :
        if (MCL_STRING_COPY_DESTROY == string->type)
        {
            MCL_VERBOSE("Copy Flag is TRUE : Given value parameter will be copied into the buffer");

            // allocate buffer
            string->buffer = MCL_MALLOC(length + 1);
            ASSERT_CODE_MESSAGE(MCL_NULL != string->buffer, MCL_OUT_OF_MEMORY, "Memory couldn't be allocated for string_t");

            // set empty right away otherwise might corrupts the mem in _strncpy since it tries to log the input parameters:
            string->buffer[0] = MCL_NULL_CHAR;

            // copy the string :
            string_util_strncpy(string->buffer, value, length);

            // finalize the buffer:
            string->buffer[length] = MCL_NULL_CHAR;
        }
        else
        {
            MCL_VERBOSE("Copy Flag is FALSE : Given value parameters address will be assigned to the buffer");

            // to avoid compilers const warning, explicitly casting to (char *):
            string->buffer = (char *)value;
        }

        // set the length :
        string->length = length;

        MCL_VERBOSE("String buffer has been prepared. Buffer = <%s>, Length = <%d>", string->buffer, string->length);
    }
    else if ((0 < value_length) && (MCL_STRING_COPY_DESTROY == string->type))
    {
        MCL_VERBOSE("NULL value received, but new string with given length (> 0) requested. Buffer for string will be allocated and zero-terminated. Buffer remains uninitialized.");

        string->buffer = MCL_MALLOC(value_length + 1);
        ASSERT_CODE_MESSAGE(MCL_NULL != string->buffer, MCL_OUT_OF_MEMORY, "Memory couldn't be allocated for string_t");

        string->length = value_length;

        // finalize the buffer and set it's length:
        string->buffer[value_length] = MCL_NULL_CHAR;
    }
    else
    {
        MCL_VERBOSE("NULL value received. String will be initialized as empty.");

        // init value is null. we are not going to allocate memory for internal buffer
        string->buffer = MCL_NULL;
        string->length = 0;
    }

    VERBOSE_LEAVE("retVal = <%d>", MCL_OK);
    return MCL_OK;
}

E_MCL_ERROR_CODE string_concatenate(string_t *string_1, string_t *string_2, string_t **result)
{
    VERBOSE_ENTRY("string_t *string_1 = <%p>, string_t *string_2 = <%p>, string_t **result = <%p>", string_1, string_2, result)

    E_MCL_ERROR_CODE code = MCL_FAIL;
    mcl_size_t result_length = string_1->length + string_2->length;

    if (MCL_OK == (code = string_initialize_new(MCL_NULL, result_length, result)))
    {
        code = string_util_snprintf((*result)->buffer, result_length + 1, "%s%s", string_1->buffer, string_2->buffer);
        ASSERT_STATEMENT_CODE_MESSAGE(MCL_OK == code, string_destroy(result), code, "Cannot concatenate strings.");
    }

    VERBOSE_LEAVE("retVal = <%d>", code);
    return code;
}

E_MCL_ERROR_CODE string_replace(string_t *source, const char *old_string, const char *new_string, string_t **result)
{
	VERBOSE_ENTRY("string_t *source = <%p>, const char *old_string = <%p>, const char *new_string = <%p>, string_t **result = <%p>", source, old_string, new_string, result)

	E_MCL_ERROR_CODE code;
	mcl_size_t start_index;
	mcl_size_t old_string_length;
	mcl_size_t new_string_length;
	mcl_size_t target_string_length;
	mcl_bool_t register_keyword_found;
	char *target_string;
	
    old_string_length = string_util_strlen(old_string);
    new_string_length = string_util_strlen(new_string);
	target_string_length = source->length - old_string_length + new_string_length;

	target_string = MCL_CALLOC(target_string_length + 1, 1);
	ASSERT_CODE_MESSAGE(MCL_NULL != target_string, MCL_OUT_OF_MEMORY, "Could not allocate memory for target_string");

	register_keyword_found = string_util_find(source->buffer, old_string, &start_index);
	ASSERT_STATEMENT_CODE_MESSAGE(MCL_TRUE == register_keyword_found, MCL_FREE(target_string), MCL_FAIL, "String does not contain old_string");

	string_util_strncat(target_string, source->buffer, start_index);
	string_util_strncat(target_string, new_string, new_string_length);
	string_util_strncat(target_string, source->buffer + start_index + old_string_length, source->length - (start_index + old_string_length));

	code = string_initialize_dynamic(target_string, target_string_length, result);

	VERBOSE_LEAVE("retVal = <%d>", code);
	return code;
}

E_MCL_ERROR_CODE string_concatenate_cstr(string_t *string, char *c_string, string_t **result)
{
    VERBOSE_ENTRY("string_t *string = <%p>, char *cstring = <%p>, string_t **result = <%p>", string, c_string, result)

	E_MCL_ERROR_CODE code;
	mcl_size_t c_string_length;
    mcl_size_t result_length;

    c_string_length = string_util_strlen(c_string);
    result_length = string->length + c_string_length;

	code = string_initialize_new(MCL_NULL, result_length, result);
    (code == MCL_OK) && (code = string_util_snprintf((*result)->buffer, result_length + 1, "%s%s", string->buffer, c_string));
    ASSERT_STATEMENT_CODE_MESSAGE(MCL_OK == code, string_destroy(result), code, "Cannot concatenate strings.");

    VERBOSE_LEAVE("retVal = <%d>", code);
    return code;
}
