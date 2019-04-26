/*!**********************************************************************
*
* @copyright Copyright (C) 2016 Siemens Aktiengesellschaft.\n
*            All rights reserved.
*
*************************************************************************
*
* @file     list.c
* @date     Jul 20, 2016
* @brief    List module implementation file.
*
************************************************************************/

#include "list.h"
#include "memory.h"
#include "definitions.h"
#include "log_util.h"

E_MCL_ERROR_CODE mcl_list_initialize(mcl_list_t **list)
{
	DEBUG_ENTRY("mcl_list_t **list = <%p>", list)

	E_MCL_ERROR_CODE code;
    ASSERT_NOT_NULL(list);

    code = list_initialize(list);

    DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

E_MCL_ERROR_CODE mcl_list_add(mcl_list_t *list, void *data)
{
	DEBUG_ENTRY("mcl_list_t *list = <%p>, void *data = <%p>", list, data)

	E_MCL_ERROR_CODE code;
    ASSERT_NOT_NULL(list);
    ASSERT_NOT_NULL(data);

    code = list_add(list, data);

    DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

E_MCL_ERROR_CODE mcl_list_next(mcl_list_t *list, mcl_list_node_t **node)
{
    DEBUG_ENTRY("mcl_list_t *list = <%p>", list)

    ASSERT_NOT_NULL(list);
    ASSERT_NOT_NULL(node);

    *node = list_next(list);

    if (MCL_NULL != *node)
    {
        DEBUG_LEAVE("retVal = <%d>", MCL_OK);
        return MCL_OK;
    }
    else
    {
        DEBUG_LEAVE("retVal = <%d>", MCL_FAIL);
        return MCL_FAIL;
    }
}

E_MCL_ERROR_CODE mcl_list_remove(mcl_list_t *list, mcl_list_node_t *node)
{
	DEBUG_ENTRY("mcl_list_t *list = <%p>, mcl_list_node_t *node = <%p>", list, node)

	E_MCL_ERROR_CODE code;
    ASSERT_NOT_NULL(list);
    ASSERT_NOT_NULL(node);

    code = list_remove(list, node);

    DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

E_MCL_ERROR_CODE mcl_list_remove_with_content(mcl_list_t *list, mcl_list_node_t *node, mcl_list_item_destroy_callback callback)
{
    DEBUG_ENTRY("mcl_list_t *list = <%p>, mcl_list_node_t *node = <%p>, mcl_list_item_destroy_callback callback = <%p>", list, node, &callback)

	E_MCL_ERROR_CODE code;
    ASSERT_NOT_NULL(list);
    ASSERT_NOT_NULL(node);
    ASSERT_NOT_NULL(callback);

    code = list_remove_with_content(list, node, callback);

    DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

E_MCL_ERROR_CODE mcl_list_exist(mcl_list_t *list, const void *item_to_find, mcl_list_compare_callback compare_function, void **item)
{
    DEBUG_ENTRY("mcl_list_t *list = <%p>, const void *item_to_find = <%p>, mcl_list_compare_callback compare_function = <%p>, void **item = <%p>", list, item_to_find, &compare_function, item)

	E_MCL_ERROR_CODE code;
    ASSERT_NOT_NULL(list);
    ASSERT_NOT_NULL(item_to_find);
    ASSERT_NOT_NULL(compare_function);
    ASSERT_NOT_NULL(item);

    code = list_exist(list, item_to_find, compare_function, item);

    DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

void mcl_list_reset(mcl_list_t *list)
{
    DEBUG_ENTRY("mcl_list_t *list = <%p>", list)

    list_reset(list);

    DEBUG_LEAVE("retVal = void");
}

void mcl_list_destroy(mcl_list_t **list)
{
    DEBUG_ENTRY("mcl_list_t **list = <%p>", list)

    list_destroy(list);

    DEBUG_LEAVE("retVal = void");
}

void mcl_list_destroy_with_content(mcl_list_t **list, mcl_list_item_destroy_callback callback)
{
    DEBUG_ENTRY("mcl_list_t **list = <%p>, mcl_list_item_destroy_callback callback = <%p>", list, &callback);

    list_destroy_with_content(list, callback);

    DEBUG_LEAVE("retVal = void");
}

E_MCL_ERROR_CODE list_initialize(list_t **list)
{
    VERBOSE_ENTRY("list_t **list = <%p>", list)

	list_t *list_local;
	
    MCL_NEW(*list);
    ASSERT_CODE_MESSAGE(MCL_NULL != *list, MCL_OUT_OF_MEMORY, "Not enough memory to allocate new list_t!");

    // initialize the list:
	list_local = *list;
    list_local->count = 0;
    list_local->current = MCL_NULL;
    list_local->head = MCL_NULL;
    list_local->last = MCL_NULL;

    VERBOSE_LEAVE("retVal = <%d>", MCL_OK);
    return MCL_OK;
}

E_MCL_ERROR_CODE list_add(list_t *list, void *data)
{
    VERBOSE_ENTRY("list_t *list = <%p>, void *data = <%p>", list, data)

	list_node_t *new_node;
    ASSERT_CODE_MESSAGE(list->count < MCL_SIZE_MAX, MCL_LIMIT_EXCEEDED, "Index of the list is already at the maximum value. Not adding the new data!");

    MCL_NEW(new_node);
    ASSERT_CODE_MESSAGE(MCL_NULL != new_node, MCL_OUT_OF_MEMORY, "Not enough memory to allocate new mcl_node!");

    new_node->data = data;
    new_node->prev = list->last;
    new_node->next = MCL_NULL;
    MCL_VERBOSE("new_node initialized and its data assigned to the received one. Node's data address = <%p>", (void *)new_node->data);

    if (0 == list->count)
    {
        MCL_VERBOSE("This is the first node in the list.");
        list->head = new_node;
        list->last = new_node;
        list->current = new_node;
    }
    else
    {
        list->last->next = new_node;
        list->last = new_node;
    }

    // If we are here, it means adding is successful, we can increase the count:
    ++list->count;
    MCL_VERBOSE("new_node has been added to the list. Current list count = <%d>", list->count);

    VERBOSE_LEAVE("retVal = <%d>", MCL_OK);
    return MCL_OK;
}

E_MCL_ERROR_CODE list_remove_with_content(list_t *list, list_node_t *node, list_item_destroy_callback callback)
{
    DEBUG_ENTRY("list_t *list = <%p>, list_node_t *node = <%p>, list_item_destroy_callback callback = <%p>", list, node, callback)

	E_MCL_ERROR_CODE result;
    void *data = node->data;

    if (MCL_OK == (result = list_remove(list, node)))
    {
        if (MCL_NULL != data)
        {
            callback(&data);
        }
    }

    DEBUG_LEAVE("retVal = <%d>", result);
    return result;
}

E_MCL_ERROR_CODE list_remove(list_t *list, list_node_t *node)
{
    VERBOSE_ENTRY("list_t *list = <%p>, list_node_t *node = <%p>", list, node)

    ASSERT_CODE_MESSAGE(list->count > 0, MCL_ARRAY_IS_EMPTY, "The list count is 0. Not removing any element!");

    // Node connection handling:
    MCL_VERBOSE("node connection handling begins");
    if (MCL_NULL == node->prev)
    {
        // first node.
        MCL_VERBOSE("node->prev is MCL_NULL.");
        MCL_VERBOSE("This is the first node.");

        if (MCL_NULL == node->next)
        {
            // prev and next are MCL_NULL. This is the only node in list.
            // no need to handle any node connection
            MCL_VERBOSE("node->next is NULL.");
            MCL_VERBOSE("This is the only node in list. No need for any connection handling.");
        }
        else
        {
            // prev is MCL_NULL but next is not. This should be the last node.
            // set MCL_NULL to (node->next)->prev
            node->next->prev = MCL_NULL;
            MCL_VERBOSE("node->next is not MCL_NULL.");
            MCL_VERBOSE("There is node's after this node. Next node's prev is set NULL.");
        }
    }
    else
    {
        // not the first node.
        MCL_VERBOSE("node->prev is not NULL.");
        MCL_VERBOSE("This is NOT the first node.");

        if (MCL_NULL == node->next)
        {
            // prev is NOT MCL_NULL but next is MCL_NULL. This is the last node
            // set node->prev->next to MCL_NULL
            node->prev->next = MCL_NULL;

            MCL_VERBOSE("node->next is MCL_NULL.");
            MCL_VERBOSE("This is the last node. Previous node's next is set NULL");
        }
        else
        {
            // prev is NOT MCL_NULL, next is NOT MCL_NULL. This is a middle node
            // set node->prev->next to node->next and the other way :
            node->prev->next = node->next;
            node->next->prev = node->prev;
            MCL_VERBOSE("node->next and node->prev are not MCL_NULL.");
            MCL_VERBOSE("This is a middle node. prev's next is connected to current next. next's prev is connected to current prev.");
        }
    }
    MCL_VERBOSE("node connection handling ends");

    MCL_VERBOSE("list handling begins");

    // List handling :
    if (list->head == node)
    {
        // if this was the head node, take the head to the next one. If the next one is MCL_NULL then list->head will also be MCL_NULL. No need to check for MCL_NULL.
        list->head = node->next;
        MCL_VERBOSE("This is the head node. Head pointer updated to the next node");
    }

    if (list->last == node)
    {
        // if this is the last node take the last node to the previous one. If the prev one is MCL_NULL then list->prev will also be MCL_NULL. No need to check for MCL_NULL.
        list->last = node->prev;
        MCL_VERBOSE("This is the last node. Last pointer updated to the previous node");
    }

    if (list->current == node)
    {
        // if current currently points to this node, we slide it to the next
        list->current = node->next;
        MCL_VERBOSE("This is the list's current node. Current pointer updated to the next node");
    }

    // decrease the list count :
    --list->count;
    MCL_VERBOSE("List counter decreased by one. Current count = <%d>", list->count);

    MCL_VERBOSE("list handling ends");

    // now node connection handling and list handling is completed. we can free the node:
    MCL_FREE(node);
    MCL_VERBOSE("node is freed");

    VERBOSE_LEAVE("retVal = <%d>", MCL_OK);
    return MCL_OK;
}

list_node_t *list_next(list_t *list)
{
    VERBOSE_ENTRY("list_t *list = <%p>", list)

	list_node_t *node = list->current;
    ASSERT_MESSAGE(MCL_NULL != list, "Triggered list handle is NULL! Not performing any operations!");

    if (MCL_NULL == list->current)
    {
        list->current = list->head;
    }
    else
    {
        list->current = list->current->next;
    }

    VERBOSE_LEAVE("retVal = <%p>", node);
    return node;
}

void list_reset(list_t *list)
{
    VERBOSE_ENTRY("list_t *list = <%p>", list)

    if (MCL_NULL != list)
    {
        list->current = list->head;
        MCL_VERBOSE("List is reset.");
    }
    else
    {
        MCL_VERBOSE("List is NULL.");
    }

    VERBOSE_LEAVE("retVal = void");
}

E_MCL_ERROR_CODE list_exist(list_t *list, const void *item_to_find, list_compare_callback compare_function, void **item)
{
    DEBUG_ENTRY("list_t *list = <%p>, const void *item_to_find = <%p>, list_compare_callback compare_function = <%p>, void **item = <%p>", list, item_to_find,
                &compare_function, item)

    // We don't call list_reset() not to change list's configuration of current node, next node etc.
    list_node_t *current_node = list->head;
    while (MCL_NULL != current_node)
    {
        E_MCL_ERROR_CODE item_exists_in_the_list = compare_function(current_node->data, item_to_find);

        if (MCL_OK == item_exists_in_the_list)
        {
            *item = current_node->data;
            DEBUG_LEAVE("retVal = <%d>", MCL_OK);
            return MCL_OK;
        }

        // We don't call list_next() not to change list's configuration of current node, next node etc.
        current_node = current_node->next;
    }

    *item = MCL_NULL;
    DEBUG_LEAVE("retVal = <%d>", MCL_FAIL);
    return MCL_FAIL;
}

void list_destroy_with_content(list_t **list, list_item_destroy_callback callback)
{
    DEBUG_ENTRY("list_t **list = <%p>, list_item_destroy_callback callback = <%p>", list, &callback)

    list_t *local_list = *list;

    if (MCL_NULL != local_list)
    {
        list_node_t *current_node = local_list->head;
        mcl_size_t index = 0;

		MCL_DEBUG("List count = <%d>", local_list->count);

        while (MCL_NULL != current_node)
        {
			list_node_t *node_to_free;

            // we are not freeing the node->data if callback is NULL! It's users responsibility!
            if ((MCL_NULL != callback) && (MCL_NULL != current_node->data))
            {
                callback(&(current_node->data));
            }

            node_to_free = current_node;
            current_node = node_to_free->next;
            MCL_FREE(node_to_free);
            MCL_DEBUG("%d. node has been freed..", index++);
        }

        MCL_FREE(local_list);
        *list = MCL_NULL;
    }

    DEBUG_LEAVE("retVal = void");
}

void list_destroy(list_t **list)
{
    VERBOSE_ENTRY("list_t **list = <%p>", list)

    list_destroy_with_content(list, MCL_NULL);

    VERBOSE_LEAVE("retVal = void");
}
