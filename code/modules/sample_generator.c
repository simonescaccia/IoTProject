#include "sample_genator.h"
#include "string.h"

/* Replace this simulation with data sampled with the prototype */

/**
 * CHIEF; self_node_type: 0, self_node: "10"
 * Water flow function simulation: x: time, y: L/min
 * y = 30 L/min for x in [0, 6]
 * y = 0 L/min for x in [7, 10]
 * 
 * BRANCH; self_node_type: 2, self_node: 11
 * y = 30 L/min for x in [0, 2]
 * y = 20 L/min for x in [4, 6] LEAKAGE
 * y = 0 L/min for x [7, 10]
*/

int get_water_flow(int self_node_type, char *self_node, int time) {
    /* time should be in the range [0,10] */

    if (self_node_type == 0 && strcmp("10", self_node) == 0) {
        /* CHIEF data */
        if (time >= 0 && time <= 6)
            return 30;
        if (time >= 7 && time <= 10)
            return 0;
    }
    if (self_node_type == 2 && strcmp("11", self_node) == 0) {
        /* BRANCH data */
        if (time >= 0 && time <= 2)
            return 30;
        if (time >= 4 && time <= 6)
            return 20;
        if (time >= 7 && time <= 10)
            return 0;
    }

    return 0;
}