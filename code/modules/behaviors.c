#include <stdio.h>
#include "xtimer.h"

#include "behaviors.h"
#include "sample_genator.h"

uint32_t SOURCE_LEAKAGE_PERIOD = US_PER_SEC * 2;

int source_lora_ttn(node_t node) {
    (void)node;

    puts("Beahvior: source_lora_ttn");
    return 0;
}

int source_lora_p2p(node_t node) {
    puts("Beahvior: source_lora_p2p");
    
    xtimer_ticks32_t last_wakeup;
    bool is_last_wakeup = false;
    int time = 0;

    while (1) {
        /* Set time for sampling: [0, 60] */
        time = (time+1) % 60;

        /* Check water flow and send a message to its children if any */
        if(get_water_flow(node.node_type, node.node_self, time)) {

        }

        if (!is_last_wakeup) {
            /* set last_wakeup only the first time */
            is_last_wakeup = true;
            last_wakeup = xtimer_now();
        }
        xtimer_periodic_wakeup(&last_wakeup, SOURCE_LEAKAGE_PERIOD);
    }

    return 0;
}

int fork_lora_p2p(node_t node) {
    (void)node;

    puts("Beahvior: source_lora_p2p");
    return 0;
}

int branch_lora_p2p(node_t node) {
    (void)node;

    puts("Beahvior: source_lora_p2p");
    return 0;
}
