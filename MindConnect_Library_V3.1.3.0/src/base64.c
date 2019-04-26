/*!**********************************************************************
 *
 * @copyright Copyright (C) 2016 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 *
 *************************************************************************
 *
 * @file     base64.c
 * @date     Aug 5, 2016
 * @brief    Base64 module implementation file.
 *
 ************************************************************************/

#include "base64.h"
#include "memory.h"
#include "log_util.h"
#include "definitions.h"

// define padding char
#define PADDING_CHAR '='

// define size of a quantum (4 Bytes data group)
#define QUANTUM_SIZE 4
#define INPUT_GROUP_SIZE 3

// only maximum of 2 padding chars are allowed
#define UPPER_COUNT_PADDING 3

// Base64 encoding/decoding table
static const char base64_encode_table[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
//J-
static const mcl_uint8_t base64_decode_table[] =
{
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x3E, 0xFF, 0xFF, 0xFF, 0x3F,
    0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E,
    0x0F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28,
    0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F, 0x30, 0x31, 0x32, 0x33, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
};
//J+

// The Base 64 encoding with an URL and filename safe alphabet, RFC 4648 section 5
static const char base64_url_encode_table[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_";
//J-
static const mcl_uint8_t base64_url_decode_table[] =
{ 
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x3E, 0xFF, 0xFF,
    0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E,
    0x0F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0xFF, 0xFF, 0xFF, 0xFF, 0x3F,
    0xFF, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28,
    0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F, 0x30, 0x31, 0x32, 0x33, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
};
//J+

// Private functions
static E_MCL_ERROR_CODE _encode_with_table(const char *table, const mcl_uint8_t *data, mcl_size_t data_size, string_t **encoded_data);
static E_MCL_ERROR_CODE _decode_with_table(const mcl_uint8_t *table, const string_t *encoded_data, mcl_uint8_t **decoded_data, mcl_size_t *decoded_data_size);
static E_MCL_ERROR_CODE _decode_quantum(const mcl_uint8_t *table, const char *source, mcl_uint8_t *destination, mcl_size_t *padding);

E_MCL_ERROR_CODE base64_decode(const string_t *encoded_data, mcl_uint8_t **decoded_data, mcl_size_t *decoded_data_size)
{
    DEBUG_ENTRY("const string_t *encoded_data = <%p>, mcl_uint8_t **decoded_data = <%p>, mcl_size_t *decoded_data_size = <%p>", encoded_data, decoded_data, decoded_data_size)

    E_MCL_ERROR_CODE code = _decode_with_table(base64_decode_table, encoded_data, decoded_data, decoded_data_size);

    DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

E_MCL_ERROR_CODE base64_url_decode(const string_t *encoded_data, mcl_uint8_t **decoded_data, mcl_size_t *decoded_data_size)
{
    DEBUG_ENTRY("const string_t *encoded_data = <%p>, mcl_uint8_t **decoded_data = <%p>, mcl_size_t *decoded_data_size = <%p>", encoded_data, decoded_data, decoded_data_size)

    E_MCL_ERROR_CODE code = _decode_with_table(base64_url_decode_table, encoded_data, decoded_data, decoded_data_size);

    DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

E_MCL_ERROR_CODE base64_encode(const mcl_uint8_t *data, mcl_size_t data_size, string_t **encoded_data)
{
    DEBUG_ENTRY("const mcl_uint8_t *data = <%p>, mcl_size_t data_size = <%u>, string_t **encoded_data = <%p>", data, data_size, encoded_data)

    E_MCL_ERROR_CODE code = _encode_with_table(base64_encode_table, data, data_size, encoded_data);

    DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

E_MCL_ERROR_CODE base64_url_encode(const mcl_uint8_t *data, mcl_size_t data_size, string_t **encoded_data)
{
    DEBUG_ENTRY("const mcl_uint8_t *data = <%p>, mcl_size_t data_size = <%u>, string_t **encoded_data = <%p>", data, data_size, encoded_data)

    E_MCL_ERROR_CODE code = _encode_with_table(base64_url_encode_table, data, data_size, encoded_data);

    DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

static E_MCL_ERROR_CODE _decode_with_table(const mcl_uint8_t *table, const string_t *encoded_data, mcl_uint8_t **decoded_data, mcl_size_t *decoded_data_size)
{
    DEBUG_ENTRY("const mcl_uint8_t *table = <%p>, const string_t *encoded_data = <%p>, mcl_uint8_t **decoded_data = <%p>, mcl_size_t *decoded_data_size = <%p>", table,
        encoded_data, decoded_data, decoded_data_size)

    char *encoded_buffer;
    mcl_size_t number_of_quantums;
    mcl_size_t raw_length;
    mcl_uint8_t *decode_buffer;
    mcl_uint8_t *position;
    mcl_size_t index = 0;
    mcl_size_t padding = 0;

    // Check the length of the input string is valid
    ASSERT_CODE_MESSAGE((encoded_data->length > 0) && (encoded_data->length % QUANTUM_SIZE == 0), MCL_BAD_CONTENT_ENCODING,
                        "Length of encoded data must be positive and multiples of 4");

    *decoded_data = MCL_NULL;
    *decoded_data_size = 0;

    // Find the position of any = padding characters
    encoded_buffer = encoded_data->buffer;
    while ((PADDING_CHAR != encoded_buffer[index]) && (MCL_NULL_CHAR != encoded_buffer[index]))
    {
        index++;
    }
    MCL_DEBUG("Found first padding symbol '=' at index = <%u>", index);

    // A maximum of two = padding characters is allowed
    if (PADDING_CHAR == encoded_buffer[index])
    {
        padding++;
        if (PADDING_CHAR == encoded_buffer[index + 1])
        {
            padding++;
        }
    }
    MCL_DEBUG("Number of padding chars = <%u>", padding);

    // Check the = padding characters weren't part way through the input
    ASSERT_CODE_MESSAGE(index + padding == encoded_data->length, MCL_BAD_CONTENT_ENCODING, "Padding must be located at the end and only maximum of 2 padding chars allowed");

    // Calculate the number of quantums
    number_of_quantums = encoded_data->length / QUANTUM_SIZE;
    MCL_DEBUG("Number of quantums (4 Byte groups) = <%u>", number_of_quantums);

    // Calculate the size of the decoded string
    raw_length = (number_of_quantums * INPUT_GROUP_SIZE) - padding;
    MCL_DEBUG("Length of resulting string will be = <%u>", raw_length);

    // Allocate our buffer including room for a zero terminator
    decode_buffer = MCL_MALLOC(raw_length + 1);
    ASSERT_CODE_MESSAGE(MCL_NULL != decode_buffer, MCL_OUT_OF_MEMORY, "Memory for storing decode result couldn't be allocated!");

    position = decode_buffer;

    // Decode the quantums
    for (index = 0; index < number_of_quantums; index++)
    {
        MCL_DEBUG("Decoding quantum = <%u> at input data = <%s>", index, encoded_buffer);
        ASSERT_STATEMENT_CODE_MESSAGE(MCL_OK == _decode_quantum(table, encoded_buffer, position, &padding), MCL_FREE(decode_buffer), MCL_BAD_CONTENT_ENCODING,
                                      "Decode of quantum failed!");

        position += padding;
        encoded_buffer += QUANTUM_SIZE;
    }

    // Zero terminate
    *position = MCL_NULL_CHAR;

    // Return the decoded data
    *decoded_data = decode_buffer;
    *decoded_data_size = raw_length;

    MCL_DEBUG("decode_buffer = <%p>", decode_buffer);
    MCL_DEBUG("decoded_data_size = <%u>", *decoded_data_size);

    DEBUG_LEAVE("retVal = <%d>", MCL_OK);
    return MCL_OK;
}

static E_MCL_ERROR_CODE _decode_quantum(const mcl_uint8_t *table, const char *source, mcl_uint8_t *destination, mcl_size_t *padding)
{
    DEBUG_ENTRY("const mcl_uint8_t *table = <%p>, const char *source = <%s>, mcl_uint8_t *destination = <%p>, mcl_size_t *padding = <%p>", table, source, destination,
        padding)

    mcl_size_t index;
    const char *current_char = source;
    mcl_uint32_t data = 0;
    *padding = 0;
    for (index = 0; index < QUANTUM_SIZE; index++, current_char++)
    {
        if (*current_char == PADDING_CHAR)
        {
            data = (data << 6);
            (*padding)++;
            MCL_DEBUG("Padding char found. Current number of paddings = <%u>", *padding);
            ASSERT_CODE_MESSAGE(*padding < UPPER_COUNT_PADDING, MCL_BAD_CONTENT_ENCODING, "Padding exceeds allowed maximum number!");
        }
        else
        {
            mcl_uint8_t table_index;
            MCL_DEBUG("Look up current char = <%c> in table.", *current_char);

            table_index = table[(mcl_uint8_t)*current_char];
            ASSERT_CODE_MESSAGE(0xFF != table_index, MCL_BAD_CONTENT_ENCODING, "Current char in table not found!");

            MCL_DEBUG("Current char = <%c> in table found at index = <%u>", *current_char, table_index);
            data = (data << 6) + table_index;
        }
    }

    if (*padding < 1)
    {
        destination[2] = (char)(data & 0xFFUL);
    }

    data >>= 8;
    if (*padding < 2)
    {
        destination[1] = (char)(data & 0xFFUL);
    }

    data >>= 8;
    destination[0] = (char)(data & 0xFFUL);
    *padding = UPPER_COUNT_PADDING - (*padding);

    DEBUG_LEAVE("retVal = <%d>", MCL_OK);
    return MCL_OK;
}

static E_MCL_ERROR_CODE _encode_with_table(const char *table, const mcl_uint8_t *data, mcl_size_t data_size, string_t **encoded_data)
{
    DEBUG_ENTRY("const char *table = <%s>, const mcl_uint8_t *data = <%p>, mcl_size_t data_size = <%u>, string_t **encoded_data = <%p>", table, data, data_size, encoded_data)

    char *output;
    char *output_start;
    mcl_uint8_t input_buffer[INPUT_GROUP_SIZE];
    mcl_uint8_t output_buffer[QUANTUM_SIZE];

    // return pointer to new data, allocated memory
    output = MCL_MALLOC(data_size * QUANTUM_SIZE / INPUT_GROUP_SIZE + QUANTUM_SIZE);
    ASSERT_CODE_MESSAGE(MCL_NULL != output, MCL_OUT_OF_MEMORY, "Memory to store encoded data couldn't be allocated!");

    MCL_DEBUG("Start to encode.");

    output_start = output;

    while (data_size > 0)
    {
        // counts input data if available, o.w. used to add padding chars accordingly
        mcl_size_t input_parts = 0;
        mcl_size_t index;

        // Take next 3 bytes from input data if available, o.w. set to 0.
        for (index = 0; index < INPUT_GROUP_SIZE; index++)
        {
            if (data_size > 0)
            {
                input_parts++;
                input_buffer[index] = *data;
                data++;
                data_size--;
            }
            else
            {
                input_buffer[index] = 0;
            }
        }
        MCL_DEBUG("data_size = <%u> left to encode.", data_size);

        // now we have 3 bytes from input, calculate output quantum of 4 bytes
        output_buffer[0] = (mcl_uint8_t)((input_buffer[0] & 0xFC) >> 2);
        output_buffer[1] = (mcl_uint8_t)(((input_buffer[0] & 0x03) << 4) | ((input_buffer[1] & 0xF0) >> 4));
        output_buffer[2] = (mcl_uint8_t)(((input_buffer[1] & 0x0F) << 2) | ((input_buffer[2] & 0xC0) >> 6));
        output_buffer[3] = (mcl_uint8_t)(input_buffer[2] & 0x3F);

        // now we have output quantum, we can construct string with given transformation table (add also padding if required)
        output[0] = table[output_buffer[0]];
        output[1] = table[output_buffer[1]];
        switch (input_parts)
        {
            case 1 : // only one byte read
                output[2] = PADDING_CHAR;
                output[3] = PADDING_CHAR;

                break;
            case 2 : // two bytes read
                output[2] = table[output_buffer[2]];
                output[3] = PADDING_CHAR;

                break;
            default :
                output[2] = table[output_buffer[2]];
                output[3] = table[output_buffer[3]];

                break;
        }

        // set output to next 4 byte for next iteration
        output += 4;
        MCL_DEBUG("4 bytes written to output = <%u>", output - output_start);
    }

    // terminate the output
    *output = MCL_NULL_CHAR;

    // return the length of the new data
    string_initialize_dynamic(output_start, 0, encoded_data);

    MCL_DEBUG("End of encode reached. encoded_data = <%s>, length encoded_data = <%u>.", (*encoded_data)->buffer, (*encoded_data)->length);
    DEBUG_LEAVE("retVal = <%d>", MCL_OK);
    return MCL_OK;
}
