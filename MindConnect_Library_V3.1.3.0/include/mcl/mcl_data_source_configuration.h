/*!**********************************************************************
*
* @copyright Copyright (C) 2017 Siemens Aktiengesellschaft.\n
*            All rights reserved.
*
*************************************************************************
*
* @file     mcl_data_source_configuration.h
* @date     Nov 13, 2017
* @brief    Data source configuration module interface header file.
*
* Meta and payload parts of data source configuration type are initialized and set by this module.
*
************************************************************************/

#ifndef MCL_DATA_SOURCE_CONFIGURATION_H_
#define MCL_DATA_SOURCE_CONFIGURATION_H_

#include "mcl/mcl_common.h"
#include "mcl/mcl_random.h"

#ifdef  __cplusplus
extern "C"
{
#endif

    /**
     * This struct is used for building the data source configuration type.
     */
    typedef struct mcl_data_source_configuration_t mcl_data_source_configuration_t;

    /**
     * This struct is used for building the data source type.
     */
    typedef struct mcl_data_source_t mcl_data_source_t;

    /**
     * This struct is used for json handling.
     */
    typedef struct mcl_json_t mcl_json_t;

    /**
     * Sets the list of data sources the @p data_source_configuration encloses.
     *
     * @param [in] data_source_configuration Data source configuration object handle.
     * @param [in] name Name of the @p data_source.
     * @param [in] description Description of the @p data_source. This parameter is optional and can be NULL.
     * @param [in] custom_data Custom data of the @p data_source. New memory space will be allocated for this parameter.
     * Ownership passed to caller. Caller must free the space. This parameter is optional and can be NULL.
     * @param [out] data_source Data source the @p data_source_configuration encloses.
     * @return
     * <ul>
     * <li>#MCL_OK in case of success.</li>
     * <li>#MCL_TRIGGERED_WITH_NULL if @p data_source_configuration, @p name or @p data_source is NULL.</li>
     * <li>#MCL_OUT_OF_MEMORY in case there is not enough memory in the system to proceed.</li>
     * <li>#MCL_LIMIT_EXCEEDED in case There is no space for @p data_source.</li>
     * </ul>
     */
    extern MCL_EXPORT E_MCL_ERROR_CODE mcl_data_source_configuration_add_data_source(mcl_data_source_configuration_t *data_source_configuration, const char *name,
            const char *description, mcl_json_t *custom_data, mcl_data_source_t **data_source);

    /**
     * Sets the list of data points held by the @p data_source.
     *
     * @param [in] data_source Data source the data_source_configuration encloses.
     * @param [in] id Agent-unique identifier of the data point.
     * @param [in] name Name of the data point.
     * @param [in] description Description of the data point. This parameter is optional and can be NULL.
     * @param [in] type Data type of the data point.
     * @param [in] unit Measurement unit of the data point.
     * @param [in] custom_data Custom data of the data point. New memory space will be allocated for this parameter.
     * Ownership passed to caller. Caller must free the space. This parameter is optional and can be NULL.
     * @return
     * <ul>
     * <li>#MCL_OK in case of success.</li>
     * <li>#MCL_TRIGGERED_WITH_NULL if @p data_source, @p id, @p name, @p type or @p unit is NULL.</li>
     * <li>#MCL_OUT_OF_MEMORY in case there is not enough memory in the system to proceed.</li>
     * <li>#MCL_LIMIT_EXCEEDED in case @p data_source has no space for a new data point.</li>
     * </ul>
     */
    extern MCL_EXPORT E_MCL_ERROR_CODE mcl_data_source_configuration_add_data_point(mcl_data_source_t *data_source, const char *id, const char *name, const char *description,
            const char *type, const char *unit, mcl_json_t *custom_data);

    /**
     * Returns @p id of @p data_source_configuration.
     *
     * Memory allocation of @p id is done by the function. But the responsibility of freeing @p id is on the user.
     *
     * @param [in] data_source_configuration Data source configuration object handle.
     * @param [out] id Unique identifier of the configuration.
     * @return
     * <ul>
     * <li>#MCL_OK in case of success.</li>
     * <li>#MCL_TRIGGERED_WITH_NULL if provided @p data_model or @p configuration_id is NULL.</li>
     * <li>#MCL_OUT_OF_MEMORY in case there is not enough memory in the system to proceed.</li>
     * </ul>
     */
    extern MCL_EXPORT E_MCL_ERROR_CODE mcl_data_source_configuration_get_id(mcl_data_source_configuration_t *data_source_configuration, char **id);


#ifdef  __cplusplus
}
#endif

#endif //MCL_DATA_SOURCE_CONFIGURATION_H_
