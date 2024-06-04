#ifndef __GFC_LIST_H__
#define __GFC_LIST_H__

#include <SDL.h>

/**
 * @brief to be used in gfc_list_foreach
 */
typedef void gfc_work_func(void*);/**<prototype for a work function*/

/**
 * @brief to be used in gfc_list_foreach_context
 */
typedef void gfc_work_func_context(void*,void*);/**<prototype for a work function*/

/**
 * @brief to be used in gfc_list_sort
 * @note It must return < 0 if the left should be before the right, or > 0 if right should be before the left, 0 if they are equal priority (like strcmp)
 */
typedef int  gfc_compare_func(void*,void*);/**<prototype for a compare function for sorting*/

typedef struct
{
    void *data;
}GFC_ListElementData;

/**
 * @brief the GFC GFC_List is an automatically expanding general purpose list
 * the list stores data in contiguous memory
 */
typedef struct
{
    GFC_ListElementData *elements;
    Uint32 size;
    Uint32 count;
}GFC_List;

/**
 * @brief allocated a new empty list
 * @return NULL on memory error or a new empty list
 */
GFC_List *gfc_list_new();

/**
 * @brief allocate a new empty list of size 'count'
 * @param count how many elements you wish to support in this list.
 * @return NULL on memory error or a new empty list
 */
GFC_List *gfc_list_new_size(Uint32 count);

/**
 * @brief make a copy of a list.  
 * @note: THIS DOES NOT COPY ANY DATA POINTED TO BY THE OLD LIST
 * @param old the list to copy
 * @return NULL on error or no old data to copy,  A new list that contains the same 
 * information as the old list.  (data pointers will be copied)
 */
GFC_List *gfc_list_copy(GFC_List *old);

/**
 * @brief deletes a list that has been previously alloc
ated
 * @param list the list to delete;
 */
void gfc_list_delete(GFC_List *list);

/**
 * @brief clears out the list, but does NOT free any data that the list may have been pointing to
 * @param list the list to clear
 * @note effectively zeroes out the list without freeing any data
 */
void gfc_list_clear(GFC_List *list);

/**
 * @brief get the data stored at the nth element
 * @param list the list to pull data from
 * @param n which element to look out
 * @return NULL on error (such as if n > the element count) or the address of the data otherwise
 */
void *gfc_list_get_nth(GFC_List *list,Uint32 n);

//shorthand
#define gfc_list_nth(list,n) gfc_list_get_nth(list,n) 

/**
 * @brief set the data stored at the nth element
 * @param list the list to change
 * @param n which element to change
 * @param data the new data to set it to
 * @note the old data WILL NOT be cleaned up
 */
void gfc_list_set_nth(GFC_List *list,Uint32 n,void *data);

/**
 * @brief add an element to the end of the list
 * @note as of 2023, the original list is what is returned on success and you do not HAVE to assign it back to this function
 * @param list the list to add to
 * @param data the data to assign to the new element
 */
void gfc_list_append(GFC_List *list,void *data);

/**
 * @brief add an element to the beginning of the list
 * @note must catch the returned list to replace the list you had
 * @param list the list to add to
 * @param data the data to assign to the new element
 */
void gfc_list_prepend(GFC_List *list,void *data);

/**
 * @brief instert a new element at the position provided
 * @param list the list to insert into
 * @param data the data to assin to the new element
 * @param n the position to insert at
 */
void gfc_list_insert(GFC_List *list,void *data,Uint32 n);

/**
 * @brief delete the element at the nth position in the array
 * @note this does not clean up the information that the list is referring to
 * @param list the list to delete out of
 * @param n the element to delete.  This is no-op if the nth element is beyond the scope of the list (event is logged)
 * @return -1 on error, 0 otherwise
 */
int gfc_list_delete_nth(GFC_List *list,Uint32 n);

/**
 * @brief delete the item at the end of the list
 * @note this does not clean up the information that the list is referring to
 * @param list the list to delete from;
 * @return 0 if all is well, -1 on error
 */
int gfc_list_delete_last(GFC_List *list);

/**
 * @brief delete the first element in the list pointing to the address of data
 * @note does not delete the data itself
 * @param list the list to delete the element from
 * @param data used to match against which element to delete
 * @return -1 on error, 0 otherwise
 */
int gfc_list_delete_data(GFC_List *list,void *data);

/**
 * @brief search the list for the given item
 * @param list the list to search
 * @param data the data to search for
 * @return -1 on not found or error, the index otherwise
 */
int gfc_list_get_item_index(GFC_List *list,void *data);

/**
 * @brief get the number of tracked elements in the list
 * @param list the list the check
 * @return the count in the list.  Will be zero if list was NULL
 */
Uint32 gfc_list_get_count(GFC_List *list);

//shorthand
#define gfc_list_count(list) gfc_list_get_count(list)

/**
 * @brief iterate over each element in the array and call the function provided
 * @param list the list to iterate over
 * @param function a pointer to a function that will be called.  Data will be set to the list data element
 */
void gfc_list_foreach(GFC_List *list,void (*function)(void *data));

/**
 * @brief iterate over each element in the array and call the function provided
 * @param list the list to iterate over
 * @param function a pointer to a function that will be called.  Data will be set to the list data element, context will be the contextData provided
 * @param contextData the data that will also be provided to the function pointer for each element
 */
void gfc_list_foreach_context(GFC_List *list,void (*function)(void *data,void *context),void *contextData);

/**
 * @brief swap the locations of two items in the list.
 * @param list the list to alter
 * @param a the first item
 * @param b the second item
 */
void gfc_list_swap_indices(GFC_List *list,Uint32 a, Uint32 b);

/**
 * @brief add the elements from b into a
 * @note the data is not duplicated and b will still point to the same data that is now also referenced in a
 * @note the new address of a is returned
 * @param a the list that will get new items added to it
 * @param b the list that will provide new items for a
 * @return NULL on failure, a pointer to the new list otherwise
 */
GFC_List *gfc_list_concat(GFC_List *a,GFC_List *b);

/**
 * @brief same as gfc_list_concat but b is freed when complete
 * @note the new address of a is returned
 * @param a the list to add items to
 * @param b the list to provide the items.  This list is freed, but the data referenced is now referenced by a
 * @return NULL on failure, a pointer to the new list otherwise
 */
GFC_List *gfc_list_concat_free(GFC_List *a,GFC_List *b);

/**
 * @brief perform a sort on the provided list.
 * @note This uses the merge sort algorithm.  It will end up created log(n) more lists in memory but not duplicate the data.
 * @param list the list to sort. This list will be sorted if this is successful
 * @param compar the comparison function,gfc_compare_func can be used as a prototype.  It must return < 0 if the left should be before the right, or > 0 if right should be before the left, 0 if they are equal priority (like strcmp)
 */
void gfc_list_sort(GFC_List *list,int (*compare)(void *a,void *b));


#endif
