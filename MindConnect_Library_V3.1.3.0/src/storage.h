/*!**********************************************************************
*
* @copyright Copyright (C) 2016 Siemens Aktiengesellschaft.\n
*            All rights reserved.
*
*************************************************************************
*
* @file     storage.h
* @date     Oct 6, 2016
* @brief    Storage module header file.
*
* This module contains the functions to be used for data storage if a data storage
* medium is present in the system.
*
************************************************************************/

#ifndef STORAGE_H_
#define STORAGE_H_

#include "string_type.h"

/**
 * This function is used to save registration information for shared secret security profile to a file.
 *
 * @param [in] path File path to save registration information.
 * @param [in] client_id Client ID.
 * @param [in] client_secret Client secret.
 * @param [in] registration_access_token Registration access token.
 * @param [in] registration_uri Registration URI.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_REGISTRATION_INFO_IS_NOT_SAVED in case of an error in saving.</li>
 * </ul>
 */
E_MCL_ERROR_CODE storage_save_shared_secret_registration_information(string_t *path, string_t *client_id, string_t *client_secret, string_t *registration_access_token,
        string_t *registration_uri);

/**
 * This function is used to save registration information for rsa security profile to a file.
 *
 * @param [in] path File path to save registration information.
 * @param [in] client_id Client ID.
 * @param [in] public_key Public key.
 * @param [in] private_key Private key.
 * @param [in] registration_access_token Registration access token.
 * @param [in] registration_uri Registration URI.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_REGISTRATION_INFO_IS_NOT_SAVED in case of an error in saving.</li>
 * </ul>
 */
E_MCL_ERROR_CODE storage_save_rsa_registration_information(string_t *path, string_t *client_id, char *public_key, char *private_key, string_t *registration_access_token,
        string_t *registration_uri);

/**
 * This function is used to load registration information from the storage for Shared Secret security profile.
 *
 * @param [in] path File path of the saved registration information.
 * @param [out] client_id Client ID.
 * @param [out] client_secret Client secret.
 * @param [out] registration_access_token Registration access token.
 * @param [out] registration_uri Registration URI.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_NO_STORAGE_MEDIUM in case there is no storage medium to save.</li>
 * <li>#MCL_NO_FILE_SUPPORT in case the system has no support for file handling.</li>
 * <li>#MCL_REGISTRATION_INFO_IS_NOT_LOADED in case of an error in loading.</li>
 * </ul>
 */
E_MCL_ERROR_CODE storage_load_shared_secret_registration_information(string_t *path, string_t **client_id, string_t **client_secret, string_t **registration_access_token,
        string_t **registration_uri);

/**
 * This function is used to load registration information from the storage for RSA security profile.
 *
 * @param [in] path File path of the saved registration information.
 * @param [out] client_id Client ID.
 * @param [out] public_key Public key.
 * @param [out] private_key Private key.
 * @param [out] registration_access_token Registration access token.
 * @param [out] registration_uri Registration URI.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_NO_STORAGE_MEDIUM in case there is no storage medium to save.</li>
 * <li>#MCL_NO_FILE_SUPPORT in case the system has no support for file handling.</li>
 * <li>#MCL_REGISTRATION_INFO_IS_NOT_LOADED in case of an error in loading.</li>
 * </ul>
 */
E_MCL_ERROR_CODE storage_load_rsa_registration_information(string_t *path, string_t **client_id, char **public_key, char **private_key, string_t **registration_access_token,
        string_t **registration_uri);
#endif //STORAGE_H_
