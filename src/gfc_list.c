#include "simple_logger.h"

#include "gfc_types.h"
#include "gfc_list.h"

void gfc_list_delete(GFC_List *list)
{
    if (!list)return;
    if (list->elements)
    {
        free(list->elements);
    }
    free(list);
}

GFC_List *gfc_list_new()
{
    return gfc_list_new_size(16);
}

GFC_List *gfc_list_copy(GFC_List *old)
{
    GFC_List *new;
    if (!old)return 0;
    if (old->size <= 0)return NULL;
    new = gfc_list_new_size(old->size);
    if (!new)return NULL;
    if (old->count <= 0)return new;
    memcpy(new->elements,old->elements,sizeof(GFC_ListElementData)*old->count);
    new->count = old->count;
    return new;
}

GFC_List *gfc_list_new_size(Uint32 count)
{
    GFC_List *l;
    if (!count)
    {
        slog("cannot make a list of size zero");
        return NULL;
    }
    if (count < 8)count = 8;
    l = (GFC_List *)malloc(sizeof(GFC_List));
    if (!l)
    {
        slog("failed to allocate space for the list");
        return NULL;
    }
    memset(l,0,sizeof(GFC_List));
    l->size = count;
    l->elements = gfc_allocate_array(sizeof(GFC_ListElementData),count);
    if (!l->elements)
    {
        slog("failed to allocate space for list elements");
        free(l);
        return NULL;
    }
    return l;
}

void gfc_list_set_nth(GFC_List *list,Uint32 n,void *data)
{
    if (!list)return;
    if ((n >= list->count)||(n >= list->size))return;
    list->elements[n].data = data;
}

void gfc_list_swap_indices(GFC_List *list,Uint32 a, Uint32 b)
{
    void *temp = NULL;
    if (!list)return;
    if (a == b)return;
    if ((a >= list->count)||(b >= list->count))return;
    if ((a >= list->size)||(b >= list->size))return;
    temp = list->elements[a].data;
    list->elements[a].data = list->elements[b].data;
    list->elements[b].data = temp;
}

void *gfc_list_get_nth(GFC_List *list,Uint32 n)
{
    if (!list)
    {
        return NULL;
    }
    if ((n >= list->count)||(n >= list->size))return NULL;
    return list->elements[n].data;
}

int gfc_list_expand(GFC_List *list)
{
    GFC_List *l;
    if (!list)
    {
        slog("no list provided");
        return 0;
    }
    if (!list->size)list->size = 8;
    l = gfc_list_new_size(list->size * 2);
    if (!l)
    {
        return 0;
    }
    if (list->count > 0)
    {
        memcpy(l->elements,list->elements,sizeof(GFC_ListElementData)*list->count);
    }
    list->size = l->size;// update the new size
    free(list->elements);//free the old data
    list->elements = l->elements;//point to new memory address
    free(l);//free the temp list
    return 1;
}

void gfc_list_append(GFC_List *list,void *data)
{
    if (list->count >= list->size)
    {
        if (!gfc_list_expand(list))
        {
            slog("append failed due to lack of memory");
            return;
        }
    }
    list->elements[list->count].data = data;
    list->count++;
}

GFC_List *gfc_list_concat(GFC_List *a,GFC_List *b)
{
    int i,count;
    void *data;
    if ((!a) || (!b))
    {
        slog("missing list data");
        return NULL;
    }
    count = gfc_list_get_count(b);
    for (i = 0; i < count;i++)
    {
        data = gfc_list_get_nth(b,i);
        gfc_list_append(a,data);
        if (a == NULL)return NULL;
    }
    return a;
}

GFC_List *gfc_list_concat_free(GFC_List *a,GFC_List *b)
{
    a = gfc_list_concat(a,b);
    if (a == NULL)return NULL;
    gfc_list_delete(b);
    return a;
}

void gfc_list_prepend(GFC_List *list,void *data)
{
    gfc_list_insert(list,data,0);
}

void gfc_list_insert(GFC_List *list,void *data,Uint32 n)
{
    if (!list)
    {
        slog("no list provided");
        return;
    }
    if (n > list->size + 1)
    {
        slog("attempting to insert element beyond length of list");
        return;
    }
    if (list->count >= list->size)
    {
        gfc_list_expand(list);
        if (!list)return;
    }
    memmove(&list->elements[n+1],&list->elements[n],sizeof(GFC_ListElementData)*(list->count - n));//copy all elements after n
    list->elements[n].data = data;
    list->count++;
    return;
}


int gfc_list_delete_first(GFC_List *list)
{
    return gfc_list_delete_nth(list,0);
}

int gfc_list_delete_last(GFC_List *list)
{
    if (!list)
    {
        slog("no list provided");
        return -1;
    }
    return gfc_list_delete_nth(list,list->count-1);
}

