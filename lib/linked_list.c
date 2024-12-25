/***
 * AUTHOR: ALEX PETTIPAS
 * ID: B00813771
 * Re-using my solution for a linked list to a CSCI2122 lab
 */

#include "linked_list.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdbool.h>


LinkedList * llist_initialize(int dataSize, char* type)
{
    LinkedList* list = malloc(sizeof(LinkedList));
    list->itemSize = dataSize;
    list->type = type;
    list->first = malloc(sizeof(Node));
    list->last = malloc(sizeof(Node));
    list->size = 0;
    return list;
}

bool llist_add_at(LinkedList* lst, int index, void* v)
{
    if (lst == NULL || !v)
    {
        return false;
    }

    Node* n = malloc(sizeof(Node));
    n->data = malloc(lst->itemSize);
    n->data = memcpy(n->data, v, lst->itemSize);
    n->prev = malloc(sizeof(Node));
    n->next = malloc(sizeof(Node));
    n->prev = NULL;
    n->next = NULL;


    if (index > lst->size)
    {
        index = lst->size;
    }
    if (lst->size == 0)
    {
        lst->first = n;
        lst->last = n;
    }
    else if (lst->size > 0 && index == 0)
    {
        lst->first->prev = n;
        n->next = lst->first;
        lst->first = n;
    }
    else if (lst->size > 0 && lst->size == index)
    {
        lst->last->next = n;
        n->prev = lst->last;
        lst->last = n;
    }
    else if (lst->size > 0 && index < lst->size && index > 0)
    {
        Node* tmp = malloc(sizeof(Node));
        tmp = lst->first;
        for (int i = 0; i < index; i++)
        {
            tmp = tmp->next;
        }
        tmp->next->prev = n;
        tmp->next = n;

        n->next = tmp->next->prev;
        n->prev = tmp;
    }
    lst->size++;

    return true;
}

bool llist_add_first(LinkedList* lst, void* v)
{

    return llist_add_at(lst, 0, v);
}

bool llist_add_last(LinkedList* lst, void* v)
{
    return llist_add_at(lst, lst->size, v);
}

void* llist_get(LinkedList* lst, int index)
{
    if (index >= 0 && index < lst->size)
    {
        Node* tmp = malloc(sizeof(Node));
        tmp = lst->first;
        for (int i = 0; i < index; i++)
        {
            tmp = tmp->next;
        }
        return tmp->data;
    }
    else
    {
        return NULL;
    }
}

int llist_index_of(LinkedList* lst, void* v)
{
    if (lst == NULL || !v)
    {
        return -1;
    }
    int location = 0;
    Node* tmp = malloc(sizeof(Node));
    tmp = lst->first;
    while (location < lst->size)
    {
        if (v == tmp->data)
        {
            return location;
        }
        location++;
        tmp = tmp->next;
    }
    return -1;
}

void* llist_remove(LinkedList* lst, int index)
{
    Node* tmp = malloc(sizeof(Node));
    if (index == 0)
    {
        tmp = lst->first;
        lst->first->next->prev = NULL;
        lst->first = lst->first->next;

        lst->size--;
        return tmp->data;
    }
    else if (index < lst->size && index > 0)
    {
        tmp = lst->first;
        for (int i = 0; i < index; i++)
        {
            tmp = tmp->next;
        }
        tmp->next->prev = tmp->prev;
        tmp->prev->next = tmp->next;
        lst->size--;

        return tmp->data;
    }
    else if (index == lst->size)
    {
        tmp = lst->last;
        lst->last->prev->next = NULL;
        lst->last = lst->last->prev;

        lst->size--;
        return tmp->data;
    }
    else
    {
        return NULL;
    }

}

void* llist_remove_first(LinkedList* lst)
{
    if (lst == NULL)
    {
        return NULL;
    }
    return llist_remove(lst, 0);
}

void* llist_remove_last(LinkedList* lst)
{
    if (lst == NULL)
    {
        return NULL;
    }
    return llist_remove(lst, lst->size);
}

bool llist_destroy(LinkedList* lst)
{
    Node* tmp = malloc(sizeof(Node));
    tmp = lst->first;
    for (int i = 0; i < lst->size-1; i++)
    {
        tmp = tmp->next;
        free(tmp->prev);
    }
    free(lst);
    return true;
}

/***
 * llist_to_array
 * added this function to assist in CSCI3161 course project.
 * @param lst
 * @return void pointer representing an array
 */
void* llist_to_array(LinkedList* lst) {
    void* arr = malloc(lst->size * lst->itemSize);
    Node* tmp = malloc(sizeof(Node));
    tmp = lst->first;
    for (int i = 0; i < lst->size; i++) {
        memcpy(arr + (i * lst->itemSize), tmp->data, lst->itemSize);
        tmp = tmp->next;
    }
    return arr;
}

