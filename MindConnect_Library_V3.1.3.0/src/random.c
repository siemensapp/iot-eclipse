/*!**********************************************************************
*
* @copyright Copyright (C) 2016 Siemens Aktiengesellschaft.\n
*            All rights reserved.
*
*************************************************************************
*
* @file     random.c
* @date     Aug 15, 2016
* @brief    Random module implementation file.
*
************************************************************************/

#include "random.h"
#include "mcl/mcl_random.h"
#include "memory.h"
#include "security.h"
#include "definitions.h"
#include "log_util.h"

#if (1 == HAVE_STDLIB_H_)
#include <stdlib.h>
#endif

#define GUID_LENGTH 36

typedef union guid_t
{
    struct
    {
        mcl_uint32_t time_low;            //!< Lowest significant 32 bits of time.
        mcl_uint16_t time_mid;            //!< Middle 16 bits of time.
        mcl_uint16_t time_hi_and_version; //!< Version followed by 12 highest significant bits of time.
        mcl_uint8_t clk_seq_hi_res;       //!< Variant followed by highest bits of clock sequence.
        mcl_uint8_t clk_seq_low;          //!< Lowest significant bits of clock sequence.
        mcl_uint8_t node[6];              //!< Node ID.
    };

    mcl_uint8_t rnd[16];                  //!< Random bytes.
} guid_t;

E_MCL_ERROR_CODE mcl_random_generate_guid(char **guid)
{
    DEBUG_ENTRY("char **guid = <%p>", guid)

	E_MCL_ERROR_CODE code;
	string_t *guid_string = MCL_NULL;

    ASSERT_NOT_NULL(guid);

    code = random_generate_guid(&guid_string);
    ASSERT_CODE_MESSAGE(MCL_OK == code, code, "GUID can not be generated.");

    *guid = guid_string->buffer;
    MCL_FREE(guid_string);

    DEBUG_LEAVE("retVal = <%d>", MCL_OK);
    return MCL_OK;
}

E_MCL_ERROR_CODE random_generate_number(mcl_uint32_t *random_number)
{
    DEBUG_ENTRY("mcl_uint32_t *random_number = <%p>", random_number)

    E_MCL_ERROR_CODE code = security_generate_random_bytes((unsigned char *)random_number, sizeof(mcl_uint32_t));

    DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

mcl_uint8_t *random_generate_array(mcl_size_t size)
{
	DEBUG_ENTRY("mcl_size_t size = <%u>", size)

	E_MCL_ERROR_CODE result;
    mcl_uint8_t *array = MCL_MALLOC(size);
    ASSERT_MESSAGE(MCL_NULL != array, "Not enough memory to allocate random array!");

    result = security_generate_random_bytes(array, size);
    ASSERT_STATEMENT_MESSAGE(MCL_OK == result, MCL_FREE(array), "security_generate_random_bytes() function fail.");

    DEBUG_LEAVE("retVal = <%p>", array);
    return array;
}

E_MCL_ERROR_CODE random_generate_guid(string_t **guid)
{
    E_MCL_ERROR_CODE code;
    guid_t guid_content;
	char *guid_string;

    DEBUG_ENTRY("string_t **guid = <%p>", guid)

    guid_string = MCL_MALLOC(GUID_LENGTH + 1);
    ASSERT_CODE_MESSAGE(MCL_NULL != guid_string, MCL_OUT_OF_MEMORY, "Memory can not be allocated for GUID string.");

    code = security_generate_random_bytes(guid_content.rnd, sizeof(guid_content));

    if (MCL_OK == code)
    {
        guid_content.clk_seq_hi_res = (mcl_uint8_t)((guid_content.clk_seq_hi_res & 0x3F) | 0x80);
        guid_content.time_hi_and_version = (mcl_uint16_t)((guid_content.time_hi_and_version & 0x0FFF) | 0x4000);

        code = string_util_snprintf(guid_string, GUID_LENGTH + 1, "%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x", guid_content.time_low, guid_content.time_mid,
                                    guid_content.time_hi_and_version, guid_content.clk_seq_hi_res, guid_content.clk_seq_low, guid_content.node[0], guid_content.node[1],
                                    guid_content.node[2], guid_content.node[3], guid_content.node[4], guid_content.node[5]);
        ASSERT_STATEMENT_CODE_MESSAGE(MCL_OK == code, MCL_FREE(guid_string), code, "GUID structure can not be printed as string.");

        code = string_initialize_dynamic(guid_string, GUID_LENGTH, guid);
        ASSERT_STATEMENT_CODE_MESSAGE(MCL_OK == code, MCL_FREE(guid_string), code, "GUID string can not be initialized.");
    }
    else
    {
        MCL_FREE(guid_string);
    }

    DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}
