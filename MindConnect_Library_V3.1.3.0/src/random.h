/*!**********************************************************************
*
* @copyright Copyright (C) 2016 Siemens Aktiengesellschaft.\n
*            All rights reserved.
*
*************************************************************************
*
* @file     random.h
* @date     Aug 15, 2016
* @brief    Random module header file.
*
* Random number generation functions are implemented in this module.
*
************************************************************************/

#ifndef RANDOM_H_
#define RANDOM_H_

#include "string_type.h"

/**
 * @brief Generates random integer number.
 *
 * @param [out] random_number Pointer to the random number generated.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_FAIL in case of an internal error in random number generation.</li>
 * </ul>
 */
E_MCL_ERROR_CODE random_generate_number(mcl_uint32_t *random_number);

/**
 * @brief Generates an array of random numbers.
 *
 * Generates an array in given @p size and fills with random numbers.
 *
 * @param [in] size The size of the array to be created.
 * @return Returns the generated array of random numbers.
 */
mcl_uint8_t *random_generate_array(mcl_size_t size);

/**
 * @brief Generates random guid.
 *
 * @param [out] guid Random guid.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_OUT_OF_MEMORY in case there is not enough memory in the system to proceed.</li>
 * <li>#MCL_FAIL in case of an internal error in random number generation.</li>
 * </ul>
 */
E_MCL_ERROR_CODE random_generate_guid(string_t **guid);

#endif //RANDOM_H_
