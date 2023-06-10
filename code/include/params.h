#ifndef PARAM_H
#define PARAM_H 

typedef struct {
    /* Defining node type: 0 for TNN, 1 for CHIEF, 2 for FORK, 3 for BRANCH */
    int node_type;

    /* Node father and children */
    char* node_source_p2p;
    char* node_self;
    char* node_alt_self;
    char* node_father;
    char** node_children;

    /* Number of children*/
    int children_count;
    /* self children position */
    int self_children_position;
} node_t;

typedef struct {
    double* water_flow;        /* Array that contains all the sensor sampling value */
    double water_flow_sum;     /* The sum of all the sensor sampling */
} sample_t;

#endif
