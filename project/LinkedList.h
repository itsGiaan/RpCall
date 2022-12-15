
#ifndef LinkedList_h
#define LinkedList_h

#include <stdlib.h>
#include <stdio.h>
#include "Node.h"

//DATA TYPES

struct LinkedList
{
    struct Node *head;
    // Index updated at runtime for the size of the list.
    int length;
    
    //MEMBER METHODS
    // Add an item at a specified index.
    void (*insert)(struct LinkedList *linked_list, int index, void *data, unsigned long size);
    // Delete an item at specified index.
    void (*remove)(struct LinkedList *linked_list, int index);
    // Access the data in the list at a specified index.
    void * (*retrieve)(struct LinkedList *linked_list, int index);
};

//FUNCTION PROTOTYPES

// CONSTRUCTORS 
struct LinkedList linked_list_init(void);
void linked_list_destroy(struct LinkedList *linked_list);


//Private Methods
//This methods should never be accessed outside the LinkedList struct.
struct Node * create_node_ll(void *data, unsigned long size);
void destroy_node_ll(struct Node *node_to_destroy);


//Public Member Methods

struct Node * iterate_ll(struct LinkedList *linked_list, int index);
void insert_ll(struct LinkedList *linked_list, int index, void *data, unsigned long size);
void remove_node_ll(struct LinkedList *linked_list, int index);
void * retrieve_ll(struct LinkedList *linked_list, int index);


#endif 
