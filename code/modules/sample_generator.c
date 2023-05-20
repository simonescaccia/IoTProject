#include "sample_genator.h"

/* Replace this simulation with data sampled with the prototype */

/**
 * link CHIEF-BRANCH; type: 0
 * 
 * CHIEF; self_node_type: 0
 * Water flow function simulation: x: time, y: L/min
 * y = 30 L/min for x in [0, 40]
 * y = 0 L/min for x in [41, 60]
 * BRANCH; self_node_type: 1
 * y = 30 L/min for x in [0, 20]
 * y = 20 L/min for x in [21, 40] LEAKAGE
 * y = 0 L/min for x [41, 60]
*/

int get_water_flow(int self_node_type, int link_type, int time) {
    /* time should be in the range [0,60] */

    if (link_type == 0 ) {
        /* CHIEF-BRANCH link */
        if (self_node_type == 0) {
            /* CHIEF data */
            if (time >= 0 && time <= 40)
                return 30;
            if (time >= 41 && time <= 60)
                return 0;
        }
        if (self_node_type == 1) {
            /* BRANCH data */
            if (time >= 0 && time <= 20)
                return 30;
            if (time >= 21 && time <= 40)
                return 20;
            if (time >= 41 && time <= 60)
                return 0;
        }
    }

    return 0;
}