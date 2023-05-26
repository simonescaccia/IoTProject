#include "sample_generator.h"
#include "string.h"

/* Replace this simulation with data sampled with the prototype */

/**
 * TTN self_node_type == 0, CHIEF self_node_type: 1
 * Water flow function simulation: x: logic_time, y: L/min
 * y = 30 L/min for x in [0, 6]
 * y = 0 L/min for x in [7, 10]
 * 
 * BRANCH self_node_type: 3
 * y = 30 L/min for x in [0, 2]
 * y = 20 L/min for x in [4, 6] LEAKAGE
 * y = 0 L/min for x [7, 10]
*/

int get_water_flow(int self_node_type, char *self_node, int logic_time) {
    /* logic_time should be in the range [0,10] */
    (void)self_node;

    if (self_node_type == 0 || self_node_type == 1) {
        /* CHIEF data */
        if (logic_time >= 0 && logic_time <= 6)
            return 30;
        if (logic_time >= 7 && logic_time <= 10)
            return 0;
    }
    if (self_node_type == 2) {
        /* BRANCH data */
        if (logic_time >= 0 && logic_time <= 2)
            return 30;
        if (logic_time >= 4 && logic_time <= 6)
            return 20;
        if (logic_time >= 7 && logic_time <= 10)
            return 0;
    }

    return 0;
}