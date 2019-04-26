/*!**********************************************************************
*
* @copyright Copyright (C) 2016 Siemens Aktiengesellschaft.\n
*            All rights reserved.
*
*************************************************************************
*
* @file     file_util.c
* @date     Oct 5, 2016
* @brief    File utility module implementation file.
*
************************************************************************/

#include "file_util.h"
#include "log_util.h"
#include "definitions.h"

#if !defined(S_ISREG) && defined(S_IFMT) && defined(S_IFREG)
#define S_ISREG(m) (((m) & S_IFMT) == S_IFREG)
#endif

E_MCL_ERROR_CODE file_util_fopen(const char *file_name, const char *mode, void **file_descriptor)
{
    DEBUG_ENTRY("const char *file_name = <%s>, const char *mode = <%s>, void **file_descriptor = <%p>", file_name, mode, file_descriptor)

    E_MCL_ERROR_CODE return_code = file_util_fopen_without_log(file_name, mode, file_descriptor);

    DEBUG_LEAVE("retVal = <%d>", return_code);
    return return_code;
}

E_MCL_ERROR_CODE file_util_fopen_without_log(const char *file_name, const char *mode, void **file_descriptor)
{
    E_MCL_ERROR_CODE return_code = MCL_OK;

    FILE *file;

#if defined(WIN32) || defined(WIN64)
    fopen_s(&file, file_name, mode);
#else
    file = fopen(file_name, mode);
#endif
    *file_descriptor = file;
    if (MCL_NULL == file)
    {
        return_code = MCL_FAIL;
    }

    return return_code;
}

E_MCL_ERROR_CODE file_util_fclose(void *file_descriptor)
{
    DEBUG_ENTRY("void *file_descriptor = <%p>", file_descriptor)

    E_MCL_ERROR_CODE return_code = file_util_fclose_without_log(file_descriptor);

    DEBUG_LEAVE("retVal = <%d>", return_code);
    return return_code;
}

E_MCL_ERROR_CODE file_util_fclose_without_log(void *file_descriptor)
{
    E_MCL_ERROR_CODE return_code = MCL_OK;

    if (MCL_NULL != file_descriptor)
    {
        mcl_int32_t result = fclose((FILE *)file_descriptor);
        if (0 != result)
        {
            return_code = MCL_FAIL;
        }
    }

    return return_code;
}

E_MCL_ERROR_CODE file_util_fwrite(const void *data, mcl_size_t size, mcl_size_t count, void *file_descriptor)
{
    DEBUG_ENTRY("const void *data = <%p>, mcl_size_t size = <%u>, mcl_size_t count = <%u>, void *file_descriptor = <%p>", data, size, count, file_descriptor)

    E_MCL_ERROR_CODE return_code = file_util_fwrite_without_log(data, size, count, file_descriptor);

    DEBUG_LEAVE("retVal = <%d>", return_code);
    return return_code;
}

E_MCL_ERROR_CODE file_util_fwrite_without_log(const void *data, mcl_size_t size, mcl_size_t count, void *file_descriptor)
{
    E_MCL_ERROR_CODE return_code = MCL_FAIL;

    mcl_size_t actual_count = fwrite(data, size, count, (FILE *)file_descriptor);
    if (count == actual_count)
    {
        return_code = MCL_OK;
    }
    else
    {
        MCL_ERROR("Data can not be written to file.");
    }

    return return_code;
}

void file_util_fread(void *data, mcl_size_t size, mcl_size_t count, void *file_descriptor, mcl_size_t *actual_count)
{
    DEBUG_ENTRY("void *data = <%p>, mcl_size_t size = <%u>, mcl_size_t count = <%u>, void *file_descriptor = <%p>, mcl_size_t *actual_count = <%p>", data, size, count,
                file_descriptor, actual_count)

    *actual_count = fread(data, size, count, (FILE *)file_descriptor);

    DEBUG_LEAVE("retVal = void");
}

E_MCL_ERROR_CODE file_util_fputs(const char *data, void *file_descriptor)
{
    DEBUG_ENTRY("const char *data = <%s>, void *file_descriptor = <%p>", data, file_descriptor)

    E_MCL_ERROR_CODE return_code = MCL_FAIL;

    mcl_int32_t result = fputs(data, (FILE *)file_descriptor);
    if (0 <= result)
    {
        return_code = MCL_OK;
    }
    else
    {
        MCL_ERROR("Data can not be written to file.");
    }

    DEBUG_LEAVE("retVal = <%d>", return_code);
    return return_code;
}

E_MCL_ERROR_CODE file_util_fgets(char *data, mcl_size_t data_size, void *file_descriptor)
{
    DEBUG_ENTRY("char *data = <%p>, mcl_size_t data_size = <%u>, void *file_descriptor = <%p>", data, data_size, file_descriptor)

    E_MCL_ERROR_CODE return_code = MCL_FAIL;

    char *output = fgets(data, (int)data_size, (FILE *)file_descriptor);
    if (MCL_NULL != output)
    {
        return_code = MCL_OK;
    }
    else
    {
        MCL_DEBUG("No data is read from file.");
    }

    DEBUG_LEAVE("retVal = <%d>", return_code);
    return return_code;
}

E_MCL_ERROR_CODE file_util_fstat(void *file_descriptor, struct stat *file_attributes)
{
    DEBUG_ENTRY("void *file_descriptor = <%p>, struct stat *file_attributes = <%p>", file_descriptor, file_attributes)

    E_MCL_ERROR_CODE return_code = MCL_FAIL;

    int result;

#if defined(WIN32) || defined(WIN64)
    result = fstat(_fileno((FILE *)file_descriptor), file_attributes);
#else
    result = fstat(fileno((FILE *)file_descriptor), file_attributes);
#endif

    if (0 == result)
    {
        return_code = MCL_OK;
    }
    else
    {
        MCL_ERROR("Error in retrieving file attributes.");
    }

    DEBUG_LEAVE("retVal = <%d>", return_code);
    return return_code;
}

E_MCL_ERROR_CODE file_util_fflush(void *file_descriptor)
{
    DEBUG_ENTRY("void *file_descriptor = <%p>", file_descriptor)

    E_MCL_ERROR_CODE return_code = file_util_fflush_without_log(file_descriptor);

    DEBUG_LEAVE("retVal = <%d>", return_code);
    return return_code;
}

E_MCL_ERROR_CODE file_util_fflush_without_log(void *file_descriptor)
{
    E_MCL_ERROR_CODE return_code = MCL_FAIL;

    int result;

#if defined(WIN32) || defined(WIN64)
    result = fflush((FILE *)file_descriptor);
#else
    result = fflush((FILE *)file_descriptor);
#endif

    if (0 == result)
    {
        return_code = MCL_OK;
    }
    else
    {
        MCL_ERROR("Error in flush.");
    }

    return return_code;
}

mcl_bool_t file_util_check_if_regular_file(const mcl_stat_t *file_attributes)
{
    DEBUG_ENTRY("const mcl_stat_t *file_attributes = <%p>", file_attributes)
    mcl_bool_t is_regular_file = MCL_FALSE;

#ifdef S_ISREG
    is_regular_file = S_ISREG(file_attributes->st_mode);
#endif

    DEBUG_LEAVE("retVal = <%d>", is_regular_file);
    return is_regular_file;
}
