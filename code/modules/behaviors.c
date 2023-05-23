#include <stdio.h>
#include "xtimer.h"

#include "behaviors.h"
#include "sample_genator.h"
#include "drivers_sx127x.h"
#include "payload_formatter.h"

/* Check payload_formatter for more details */
#define MESSAGE_MAXIMUM_LENGTH 21

uint32_t SOURCE_LEAKAGE_PERIOD = US_PER_SEC * 2;
uint32_t SOURCE_DUTY_CYCLE_PERIOD = US_PER_SEC * 1;
uint32_t LATENCY_P2P = US_PER_SEC * 0;

int source_lora_ttn(node_t node) {
    (void)node;

    puts("Beahvior: source_lora_ttn");
    return 0;
}

/* source p2p */

void _source_message_received_clb (node_t node) {
    (void)node;
}

static void send_water_flow_to_children(node_t node, int time) {
    /* Check water flow and send a message to its children if any */
    int water_flow = get_water_flow(node.node_type, node.node_self, time);
    if(water_flow) {

        /* Convert int to char* */
        char str[MESSAGE_MAXIMUM_LENGTH];
        sprintf(str, "%d", water_flow);

        /* Send water flow to the child */
        char* list[2] = {"send_cmd", str};
        char** argv = (char**)&list;
        send_cmd(2, argv);
    }
}

int source_lora_p2p(node_t node) {
    puts("Beahvior: source_lora_p2p");
    
    xtimer_ticks32_t last_wakeup;
    bool is_last_wakeup = false;
    int time = 0;



    while (1) {
        /* Set time for sampling: [0, 60] */
        time = (time+1) % 10;

        send_water_flow_to_children(node, time);

        if (!is_last_wakeup) {
            /* set last_wakeup only the first time */
            is_last_wakeup = true;
            last_wakeup = xtimer_now();
        }
        xtimer_periodic_wakeup(&last_wakeup, SOURCE_LEAKAGE_PERIOD);
    }

    return 0;
}

/* end source p2p */

int fork_lora_p2p(node_t node) {
    (void)node;

    puts("Beahvior: fork_lora_p2p");
    return 0;
}

int branch_lora_p2p(node_t node) {
    (void)node;

    puts("Beahvior: branch_lora_p2p");



    return 0;
}
