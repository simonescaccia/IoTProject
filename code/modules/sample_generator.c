#include "sample_generator.h"
#include "string.h"

/* Replace this simulation with data sampled with the prototype */

/**
 * TTN self_node_type == 0, CHIEF self_node_type: 1
 * Water flow function simulation: x: logic_time, y: L/min
 * y = 30 L/min for x in [0, 7]
 * y = 0 L/min for x in [8, 10]
 * 
*/

int get_water_flow(int self_node_type, char *self_node, int logic_time) {
    /* logic_time should be in the range [0,10] */
    (void)self_node;

    if (self_node_type == 0 || self_node_type == 1) {
        /* SOURCE data */
        if (logic_time >= 0 && logic_time <= 7)
            return 30;
        if (logic_time >= 8 && logic_time <= 10)
            return 0;
    }
    if (self_node_type == 2) {
        /* FORK data */
        if (logic_time >= 0 && logic_time <= 3)
            return 30;
        if (logic_time >= 4 && logic_time <= 7)
            return 20;
        if (logic_time >= 8 && logic_time <= 10)
            return 0;
    }
    if (self_node_type == 3) {
        /* BRANCH data */
        if (logic_time >= 0 && logic_time <= 3)
            return 30;
        if (logic_time >= 4 && logic_time <= 5)
            return 20;
        if (logic_time >= 6 && logic_time <= 7)
            return 10;
        if (logic_time >= 8 && logic_time <= 10)
            return 0;
    }

    return 0;
}