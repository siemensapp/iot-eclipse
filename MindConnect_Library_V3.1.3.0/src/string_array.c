/*!**********************************************************************
*
* @copyright Copyright (C) 2016 Siemens Aktiengesellschaft.\n
*            All rights reserved.
*
*************************************************************************
*
* @file     string_array.c
* @date     Aug 19, 2016
* @brief    String array module implementation file.
*
************************************************************************/

#include "string_array.h"
#include "memory.h"
#include "definitions.h"
#include "log_util.h"

E_MCL_ERROR_CODE string_array_initialize(mcl_size_t count, string_array_t **array)
{
	VERBOSE_ENTRY("mcl_size_t count = <%u>, string_array_t **array = <%p>", count, array)

	string_array_t *new_array;
	mcl_size_t index;

    ASSERT_CODE_MESSAGE(0 < count, MCL_INVALID_PARAMETER, "Count cannot be zero.");

    MCL_NEW(*array);
    ASSERT_CODE_MESSAGE(MCL_NULL != *array, MCL_OUT_OF_MEMORY, "Not enough memory to allocate for array!");

    new_array = *array;

    // initialize the new array :
    new_array->increment_value = DEFAULT_INCREMENT_VALUE;
    new_array->count = count;
    new_array->index = 0;
    new_array->total_length = 0;

    // not used calloc here since the items will be initialized below:
    new_array->items = MCL_MALLOC(count * sizeof(string_array_item_t));
    if (MCL_NULL == new_array->items)
    {
        MCL_FREE(*array);
        MCL_ERROR_RETURN(MCL_OUT_OF_MEMORY, "Not enough memory to allocate array item !");
    }

    // items has been allocated. Now intiialize its items :
    for (index = 0; index < count; index++)
    {
        new_array->items[index].string = MCL_NULL;
        new_array->items[index].destroy = MCL_FALSE;
    }

    VERBOSE_LEAVE("retVal = <%d>", MCL_OK);
    return MCL_OK;
}

E_MCL_ERROR_CODE string_array_set_increment(string_array_t *array, mcl_size_t increment_value)
{
    DEBUG_ENTRY("string_array_t *array = <%p>, mcl_size_t increment_value = <%u>", array, increment_value)

    ASSERT_CODE_MESSAGE(0 < increment_value, MCL_INVALID_PARAMETER, "Increment value cannot be equal to 0.");

    array->increment_value = increment_value;

    DEBUG_LEAVE("retVal = <%d>", MCL_OK);
    return MCL_OK;
}

E_MCL_ERROR_CODE string_array_add(string_array_t *array, string_t *string, mcl_bool_t destroy)
{
    DEBUG_ENTRY("string_array_t *array = <%p>, string_t *string = <%p>, mcl_bool_t destroy = <%u>", array, string, destroy)

    // check if array is full ( not expecting the index > count case but still used >= instead of ==. Feels more safe):
    if (array->index >= array->count)
    {
		mcl_size_t new_count;
		mcl_size_t new_size;
		mcl_size_t index;

        MCL_DEBUG("Array is full. It will be resized by increment value");

        // if somehow increment value is 0, reset :
        if (0 == array->increment_value)
        {
            array->increment_value = DEFAULT_INCREMENT_VALUE;
        }

        new_count = array->count + array->increment_value;
        new_size = new_count * sizeof(string_array_item_t);
        MCL_RESIZE(array->items, new_size);
        ASSERT_CODE_MESSAGE(MCL_NULL != array->items, MCL_OUT_OF_MEMORY, "Array items couldn't be resized!");

        for (index = array->count; index < new_count; index++)
        {
            array->items[index].string = MCL_NULL;
            array->items[index].destroy = MCL_FALSE;
        }

        array->count = new_count;

        // items size have to be bigger now. No need to check again for index<count
    }

    array->items[array->index].string = string;
    array->items[array->index].destroy = destroy;
    array->index++;
    array->total_length += string->length;

    DEBUG_LEAVE("retVal = <%d>", MCL_OK);
    return MCL_OK;
}

string_t *string_array_get(string_array_t *array, mcl_size_t index)
{
	DEBUG_ENTRY("string_array_t *array = <%p>, mcl_size_t index = <%u>", array, index)
	
	string_t *string_to_return;

    ASSERT_MESSAGE(MCL_NULL != array, "Received array is NULL!");
    ASSERT_MESSAGE(MCL_NULL != array->items, "Received array's item array is NULL!");

    // index should be less than array->index not array->count !!
    ASSERT_MESSAGE(index < array->index, "Received index is out of bounds!");

    string_to_return = array->items[index].string;

    DEBUG_LEAVE("retVal = <%p>", string_to_return);
    return string_to_return;
}

string_t *string_array_to_string(string_array_t *array)
{
	DEBUG_ENTRY("string_array_t *array = <%p>", array)
	
	char *final_buffer;
	char *temp_buffer;
	mcl_size_t index;
	string_t *final_string = MCL_NULL;
	
    ASSERT_MESSAGE(MCL_NULL != array, "Received array is NULL!");
    ASSERT_MESSAGE(MCL_NULL != array->items, "Received array's item array is NULL!");
    ASSERT_MESSAGE(0 != array->total_length, "Received array's total length is 0!");

    // allocate buffer with total lenght
    final_buffer = MCL_MALLOC(array->total_length + 1);
    ASSERT_MESSAGE(MCL_NULL != final_buffer, "Not enough memory for final_buffer!");
    final_buffer[0] = MCL_NULL_CHAR;

    // this is going to be used to increase strncat performance
    temp_buffer = final_buffer;

    for (index = 0; index < array->index; index++)
    {
        string_util_strncat(temp_buffer, array->items[index].string->buffer, array->items[index].string->length);

        // increasing temp_buffer to make it point to the last null char put by strncat. This way in the next call to strncat,
        // it is not going to search for nullchar from the beginning of the string. This will increase performance :
        temp_buffer += array->items[index].string->length;
        MCL_DEBUG("String at index <%d> has been added to the final buffer = <%s>", index, final_buffer);
    }

    // initialize the string as dynamic : not re allocate again but will free the buffer during it's destroy :
    if (MCL_OK != string_initialize_dynamic(final_buffer, array->total_length, &final_string))
    {
        MCL_FREE(final_buffer);
        MCL_ERROR_RETURN_POINTER(MCL_NULL, "string initialize operation has been failed for final_string!");
    }

    DEBUG_LEAVE("retVal = <%p>", final_string);
    return final_string;
}

void string_array_destroy(string_array_t **array)
{
    DEBUG_ENTRY("string_array_t **array = <%p>", array)

    string_array_t *local_array = *array;

    if (MCL_NULL != local_array)
    {
        if (MCL_NULL != local_array->items)
        {
			mcl_size_t index;

            for (index = 0; index < local_array->count; index++)
            {
                if (MCL_TRUE == local_array->items[index].destroy)
                {
                    // try to destroy it
                    string_destroy(&(local_array->items[index].string));
                    MCL_DEBUG("Current string item in string array has been destroyed.");
                }
                else
                {
                    MCL_DEBUG("Current string item has been added to array as not destroyable. Not calling destroy.");
                }
            }

            MCL_DEBUG("String items in the array has been released. Now freeing the array.");
            MCL_FREE(local_array->items);
        }
        MCL_FREE(*array);
    }

    DEBUG_LEAVE("retVal = void");
}
