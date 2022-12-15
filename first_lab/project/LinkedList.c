#include "LinkedList.h"

//CONSTRUCTORS

//Initialize a linked list 
struct LinkedList linked_list_init()
{
    struct LinkedList new_list;
    new_list.head = NULL;
    new_list.length = 0;
    
    new_list.insert = insert_ll;
    new_list.remove = remove_node_ll;
    new_list.retrieve = retrieve_ll;
    
    return new_list;
}

//Destroy a LinkedList
void linked_list_destroy(struct LinkedList *linked_list)
{
    for (int i = 0; i < linked_list->length; i++)
    {
        linked_list->remove(linked_list, 0);
    }
}



//PRIVATE METHODS

// The create_node function creates a new node to add to the chain by allocating space on the heap and calling the node constructor.
struct Node * create_node_ll(void *data, unsigned long size)
{
    // Allocate space.
    struct Node *new_node = (struct Node *)malloc(sizeof(struct Node));
    // Call the constructor.
    *new_node = node_constructor(data, size);
    return new_node;
}

// The destroy_node function removes a node by deallocating it's memory address.  This simply renames the node destructor function.
void destroy_node_ll(struct Node *node_to_destroy)
{
    node_destructor(node_to_destroy);
}

// The iterate function traverses the list from beginning to end.
struct Node * iterate_ll(struct LinkedList *linked_list, int index)
{
    // Confirm the user has specified a valid index.
    if (index < 0 || index >= linked_list->length)
    {
        return NULL;
    }
    // Create a tmp node for iteration.
    struct Node *tmp = linked_list->head;
    // Step through the list until the desired index is reached.
    for (int i = 0; i < index; i++)
    {
        tmp = tmp->next;
    }
    return tmp;
}



//PUBLIC METHODS

// Insert a new node in the list.
void insert_ll(struct LinkedList *linked_list, int index, void *data, unsigned long size)
{
    struct Node *node_to_insert = create_node_ll(data, size);
    if (index == 0)
    {
        node_to_insert->next = linked_list->head;
        linked_list->head = node_to_insert;
    }
    else
    {
        struct Node *tmp = iterate_ll(linked_list, index - 1);
        node_to_insert->next = tmp->next;
        tmp->next = node_to_insert;
        
    }
    // Increment the list length.
    linked_list->length += 1;
}

// The remove function removes a node from the linked list.
void remove_node_ll(struct LinkedList *linked_list, int index)
{
    if (index == 0)
    {
        struct Node *node_to_remove = linked_list->head;
        if (node_to_remove)
        {
            linked_list->head = node_to_remove->next;
            destroy_node_ll(node_to_remove);
        }
    }
    else
    {
        // Find the item in the list before the one that is going to be removed.
        struct Node *tmp = iterate_ll(linked_list, index - 1);
        // Use the tmp to define the node to be removed.
        struct Node *node_to_remove = tmp->next;
        // Update the tmp's next to skip the node to be removed.
        tmp->next = node_to_remove->next;
        // Remove the node.
        destroy_node_ll(node_to_remove);
    }
    // Decrement the list length.
    linked_list->length -= 1;
}

// The retrieve function is used to access data in the list.
void * retrieve_ll(struct LinkedList *linked_list, int index)
{
    struct Node *tmp = iterate_ll(linked_list, index);
    if (tmp)
    {
        return tmp->data;
    }
    else
    {
        return NULL;
    }
}
