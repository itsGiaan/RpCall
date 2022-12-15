#ifndef Node_h
#define Node_h

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

//DATA TYPES

struct Node
{
    void *data;
    
    struct Node *next;
    struct Node *previous;
};


//CONSTRUCTORS

// Create a new node.
struct Node node_constructor(void *data, unsigned long size);
// Destroy a node.
void node_destructor(struct Node *node);

#endif 
