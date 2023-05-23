#include <stdio.h>
#include "xtimer.h"

#include "behaviors.h"
#include "sample_genator.h"
#include "drivers_sx127x.h"
#include "payload_formatter.h"
#include "semtech-loramac.h"

/* Check payload_formatter for more details */
#define MESSAGE_MAXIMUM_LENGTH 21

uint32_t SOURCE_LEAKAGE_PERIOD = US_PER_SEC * 2;
uint32_t SOURCE_DUTY_CYCLE_PERIOD = US_PER_SEC * 1;
uint32_t LATENCY_P2P = US_PER_SEC * 0;

int source_lora_ttn(node_t node) {

    puts("Beahvior: source_lora_ttn");

    /* json to publish on TTN */
    char json[128];

    /* Sampling time */
    int s_time = 0;
    /* Current date and time */
    //char datetime[20];
    //time_t current;

    while(1) {
        /*time(&current);
        struct tm* t = localtime(&current);
        int c = strftime(datetime, sizeof(datetime), "%Y-%m-%d %T", t);
        if(c == 0) {
            printf("Error: invalid format.\n");
            return -1;
        }*/
        /* Set time for sampling: [0, 60] */
        s_time = (s_time+1) % 10;

        /* Get water flow value */
        int water_flow = get_water_flow(node.node_type, node.node_self, s_time);
        /* Fill json document */
        sprintf(json, "{\"node_id\": \"%s\", \"datetime\": \"%s\", \"water_flow\": \"%d\"}", node.node_self, "datetime", water_flow);
        
        puts(json);

        /* Set TTN application parameters */
        char* deveui_list[4] = {"loramac", "set", "deveui", "70B3D57ED005D65B"};
        char** argv = (char**)&deveui_list;
        loramac_handler(4,argv);

        char* appeui_list[4] = {"loramac", "set", "appeui", "0000000000000011"};
        argv = (char**)&appeui_list;
        loramac_handler(4,argv);

        char* appkey_list[4] = {"loramac", "set", "appkey", "31AD5FCDDCD4F37EAC0293F186A9AB72"};
        argv = (char**)&appkey_list;
        loramac_handler(4,argv);

        char* join_list[3] = {"loramac", "join", "otaa"};
        argv = (char**)&join_list;
        loramac_handler(3,argv);

        /* Send a message */
        /*char* send_list[2] = {"send_cmd", json};
        argv = (char**)&send_list;
        send_cmd(2, argv);*/

        char* tx_list[3] = {"loramac", "tx", json};
        argv = (char**)&tx_list;
        loramac_handler(3, argv);

        /* Sleeping for five seconds */
        xtimer_sleep(5);
    }

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
        char str_water_flow[MESSAGE_MAXIMUM_LENGTH];
        sprintf(str_water_flow, "%d", water_flow);

        /* Send water flow to the child */
        char* list[2] = {"send_cmd", format_payload(str_water_flow, node.node_self, node.node_children)};
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
