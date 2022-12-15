#include "Node.h"

//CONSTRUCTORS

// create a new node.
struct Node node_constructor(void *data, unsigned long size)
{
    if (size < 1)
    {
        printf("Incorrect size for node...\n");
        exit(1);
    }
    
    struct Node node;
    node.data = malloc(size);
    memcpy(node.data, data, size);
    node.next = NULL;
    node.previous = NULL;
    return node;
}

// Destroy a node.
void node_destructor(struct Node *node)
{
    free(node->data);
    free(node);
}
