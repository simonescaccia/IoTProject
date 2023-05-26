#ifndef PARAM_H
#define PARAM_H 

typedef struct {
    /* Defining node type: 0 for TNN, 1 for CHIEF, 2 for FORK, 3 for BRANCH */
    int node_type;

    /* Node father and children */
    char* node_father;
    char* node_self;
    char* node_alt_self;
    char** node_children;

    /* Number of children*/
    int children_count;
} node_t;

#endif
