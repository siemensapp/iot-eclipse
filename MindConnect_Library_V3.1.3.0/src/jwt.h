/*!**********************************************************************
 *
 * @copyright Copyright (C) 2016 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 *
 *************************************************************************
 *
 * @file     jwt.h
 * @date     Jun 28, 2016
 * @brief    JWT module header file.
 *
 * This module performs JWT operations : generates onboarding and authentication JWT's.
 *
 ************************************************************************/

#ifndef JWT_H_
#define JWT_H_

#include "security_handler.h"
#include "json_util.h"

/**
 * @brief JWT Module handler
 */
typedef struct jwt_t
{
    json_t *header;                          //!< Header of jwt.
    json_t *payload;                         //!< Payload of jwt.
    security_handler_t *security_handler;    //!< Security handler.
    E_MCL_SECURITY_PROFILE security_profile; //!< Security profile.
    mcl_time_t issued_at;                    //!< Time of issue.
} jwt_t;

/**
 * @brief JWT Initializer
 * @param [in] security_handler Already initialized security handler. All JWT operations will be done using this object.
 * @param [in] security_profile Onboarding security profile. Important to get correct kind of authentication JWT.
 * @param [in] tenant Tenant which will be set in JWT payload.
 * @param [out] jwt The newly initialized jwt handler.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_OUT_OF_MEMORY in case there is not enough memory in the system to proceed.</li>
 * <li>#MCL_FAIL in case JSON object can not be created.</li>
 * </ul>
 */
E_MCL_ERROR_CODE jwt_initialize(security_handler_t *security_handler, E_MCL_SECURITY_PROFILE security_profile, string_t *tenant, jwt_t **jwt);

/**
 * @brief Used to generate the JWT Token as json string.
 *
 * @param [in] jwt JWT Handler.
 * @return Pointer to the JWT token of type #string_t or NULL in case of an error.
 */
string_t *jwt_get_token(jwt_t *jwt);

/**
 * @brief To destroy the JWT Handler.
 *
 * Will release the resources of JWT Handler. After destroy operation, handler shouldn't be used.
 *
 * @param [in] jwt JWT Handler to destroy
 */
void jwt_destroy(jwt_t **jwt);

#endif //JWT_H_