int gfc_list_get_item_index(GFC_List *list,void *data)
{
    int i;
    if (!list)
    {
        slog("no list provided");
        return -1;
    }
    if (!data)return 0;
    for (i = 0; i < list->count;i++)
    {
        if (list->elements[i].data == data)
        {
            // found it
            return i;
        }
    }
    return -1;    
}

int gfc_list_delete_data(GFC_List *list,void *data)
{
    int i;
    if (!list)
    {
        slog("no list provided");
        return -1;
    }
    if (!data)return 0;
    for (i = 0; i < list->count;i++)
    {
        if (list->elements[i].data == data)
        {
            // found it, now delete it
            gfc_list_delete_nth(list,i);
            return 0;
        }
    }
    return -1;
}

void gfc_list_clear(GFC_List *list)
{
    if (!list)return;
    memset(list->elements,0,list->size);//zero out all the data;
    list->count = 0;
}

int gfc_list_delete_nth(GFC_List *list,Uint32 n)
{
    int i;
    if (!list)
    {
        slog("no list provided");
        return -1;
    }
    if (n >= list->count)
    {
        slog("attempting to delete beyond the length of the list");
        return -1;
    }
    if (n == (list->count - 1))
    {
        list->count--;// last element in the array, this is easy
        list->elements[n].data = NULL;
        return 0;
    }
    for (i = n; i < (list->count - 1);i++)
    {
        list->elements[i].data = list->elements[i+1].data;
    }
    if (i+1 < list->count)list->elements[i+1].data = NULL;
    list->count--;
    return 0;
}

Uint32 gfc_list_get_count(GFC_List *list)
{
    if (!list)return 0;
    return list->count;
}

void gfc_list_split(GFC_List *base,GFC_List **leftOut,GFC_List **rightOut)
{
    Uint32 halfway;
    Uint32 i;
    GFC_List *left,*right;
    if ((!base)||(!leftOut)||(!rightOut))return;
    if (base->count < 2)return;//nothing to do
    halfway = base->count / 2;
    left = gfc_list_new_size(halfway + 1);//pad for an extra
    right = gfc_list_new_size(halfway + 1);//pad for an extra
    for (i =0; i < halfway;i++)
    {
        gfc_list_append(left,gfc_list_get_nth(base,i));
    }
    for (;i < base->count;i++)
    {
        gfc_list_append(right,gfc_list_get_nth(base,i));
    }
    *leftOut = left;
    *rightOut = right;
}

//plan: merge sort.  First split the list into two smaller lists, and then recursively have each one sort
void gfc_list_sort(GFC_List *list,int (*compare)(void *a,void *b))
{
    int l,r;
    void *leftItem,*rightItem;
    GFC_List *left,*right;
    if ((!list)||(!compare))return;//no list or no compare function, so stop
    if (list->count <= 1)return;//not enough to sort
    //now split it
    gfc_list_split(list,&left,&right);
    if ((left == NULL)||(right == NULL))
    {
        return;//nothing left to do
    }
    //sort each half
    gfc_list_sort(left,compare);
    gfc_list_sort(right,compare);
    //now merge the halves
    //for each element in each list (until one of them 
    l = 0;
    r = 0;
    //clear the original list
    gfc_list_clear(list);
    //using the count getter in case either list is NULL
    while((l < gfc_list_get_count(left))&&(r < gfc_list_get_count(right)))
    {
        leftItem = gfc_list_get_nth(left,l);
        rightItem = gfc_list_get_nth(right,r);
        if ((!leftItem)||(!rightItem))break;//nothing left to compare;
        if (compare(leftItem,rightItem) <= 0)
        {
            gfc_list_append(list,leftItem);
            l++;//advance our trek through the left list
        }
        else
        {
            gfc_list_append(list,rightItem);
            r++;//advance our trek through the right list
        }
    }
    if (l < gfc_list_get_count(left))
    {
        //finish with the remaining left
        for (;l < gfc_list_get_count(left);l++)
        {
            gfc_list_append(list,gfc_list_get_nth(left,l));
        }
    }
    else if (r < gfc_list_get_count(right))//these should be mutually exclusive and no more than 1 item
    {
        //finish with the remaining right
        for (;r < gfc_list_get_count(right);r++)
        {
            gfc_list_append(list,gfc_list_get_nth(right,r));
        }
    }
    //at this point the main list should be sorted in place
    //cleanup
    gfc_list_delete(left);
    gfc_list_delete(right);
}

void gfc_list_foreach(GFC_List *list,void (*function)(void *data))
{
    int i;
    if (!list)
    {
        return;
    }
    if (!function)
    {
        slog("no function provided");
    }
    for (i = 0;i < list->count;i++)
    {
        function(list->elements[i].data);
    }
}

void gfc_list_foreach_context(GFC_List *list,void (*function)(void *data,void *context),void *contextData)
{
    int i;
    if (!list)
    {
        slog("no list provided");
        return;
    }
    if (!function)
    {
        slog("no function provided");
    }
    for (i = 0;i < list->count;i++)
    {
        function(list->elements[i].data,contextData);
    }
}

/*eol@eof*/
