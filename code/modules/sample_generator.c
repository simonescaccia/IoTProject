#include <stdio.h>

#include "sample_generator.h"
#include "string.h"

/**
 * logic_time in [0, 2]: Simulate no LEAKAGE with water usage
 * logic_time in [3, 5]: Simulate no LEAKAGE without water usage
 * logic_time in [6, 8]: Simulate a LEAKAGE between the CHIEF and the FORK with water usage
 * logic_time in [9, 11]: Simulate a LEAKAGE between the FORK and a BRANCH with water usage
 * logic_time in [12, 14]: Simulate a LEAKAGE between the CHIEF and the FORK without water usage
 * logic_time in [15, 17]: Simulate a LEAKAGE between the FORK and a BRANCH without water usage
*/

/**
 * @brief position cares about the pair sensor-child: set 0 or 1, and used only for FORKs and BRANCHs
*/
double get_water_flow(int self_node_type, int position, int time) {
    int logic_time = time % 18;
    /* logic_time should be in the range [0,17] */

    if (self_node_type != 2 || (self_node_type == 2 && position == 0)) {
        /* FORK print only one time */
        if (logic_time == 0)    puts("Simulate no LEAKAGE with water usage\n");
        if (logic_time == 3)    puts("Simulate no LEAKAGE without water usage\n");
        if (logic_time == 6)    puts("Simulate a LEAKAGE between the CHIEF and the FORK with water usage\n");
        if (logic_time == 9)    puts("Simulate a LEAKAGE between the FORK and a BRANCH with water usage\n");
        if (logic_time == 12)   puts("Simulate a LEAKAGE between the CHIEF and the FORK without water usage\n");
        if (logic_time == 15)   puts("Simulate a LEAKAGE between the FORK and a BRANCH without water usage\n");
    }

    if (self_node_type == 0 || self_node_type == 1) {
        /* SOURCE data */
        if ((logic_time >= 0 && logic_time <= 2) || (logic_time >= 6 && logic_time <= 11))
            return 40.0; /* Maximum flow rate*/
        if (logic_time >= 12 && logic_time <= 17)
            return 20.0;
        if (logic_time >= 3 && logic_time <= 5)
            return 0.0;
    }
    if (self_node_type == 2 && position == 0) {
        /* FORK data sensor 0*/
        if ((logic_time >= 0 && logic_time <= 2) || (logic_time >= 9 && logic_time <= 11) || (logic_time >= 15 && logic_time <= 17))
            return 20.0;
        if (logic_time >= 6 && logic_time <= 8)
            return 10.0;
        if ((logic_time >= 3 && logic_time <= 5) || (logic_time >= 12 && logic_time <= 14))
            return 0.0;
    }
    if (self_node_type == 2 && position == 1) {
        /* FORK data sensor 1*/
        if ((logic_time >= 0 && logic_time <= 2) || (logic_time >= 9 && logic_time <= 11))
            return 20.0;
        if (logic_time >= 6 && logic_time <= 8)
            return 10.0;
        if ((logic_time >= 3 && logic_time <= 5) || (logic_time >= 12 && logic_time <= 17))
            return 0.0;
    }
    if (self_node_type == 3 && position == 0) {
        /* BRANCH 0 data */
        if (logic_time >= 0 && logic_time <= 2)
            return 20.0;
        if (logic_time >= 6 && logic_time <= 11)
            return 10.0;
        if ((logic_time >= 3 && logic_time <= 5) || (logic_time >= 12 && logic_time <= 17))
            return 0.0;
    }
    if (self_node_type == 3 && position == 1) {
        /* BRANCH 1 data */
        if ((logic_time >= 0 && logic_time <= 2) || (logic_time >= 9 && logic_time <= 11))
            return 20.0;
        if (logic_time >= 6 && logic_time <= 8)
            return 10.0;
        if ((logic_time >= 3 && logic_time <= 5) || (logic_time >= 12 && logic_time <= 17))
            return 0.0;
    }


    return 0.0;
}