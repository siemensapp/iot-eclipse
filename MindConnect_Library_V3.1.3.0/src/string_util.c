/*!**********************************************************************
*
* @copyright Copyright (C) 2016 Siemens Aktiengesellschaft.\n
*            All rights reserved.
*
*************************************************************************
*
* @file     string_util.c
* @date     Aug 12, 2016
* @brief    String utility module implementation file.
*
************************************************************************/

#include "string_util.h"
#include "log_util.h"
#include "definitions.h"
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

#define LOWERCASE(n) ((n >= 'A' && n <= 'Z') ? (n + ('a' - 'A')) : n)

mcl_size_t string_util_strlen(const char *buffer)
{
    DEBUG_ENTRY("const char *buffer = <%s>", buffer)

    mcl_size_t return_value = strlen(buffer);

    DEBUG_LEAVE("retVal = <%u>", return_value);
    return return_value;
}

mcl_size_t string_util_strnlen(const char *buffer, mcl_size_t maximum_length)
{
    DEBUG_ENTRY("const char *buffer = <%s>, mcl_size_t maximum_length = <%u>", buffer, maximum_length)

    mcl_size_t count = 0;
    while ((count < maximum_length) && (MCL_NULL_CHAR != buffer[count]))
    {
        count++;
    }

    DEBUG_LEAVE("retVal = <%u>", count);
    return count;
}

void string_util_strncpy(char *destination, const char *source, mcl_size_t count)
{
    DEBUG_ENTRY("char *destination = <%s>, const char *source = <%s>, mcl_size_t count = <%u>", destination, source, count)

#if defined(WIN32) || defined(WIN64)
    strncpy_s(destination, count + 1, source, count);
#else
    strncpy(destination, source, count);
#endif

    DEBUG_LEAVE("retVal = void");
}

void string_util_strncat(char *destination, const char *source, mcl_size_t count)
{
    DEBUG_ENTRY("char *destination = <%s>, const char *source = <%s>, mcl_size_t count = <%u>", destination, source, count)

#if defined(WIN32) || defined(WIN64)
    strncat_s(destination, strlen(destination) + count + 1, source, count);
#else
    strncat(destination, source, count);
#endif

    DEBUG_LEAVE("retVal = void");
}

E_MCL_ERROR_CODE string_util_strncmp(const char *string_1, const char *string_2, mcl_size_t count)
{
    DEBUG_ENTRY("const char *string_1 = <%s>, const char *string_2 = <%s>, mcl_size_t count = <%u>", string_1, string_2, count)

    E_MCL_ERROR_CODE code;
    int result = strncmp(string_1, string_2, count);

    // this check is necessary since result can be < 0
    if (0 == result)
    {
        code = MCL_OK;
    }
    else
    {
        code = MCL_FAIL;
    }

    DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

E_MCL_ERROR_CODE string_util_snprintf(char *string, mcl_size_t length, const char *format, ...)
{
    DEBUG_ENTRY("char *string = <%p>, mcl_size_t length = <%u>, const char *format = <%s>", string, length, format)

    // We will use vsnprintf to be able to pass the va_args:

    va_list args;
	mcl_int32_t count;

    va_start(args, format);

    count = vsnprintf(string, length, format, args);
    va_end(args);

    if ((count > 0) && (((mcl_size_t)count) <= length))
    {
        DEBUG_LEAVE("retVal = <%d>", MCL_OK);
        return MCL_OK;
    }
    else
    {
        // There was a problem about writing the string :
        MCL_ERROR_RETURN(MCL_FAIL, "Couldn't write the string!");
    }
}

mcl_bool_t string_util_memcmp(const void *block_1, const void *block_2, mcl_size_t count)
{
    DEBUG_ENTRY("const void *block_1 = <%p>, const void *block_2 = <%p>, mcl_size_t count = <%u>", block_1, block_2, count)

    mcl_bool_t result = (0 == memcmp(block_1, block_2, count)) ? MCL_TRUE : MCL_FALSE;

    DEBUG_LEAVE("retVal = <%s>", result ? "MCL_TRUE" : "MCL_FALSE");
    return result;
}

void string_util_memcpy(void *destination, const void *source, mcl_size_t count)
{
    DEBUG_ENTRY("void *destination = <%p>, const void *source = <%p>, mcl_size_t count = <%u>", destination, source, count)

    memcpy(destination, source, count);

    DEBUG_LEAVE("retVal = void");
}

char *string_util_strdup(const char *string)
{
    DEBUG_ENTRY("const char *string = <%s>", string)

    char *result;

#if defined(WIN32) || defined(WIN64)
    result = _strdup(string);
#else
    result = strdup(string);
#endif

    DEBUG_LEAVE("retVal = <%p>", result);
    return result;
}

mcl_bool_t string_util_find(const char *source, const char *target, mcl_size_t *start_index)
{
	DEBUG_ENTRY("char *src = <%p>, char *target = <%p>, mcl_size_t *start_index = <%p>", source, target, start_index)

	mcl_size_t index;
    mcl_size_t source_length;
    mcl_size_t target_length;
    mcl_bool_t is_found = MCL_FALSE;
    source_length = string_util_strlen(source);
    target_length = string_util_strlen(target);

    if (!(0 == target_length) && !(target_length > source_length))
    {
        mcl_size_t state = 0;

        for (index = 0; index < source_length; index++)
        {
            state = (source[index] == target[state]) ? state + 1 : 0;

            if (state == target_length)
            {
                *start_index = index - (target_length - 1);
                is_found = MCL_TRUE;

                break;
            }
        }
    }

    DEBUG_LEAVE("retVal = <%s>", is_found ? "MCL_TRUE" : "MCL_FALSE");
    return is_found;
}

mcl_bool_t string_util_find_case_insensitive(const char *source, const char *target, mcl_size_t *start_index)
{
	DEBUG_ENTRY("char *src = <%p>, char *target = <%p>, mcl_size_t *start_index = <%p>", source, target, start_index)

	mcl_size_t index;
    mcl_size_t source_length;
    mcl_size_t target_length;
    mcl_bool_t is_found = MCL_FALSE;
    source_length = string_util_strlen(source);
    target_length = string_util_strlen(target);

    if ((0 != target_length) && (target_length <= source_length))
    {
        mcl_size_t state = 0;

        for (index = 0; index < source_length; index++)
        {
            state = (source[index] == target[state] || LOWERCASE(source[index]) == LOWERCASE(target[state])) ? state + 1 : 0;

            if (state == target_length)
            {
                *start_index = index - (target_length - 1);
                is_found = MCL_TRUE;

                break;
            }
        }
    }

    DEBUG_LEAVE("retVal = <%s>", is_found ? "MCL_TRUE" : "MCL_FALSE");
    return is_found;
}

mcl_bool_t string_util_is_space(char character)
{
    DEBUG_ENTRY("char character = <%c>", character)

    mcl_bool_t result = (character == ' ') || (character == '\t') || (character == '\n') || (character == '\v') || (character == '\f') || (character == '\r');

    DEBUG_LEAVE("retVal = <%s>", result ? "MCL_TRUE" : "MCL_FALSE");
    return result;
}

long string_util_strtol(const char *source, int base, char **end_pointer)
{
    DEBUG_ENTRY("char* source = <%p>, int base = <%p>, char *end_pointer = <%p>", source, end_pointer, base)

    long result = strtol(source, end_pointer, base);

    DEBUG_LEAVE("retVal = <%u>", result);
    return result;
}
