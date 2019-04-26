/*!**********************************************************************
*
* @copyright Copyright (C) 2016 Siemens Aktiengesellschaft.\n
*            All rights reserved.
*
*************************************************************************
*
* @file     time_util.c
* @date     Dec 21, 2016
* @brief    Time utility module implementation file.
*
************************************************************************/

#include "time_util.h"
#include "definitions.h"
#include "log_util.h"
#include "memory.h"

E_MCL_ERROR_CODE time_util_convert_to_iso_8601_format(const time_t *time_value, string_t **iso8601_formatted_time)
{
    DEBUG_ENTRY("const time_t *time_value = <%p>, string_t **iso8601_formatted_time = <%p>", time_value, iso8601_formatted_time)
	
	E_MCL_ERROR_CODE return_code;
    struct tm *time_structure;

#if defined(WIN32) || defined(WIN64)
	errno_t result;
#else
	struct tm *temp;
#endif

	// Populate time structure for the given time value.
	MCL_NEW(time_structure);
	ASSERT_CODE(MCL_NULL != time_structure, MCL_OUT_OF_MEMORY);

#if defined(WIN32) || defined(WIN64)
	result = gmtime_s(time_structure, time_value);
	ASSERT_STATEMENT_CODE_MESSAGE(0 == result, MCL_FREE(time_structure), MCL_FAIL, "gmtime_s() function returns error.");
#else
    temp = gmtime_r(time_value, time_structure);
	ASSERT_STATEMENT_CODE_MESSAGE(MCL_NULL != temp, MCL_FREE(time_structure), MCL_FAIL, "gmtime_r() function returns null.");
#endif

    // Initialize a string for ISO 8601 date and time format.
    return_code = string_initialize_new(MCL_NULL, 25, iso8601_formatted_time);
    ASSERT_STATEMENT_CODE_MESSAGE(MCL_OK == return_code, MCL_FREE(time_structure), return_code, "Memory allocation failed for ISO 8601 formatted date and time.");

    // Compose the string in ISO 8601 date and time format.
    if (0 == strftime((*iso8601_formatted_time)->buffer, (*iso8601_formatted_time)->length, "%Y-%m-%dT%H:%M:%S.000Z", time_structure))
    {
        string_destroy(iso8601_formatted_time);
        MCL_DEBUG("File creation time can not be converted to ISO-8601 date and time format.");
        return_code = MCL_FAIL;
    }

	MCL_FREE(time_structure);
    DEBUG_LEAVE("retVal = <%d>", return_code);
    return return_code;
}

void time_util_get_time(mcl_time_t *current_time)
{
    DEBUG_ENTRY("mcl_time_t *current_time = <%p>", current_time)

    time(current_time);

    DEBUG_LEAVE("retVal = void");
}

// YYYY-MM-DDThh:mm:ss.sssZ Ex:2016-04-26T08:06:25.317Z
mcl_bool_t time_util_validate_timestamp(const char *timestamp)
{
	DEBUG_ENTRY("const char *timestamp = <%p>", timestamp)

	const char characters_to_check[7] = { '-', '-', 'T', ':', ':', '.', 'Z' };
	const uint16_t maximum_values[7] = { 2999, 12, 31, 23, 59, 59, 999 };
	const uint8_t character_indexes_to_check[7] = { 4, 7, 10, 13, 16, 19, 23 };
	const char *temporary_buffer = timestamp;
	char *end_pointer = MCL_NULL;
	mcl_bool_t ok;
	uint8_t index;

    // Check timestamp length.
    const mcl_size_t expected_timestamp_length = 24;
    mcl_size_t actual_timestamp_length = 0;

    actual_timestamp_length = string_util_strlen(timestamp);
    ok = expected_timestamp_length == actual_timestamp_length;

    // Check timestamp format.
    for (index = 0; index < 7; ++index)
    {
        ok = ok && characters_to_check[index] == timestamp[character_indexes_to_check[index]];
    }

	// Check date and time values.
    for (index = 0; index < 7; ++index)
    {
        // 10 is base.
        mcl_size_t temporary_value = string_util_strtol(temporary_buffer, 10, &end_pointer);

        ok = ok && maximum_values[index] >= temporary_value;

        if (index < 3)
        {
            // Date values can not be zero.
            ok = ok && temporary_value > 0;
        }

        // To show the address after the special character.
        temporary_buffer = end_pointer + 1;
    }

    DEBUG_LEAVE("retVal = <%d>", ok);
    return ok;
}
