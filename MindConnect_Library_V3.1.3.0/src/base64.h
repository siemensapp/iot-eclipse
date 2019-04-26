/*!**********************************************************************
 *
 * @copyright Copyright (C) 2016 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 *
 *************************************************************************
 *
 * @file     base64.h
 * @date     Aug 5, 2016
 * @brief    Base64 module header file.
 *
 * This module defines methods to calculate encoding, decoding, base64 and base64url for given data.
 *
 ************************************************************************/

#ifndef BASE64_H_
#define BASE64_H_

#include "string_type.h"

/**
 * Given a base64 zero-terminated string at @p encoded_data, decode it and return a
 * pointer in @p decoded_data to a newly allocated memory area holding decoded
 * data. Size of decoded data is returned in variable pointed by @p decoded_data_size.
 *
 * When decoded data length is 0, returns #MCL_NULL in @p decoded_data.
 *
 * @param encoded_data [in] Zero-terminated string which is base64 encoded and has to be decoded.
 * @param decoded_data [out] Newly allocated memory holding decoded data.
 * @param decoded_data_size [out] Size of decoded data.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_OUT_OF_MEMORY in case there is not enough memory in the system to proceed.</li>
 * <li>#MCL_BAD_CONTENT_ENCODING if provided @p encoded_data has invalid length (0 or not multiples of 4) or if the given @p encoded_data is invalidly encoded.</li>
 * </ul>
 */
E_MCL_ERROR_CODE base64_decode(const string_t *encoded_data, mcl_uint8_t **decoded_data, mcl_size_t *decoded_data_size);

/**
 * Given a base64 URL zero-terminated string at @p encoded_data, decode it and return a
 * pointer in @p decoded_data to a newly allocated memory area holding decoded
 * data. Size of decoded data is returned in variable pointed by @p decoded_data_size.
 *
 * When decoded data length is 0, returns #MCL_NULL in @p decoded_data.
 *
 * @param encoded_data [in] Zero-terminated string which is base64 URL encoded and has to be decoded.
 * @param decoded_data [out] Newly allocated memory holding decoded data.
 * @param decoded_data_size [out] Size of @p decoded_data.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_OUT_OF_MEMORY in case there is not enough memory in the system to proceed.</li>
 * <li>#MCL_BAD_CONTENT_ENCODING if provided @p encoded_data has invalid length (0 or not multiples of 4) or if the given @p encoded_data is invalidly encoded.</li>
 * </ul>
 */
E_MCL_ERROR_CODE base64_url_decode(const string_t *encoded_data, mcl_uint8_t **decoded_data, mcl_size_t *decoded_data_size);

/**
 * Given a pointer to an input data and an input size, encode it with base64 and
 * return a pointer in @p encoded_data to a newly allocated string area holding
 * encoded data. Length of encoded data is also returned.
 *
 * When encoded data length is 0, returns #MCL_NULL in @p encoded_data.
 *
 * @param data [in] Input data that has to be base64 encoded.
 * @param data_size [in] Size of given input @p data.
 * @param encoded_data [out] Newly allocated string holding encoded data and its length.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_OUT_OF_MEMORY in case there is not enough memory in the system to proceed.</li>
 * </ul>
 */
E_MCL_ERROR_CODE base64_encode(const mcl_uint8_t *data, mcl_size_t data_size, string_t **encoded_data);

/**
 * Given a pointer to an input data and an input size, encode it with base64 URL and
 * return a pointer in @p encoded_data to a newly allocated string area holding
 * encoded data. Length of encoded data is also returned.
 *
 * When encoded data length is 0, returns #MCL_NULL in @p encoded_data.
 *
 * @param data [in] Input data that has to be base64 URL encoded.
 * @param data_size [in] Size of given input @p data.
 * @param encoded_data [out] Newly allocated string holding encoded data and its length.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_OUT_OF_MEMORY in case there is not enough memory in the system to proceed.</li>
 * </ul>
 */
E_MCL_ERROR_CODE base64_url_encode(const mcl_uint8_t *data, mcl_size_t data_size, string_t **encoded_data);

#endif //BASE64_H_
