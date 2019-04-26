/*!**********************************************************************
 *
 * @copyright Copyright (C) 2016 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 *
 *************************************************************************
 *
 * @file     security.h
 * @date     Jun 27, 2016
 * @brief    Security module header file.
 * *
 * Inner security interface. Contains security related functions like hashing, public/private key generation.
 * Implementation file might change depending on the platform and will be in security_XXX.c format.
 *
 ************************************************************************/

#ifndef SECURITY_H_
#define SECURITY_H_

#include "mcl/mcl_common.h"

/**
 * Initializes security module. This function is used e.g. to set memory callback functions.
 */
void security_initialize(void);

/**
 * @see #security_handler_hash_sha256
 */
E_MCL_ERROR_CODE security_hash_sha256(const mcl_uint8_t *data, mcl_size_t data_size, mcl_uint8_t **hash, mcl_size_t *hash_size);

/**
 * @brief To be used to sign data with RSA key.
 *
 * Received key will be used to sign.
 *
 * @param [in] rsa_key Private key to be used in signing.
 * @param [in] data The data to be signed.
 * @param [in] data_size Size of the data.
 * @param [out] signature Generated signature.
 * @param [out] signature_size Size of signature.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_OUT_OF_MEMORY in case there is not enough memory in the system to proceed.</li>
 * <li>#MCL_FAIL in case of an internal error in MCL.</li>
 * </ul>
 */
E_MCL_ERROR_CODE security_rsa_sign(char *rsa_key, char *data, mcl_size_t data_size, mcl_uint8_t **signature, mcl_size_t *signature_size);

/**
 * @brief To be used to generate the RSA public/private keys.
 *
 * Generated keys will be returned as out parameters.
 *
 * @param [out] public_key Generated public key. New memory space will be allocated for this parameter. Ownership passed to caller. Caller must free the space.
 * @param [out] private_key Generated private key. New memory space will be allocated for this parameter. Ownership passed to caller. Caller must free the space.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_OUT_OF_MEMORY in case there is not enough memory in the system to proceed.</li>
 * <li>#MCL_FAIL in case of an internal error in MCL.</li>
 * </ul>
 */
E_MCL_ERROR_CODE security_generate_rsa_key(char **public_key, char **private_key);

/**
 * @brief To be used to get the modulus (n) and public exponent (e) parameters of RSA key in Base64 format.
 *
 * @param public_key Public key in PEM format.
 * @param modulus Base64 encoded modulus of RSA key.
 * @param exponent Base64 encoded public exponent of RSA key.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_OUT_OF_MEMORY in case there is not enough memory in the system to proceed.</li>
 * <li>#MCL_FAIL in case of an internal error in MCL.</li>
 * </ul>
 */
E_MCL_ERROR_CODE security_rsa_get_modulus_and_exponent(char *public_key, char **modulus, char **exponent);

/**
 * @brief To be used to generate random bytes.
 *
 * @param [out] buffer Buffer which will be filled with random bytes.
 * @param [in] size Size of the buffer.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_FAIL in case of an internal error in MCL.</li>
 * </ul>
 */
E_MCL_ERROR_CODE security_generate_random_bytes(unsigned char *buffer, mcl_size_t size);

#endif //SECURITY_H_
