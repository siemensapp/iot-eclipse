/*!**********************************************************************
 *
 * @copyright Copyright (C) 2016 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 *
 *************************************************************************
 *
 * @file     mcl_store.h
 * @date     Jul 15, 2016
 * @brief    Store module interface header file.
 *
 * Store is the container for all types of data to be exchanged using MCL.
 *
 ************************************************************************/

#ifndef MCL_STORE_H_
#define MCL_STORE_H_

#include "mcl/mcl_time_series.h"
#include "mcl/mcl_custom_data.h"
#include "mcl/mcl_stream_data.h"
#include "mcl/mcl_data_source_configuration.h"
#include "mcl/mcl_json_util.h"
#include "mcl/mcl_event.h"

#ifdef  __cplusplus
extern "C"
{
#endif

    /**
     * This struct holds references to data to exchange via communication interface.
     */
    typedef struct mcl_store_t mcl_store_t;

	/**
	* @brief This struct is used for building the complete message of file.
	*/
    typedef struct mcl_file_t mcl_file_t;

    /**
     * This function creates and initializes an object of type #mcl_store_t.
     *
     * Store is used to hold different types of data. These data will be processed later by mcl_communication module
     * to perform exchange operation.
     * A store can be initialized as streamable or non-streamable (@p streamable). During exchange operation,
     * data of a Non-Streamable store will be sent to MindSphere using a normal HTTP message, while the data of a
     * streamable store will be sent using chunked Transfer-Encoding.
     *
     * @param [in] streamable Indicates if the content of this store will be exchanged using chunked Transfer-Encoding or not.
     * @param [out] store The newly initialized store.
     * @return
     * <ul>
     * <li>#MCL_OK in case of success.</li>
     * <li>#MCL_TRIGGERED_WITH_NULL in case @p store is NULL.</li>
     * <li>#MCL_OUT_OF_MEMORY in case there is not enough memory in the system to proceed.</li>
     * <li>#MCL_FAIL in case initialization of store fails.</li>
     * </ul>
     */
    extern MCL_EXPORT E_MCL_ERROR_CODE mcl_store_initialize(mcl_bool_t streamable, mcl_store_t **store);

    /**
     * This function destroys the mcl_store_t object and frees any memory allocated.
     *
     * @param [in] store Preinitialized #mcl_store_t object to destroy.
     * @return
     * <ul>
     * <li>#MCL_OK in case of success.</li>
     * <li>#MCL_TRIGGERED_WITH_NULL in case @p store is NULL.</li>
     * </ul>
     */
    extern MCL_EXPORT E_MCL_ERROR_CODE mcl_store_destroy(mcl_store_t **store);

    /**
     * This function creates and initializes a new #mcl_time_series_t structure.
     *
     * @param [in] store MCL store which will contain the new #mcl_time_series_t structure created.
     * @param [in] version Version of the time series data. Format must be in the form of MAJOR.MINOR where major and minor are non-negative integers.
     * @param [in] configuration_id Configuration ID of the time series data.
     * @param [in] routing Routing information which is optional. NULL can be given to use the default parser.
     * @param [out] time_series Created and initialized time series data struct.
     * @return
     * <ul>
     * <li>#MCL_OK in case of success.</li>
     * <li>#MCL_TRIGGERED_WITH_NULL in case any mandatory argument is NULL.</li>
     * <li>#MCL_OUT_OF_MEMORY in case there is not enough memory in the system to proceed.</li>
     * <li>#MCL_LIMIT_EXCEEDED in case there is no space to create new #mcl_time_series_t in @p store.</li>
     * <li>#MCL_INVALID_PARAMETER if version format is not correct.</li>
     * </ul>
     */
    extern MCL_EXPORT E_MCL_ERROR_CODE mcl_store_new_time_series(mcl_store_t *store, const char *version, const char *configuration_id, const char *routing,
            mcl_time_series_t **time_series);

    /**
     * This function creates and initializes a new #mcl_event_t structure.
     *
     * @param [in] store MCL store which will contain the new #mcl_event_t structure created.
     * @param [in] version Version of event. Format must be in the form of MAJOR.MINOR where major and minor are non-negative integers.
     * @param [in] type Type of event.
     * @param [in] type_version Version of event type.
     * @param [in] severity Severity level declared in #E_MCL_EVENT_SEVERITY.
     * @param [in] timestamp Timestamp of the event in YYYY-MM-DDThh:mm:ss.sssZ format. Ex:2016-04-26T08:06:25.317Z.
     * @param [out] event Created and initialized event data struct.
     * @return
     * <ul>
     * <li>#MCL_OK in case of success.</li>
     * <li>#MCL_TRIGGERED_WITH_NULL in case any mandatory argument is NULL.</li>
     * <li>#MCL_OUT_OF_MEMORY in case there is not enough memory in the system to proceed.</li>
     * <li>#MCL_LIMIT_EXCEEDED in case there is no space to create new #mcl_event_t in @p store.</li>
     * <li>#MCL_INVALID_PARAMETER if version format is not correct.</li>
     * </ul>
     */
    extern MCL_EXPORT E_MCL_ERROR_CODE mcl_store_new_event(mcl_store_t *store, const char *version, const char *type, const char *type_version, E_MCL_EVENT_SEVERITY severity, const char *timestamp, mcl_event_t **event);

    /**
     * This function creates and initializes a new #mcl_file_t structure.
     *
     * @param [in] store MCL store which will contain the new #mcl_file_t structure created.
     * @param [in] version Version of file type. Format must be in the form of MAJOR.MINOR where major and minor are non-negative integers.
     * @param [in] file_path Path of the file to be added to store.
	 * @param [in] file_name Name of the file. This name will be assigned to the file that is being uploaded.
     * @param [in] file_type Type of the file.
     * @param [in] routing Routing information which is optional. NULL can be given to use the default parser.
     * @param [out] file Created and initialized file data struct.
     * @return
     * <ul>
     * <li>#MCL_OK in case of success.</li>
     * <li>#MCL_TRIGGERED_WITH_NULL in case any mandatory argument is NULL.</li>
     * <li>#MCL_OUT_OF_MEMORY in case there is not enough memory in the system to proceed.</li>
     * <li>#MCL_LIMIT_EXCEEDED in case there is no space to create new #mcl_file_t in @p store.</li>
     * <li>#MCL_FILE_CANNOT_BE_OPENED in case file does not exist or is not a regular file.</li>
     * <li>#MCL_INVALID_PARAMETER if version format is not correct.</li>
     * <li>#MCL_OPERATION_IS_NOT_SUPPORTED in case file size exceeds the maximum file size for upload operation which is 10 MB.</li>
     * <li>#MCL_FAIL in case of an internal error in MCL.</li>
     * </ul>
     */
    extern MCL_EXPORT E_MCL_ERROR_CODE mcl_store_new_file(mcl_store_t *store, const char *version, const char *file_path, const char *file_name, const char *file_type, const char *routing, mcl_file_t **file);

    /**
     * This function creates and initializes a new #mcl_custom_data_t structure.
     *
     * @param [in] store MCL store which will contain the new mcl_custom_data_t structure created.
     * @param [in] version Version of the custom_data.
     * @param [in] type Type of the custom_data.
     * @param [in] routing Routing information which is optional. NULL can be given to use the default parser.
     * @param [out] custom_data Created and initialized #mcl_custom_data_t object.
     * @return
     * <ul>
     * <li>#MCL_OK in case of success.</li>
     * <li>#MCL_TRIGGERED_WITH_NULL in case any mandatory argument is NULL.</li>
     * <li>#MCL_OUT_OF_MEMORY in case there is not enough memory in the system to proceed.</li>
     * <li>#MCL_LIMIT_EXCEEDED in case there is no space to create new #mcl_custom_data_t in @p store.</li>
     * <li>#MCL_INVALID_PARAMETER if version format is not correct.</li>
     * </ul>
     */
    extern MCL_EXPORT E_MCL_ERROR_CODE mcl_store_new_custom_data(mcl_store_t *store, const char *version, const char *type, const char *routing,
            mcl_custom_data_t **custom_data);

    /**
     * This function creates and initializes a new streamable mcl_stream_data_t structure.
     *
     * With this data type, user can specify a callback and context to fill the payload of the custom data.
     * The callback will be called while performing the send operation.
     *
     * @warning The store must be initialized as 'streamable' to be able to add this type of item.
     *
     * @param [in] store MCL store which will contain the new mcl_stream_data_t structure created.
     * @param [in] version Version of the stream_data. Format must be in the form of MAJOR.MINOR where major and minor are non-negative integers.
     * @param [in] type Type of the stream_data.
     * @param [in] routing Routing information which is optional. NULL can be given to use the default parser.
     * @param [in] stream_data_read_callback Stream data read callback function.
     * @param [in] user_context User context used to sent to the callback function.
     * @param [out] stream_data Created and initialized stream_data data struct.
     * @return
     * <ul>
     * <li>#MCL_OK in case of success.</li>
     * <li>#MCL_TRIGGERED_WITH_NULL in case @p store, @p version, @p type or @p stream_data is NULL.</li>
     * <li>#MCL_OUT_OF_MEMORY in case there is not enough memory in the system to proceed.</li>
     * <li>#MCL_INVALID_PARAMETER in case @p store is not streamable or version format is not correct.</li>
     * <li>#MCL_LIMIT_EXCEEDED in case there is no space to create new #mcl_stream_data_t in @p store.</li>
     * </ul>
     */
    extern MCL_EXPORT E_MCL_ERROR_CODE mcl_store_new_stream_data(mcl_store_t *store, const char *version, const char *type, const char *routing,
            mcl_stream_data_read_callback_t stream_data_read_callback, void *user_context, mcl_stream_data_t **stream_data);

    /**
     * This function creates and initializes a new #mcl_data_source_configuration_t structure.
     *
     * @param [in] store MCL store which will contain the new mcl_data_source_configuration_t structure created.
     * @param [in] version Version of the @p data_source_configuration. Format must be in the form of MAJOR.MINOR where major and minor are non-negative integers.
     * @param [out] data_source_configuration Created and initialized #mcl_data_source_configuration_t object.
     * @return
     * <ul>
     * <li>#MCL_OK in case of success.</li>
     * <li>#MCL_TRIGGERED_WITH_NULL in case any mandatory argument is NULL.</li>
     * <li>#MCL_OUT_OF_MEMORY in case there is not enough memory in the system to proceed.</li>
     * <li>#MCL_LIMIT_EXCEEDED in case there is no space to create new #mcl_data_source_configuration_t in @p store.</li>
     * <li>#MCL_INVALID_PARAMETER if version format is not correct.</li>
     * </ul>
     */
    extern MCL_EXPORT E_MCL_ERROR_CODE mcl_store_new_data_source_configuration(mcl_store_t *store, const char *version, mcl_data_source_configuration_t **data_source_configuration);

#ifdef  __cplusplus
}
#endif

#endif //MCL_STORE_H_
