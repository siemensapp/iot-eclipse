/*!**********************************************************************
 *
 * @copyright Copyright (C) 2016 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 *
 *************************************************************************
 *
 * @file     list.h
 * @date     Jul 20, 2016
 * @brief    List module header file.
 *
 * This module is used for double linked list handling.
 *
 ************************************************************************/

#ifndef LIST_H_
#define LIST_H_

#include "mcl/mcl_list.h"

typedef mcl_list_node_t list_node_t;
typedef mcl_list_t list_t;
typedef mcl_list_compare_callback list_compare_callback;
typedef mcl_list_item_destroy_callback list_item_destroy_callback;

/**
 * @brief Initializes the list.
 *
 * Head, Last and Current pointers will be set to NULL. Count will be 0.
 *
 * @param [out] list Will point to the initialized #list_t object.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_OUT_OF_MEMORY in case there is not enough memory in the system to proceed.</li>
 * </ul>
 */
E_MCL_ERROR_CODE list_initialize(list_t **list);

/**
 * @brief Adds a new list item.
 *
 * The received data pointer is added to the list. No new memory is allocated and no memory copy operations done.
 * The lifetime of the @p data should be handled by the user.
 *
 * @param [in] list The list to which the data is added.
 * @param [in] data The data to be added. Only data pointer will be kept in the list. No additional memory operation will perform.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_OUT_OF_MEMORY in case there is not enough memory in the system to proceed.</li>
 * <li>#MCL_LIMIT_EXCEEDED in case @p list is full and @p data can not be added to @p list.</li>
 * </ul>
 */
E_MCL_ERROR_CODE list_add(list_t *list, void *data);

/**
 * @brief Removes a node from the list.
 *
 * The node (#list_node_t) will be freed but the data that it holds is not going to be freed.
 * <b>User needs to free the resource before it calls remove!</b>
 *
 * @param [in] list The list from which the node is removed.
 * @param [in] node The node to be removed.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_ARRAY_IS_EMPTY in case @p list is empty.</li>
 * </ul>
 */
E_MCL_ERROR_CODE list_remove(list_t *list, list_node_t *node);

/**
 * @brief Removes a node from the list and destroys the removed item with the provided callback function.
 *
 * The node (#list_node_t) will be freed and the data that it holds is going to be destroyed by passing to the provided callback function.
 * <b>User needs to free the resource before it calls remove!</b>
 *
 * @param [in] list The list from which the node is removed.
 * @param [in] node The node to be removed.
 * @param [in] callback The callback function to destroy the list item.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_ARRAY_IS_EMPTY in case @p list is empty.</li>
 * </ul>
 */
E_MCL_ERROR_CODE list_remove_with_content(list_t *list, list_node_t *node, list_item_destroy_callback callback);

/**
 * @brief Get the next node from the list.
 *
 * Last returned node is kept in list. This function can be called consequently to loop over the list.
 * If there is no node left to return or the list is empty, this function returns NULL.
 *
 * @param [in] list The list.
 * @return The next node in the list.
 */
list_node_t *list_next(list_t *list);

/**
 * @brief Searches @p item_to_find in the @p list.
 *
 * @param [in] list To search in.
 * @param [in] item_to_find To search for.
 * @param [in] compare_function Callback function to compare.
 * @param [out] item If item_to_find is found, then the node data where item_to_find exists returned with this parameter.
 * @return
 * <ul>
 * <li>#MCL_OK if @p item_to_find exists in the @p list.</li>
 * <li>#MCL_FAIL if @p item_to_find doesn't exist in the @p list.</li>
 * </ul>
 */
E_MCL_ERROR_CODE list_exist(list_t *list, const void *item_to_find, list_compare_callback compare_function, void **item);

/**
 * @brief Reset the current node to head node.
 *
 * The goal here is to be able to loop over the list from it's beginning.
 *
 * @param [in] list The list which is used to reset it's current node to it's head node.
 */
void list_reset(list_t *list);

/**
 * @brief To destroy the list.
 *
 * For every node in the list, it frees the node but not the data it holds. After all nodes freed,
 * it frees the list itself and sets it's value as NULL.
 *
 * <b>User needs to free all the data that the list is holding before destroying it! </b>
 *
 * @param [in] list The address of the pointer of the list to be destroyed.
 */
void list_destroy(list_t **list);

/**
 * @brief To destroy the list and its items with a given callback function.
 *
 * For every node in the list, it frees the node. The data of the node can also freed by given callback function. After all nodes freed,
 * it frees the list itself and sets it's value as NULL.
 *
 * @param [in] list The address of the pointer of the list to be destroyed.
 * @param [in] callback The callback function to destroy each node data within the list. If NULL no action will be performed.
 */
void list_destroy_with_content(list_t **list, list_item_destroy_callback callback);

#endif //LIST_H_
