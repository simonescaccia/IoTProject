#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "xtimer.h"
#include "mutex.h"

#include "behaviors.h"
#include "sample_generator.h"
#include "drivers_sx127x.h"
#include "payload_formatter.h"
#include "semtech-loramac.h"

#include "app_debug.h"

/* Check payload_formatter for more details */
#define VALUE_MAXIMUM_LENGTH 14
#define LOGIC_TIME_MAXIMUM_LENGTH 5

/* Leakage */
#define LEAKAGE_CONDITION 0 /* L/min */

uint32_t LEAKAGE_TEST_PERIOD = US_PER_SEC * 15;
uint32_t LATENCY_P2P = US_PER_SEC * 0;

int tx_complete;

int source_lora_ttn(node_t node) {

    puts("Beahvior: source_lora_ttn");

    /* json to publish on TTN */
    char json[128];

    /* Sampling time */
    int s_time = -1;
    /* Current date and time */
    //char datetime[20];
    //time_t current;

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

    char* dr_list[4] = {"loramac", "set", "dr", "5"};
    argv = (char**)&dr_list;
    loramac_handler(4,argv);

    char* join_list[3] = {"loramac", "join", "otaa"};
    argv = (char**)&join_list;
    loramac_handler(3,argv);

    while(1) {
        /*time(&current);
        struct tm* t = localtime(&current);
        int c = strftime(datetime, sizeof(datetime), "%Y-%m-%d %T", t);
        if(c == 0) {
            printf("Error: invalid format.\n");
            return -1;
        }*/

        /* Set time for sampling: [0, 17] */
        s_time = (s_time+1) % 18;

        /* Get water flow value */
        int water_flow = get_water_flow(node.node_type, 0, s_time);
        /* Fill json document */
        sprintf(json, "{\"node_id\": \"%s\", \"datetime\": \"%s\", \"water_flow\": \"%d\"}", node.node_self, "datetime", water_flow);
        
        puts(json);

        /* Send a message 
        char* send_list[2] = {"send_cmd", json};
        argv = (char**)&send_list;
        send_cmd(2, argv);*/

        char* tx_list[3] = {"loramac", "tx", json};
        argv = (char**)&tx_list;
        loramac_handler(3, argv);

        /* Sleeping for five seconds */
        xtimer_sleep(5);

        /* AWS integration */
        /* TTNClient = mqtt.Client()
        AWSClient = AWSIoTMQTTClient("TTNbridge")

        // Connect the clients
        TTNClient.connect("eu.thethings.network", 1883, 60)
        AWSClient.connect()
        TTNClient.username_pw_set("<application-id>", "<access-key>")
        */
    }

    return 0;
}

static void _start_listening (void) {
    /* listen for lora messages */
    char* list[1] = {"listen_cmd"};
    char** argv = (char**)&list;
    listen_cmd(1, argv);
}

static void _send_water_flow_to_children(node_t node, int time) {
    /* Check water flow for each sensor and send a message to its children if any */
    int* water_flow = (int*)malloc(sizeof(int)*node.children_count);
    int water_flow_sum = 0;
    for (int i = 0; i < node.children_count; i++) {
        /* Sample */
        water_flow[i] = get_water_flow(node.node_type, i, time);
        /* Sum */
        water_flow_sum += water_flow[i];
    } 

    if (water_flow_sum) {

        if(APP_DEBUG) printf("Water flow sum: %d\n\n", water_flow_sum);

        /* Convert the time from int to string */
        char str_time[LOGIC_TIME_MAXIMUM_LENGTH];
        sprintf(str_time, "%d", time); 
        /* Convert the water flow from int to char* and split it between children */
        char** str_water_flow = (char**)malloc(sizeof(char*));
        for (int i = 0; i < node.children_count; i++) {
            str_water_flow[i] = (char*)malloc(sizeof(char)*VALUE_MAXIMUM_LENGTH);
            sprintf(str_water_flow[i], "%d", water_flow[i]);
        }

        free(water_flow);

        /* Send water flow to children */
        for (int i = 0; i < node.children_count; i++) {
            tx_complete = 0;
            char* list[2] = {"send_cmd", format_payload(str_water_flow[i], node.node_self, node.node_children[i], "V", str_time)};
            char** argv = (char**)&list;
            send_cmd(2, argv);

            /* Waiting the transmission complete */
            while (!tx_complete) {
                puts("Here");
                /* The sendere thread has less priority, so we need to sleep a little bit */
                xtimer_sleep(0.5);
            }
        }
        
        /* Restart listening */
        _start_listening();
    }
}

void _check_leakage (node_t node, payload_t* payload) {
    /* Sample */
    int water_flow = get_water_flow(node.node_type, node.self_children_position, atoi(payload->logic_time));
    printf("Current water flow: %d. ", water_flow);

    /* Compute the difference */
    int difference = atoi(payload->value) - water_flow;

    if (difference > LEAKAGE_CONDITION) {
        /* Leakage detected */
        puts("leakage detected, sending a message to the source");

        /* Convert the differece from int to char* */
        char str_difference[VALUE_MAXIMUM_LENGTH];
        sprintf(str_difference, "%d", difference);

        /* Wait for source switch to listen mode */
        if (strcmp(node.node_father, node.node_source_p2p) == 0) {
            xtimer_sleep(0.5);
        }

        /* Send a message to the source */
        char* list[2] = {"send_cmd", format_payload(str_difference, node.node_self, node.node_source_p2p, "L", payload->logic_time)};
        char** argv = (char**)&list;
        send_cmd(2, argv);

        /* Restart listening */
        _start_listening();
    } else {
        puts("No leakage detected\n");
    }
}

void transmission_complete_clb (void) {
    puts("Callback on trasmission complete");
    tx_complete = 1;
}

void message_received_clb (node_t node, char message[32]) {
    if (APP_DEBUG) puts("Callback invoked, starting message parsing");

    if (strlen(message) > 31) {
        printf("Extraneous message received, message lenght: %d\n", strlen(message));
        return;
    }

    /* Message parsing */
    payload_t* payload = get_values(message);
    if (!payload) {
        /* Not a message from our application */
        if (APP_DEBUG) puts("Not a message from our application");
        return;
    }

    /* Check destination */
    if (strcmp(payload->to, node.node_self) != 0) {
        /* Message not sent to me */
        if (APP_DEBUG) puts("Message not sent to me");
        return;
    }
    
    /* Compute the sender of the message */
    if (strcmp(payload->from, node.node_father) == 0) {
        /* Message sent from the parent */
        printf("Message from the parent received: %s\n", message);

        /* Check leakage */
        _check_leakage(node, payload);
        return;
    }

    /* The CHIEF receive all the leakage messages */
    if (node.node_type == 1 && strcmp(payload->is_leak, "L") == 0) {
        printf("Message of leakage received: %s\n\n", message);

        /* UART send message to SOURCE TTN*/

        return;        
    }

}

int lora_p2p(node_t node) {
    puts("Behavior: lora_p2p");
    
    xtimer_ticks32_t last_wakeup;
    bool is_last_wakeup = false;
    int time = -1;

    _start_listening();

    while (1) {
        /* Set time for sampling: [0, 17] */
        time = (time+1) % 18;

        /* BRANCH doesn't have children */
        if (node.node_type != 3) _send_water_flow_to_children(node, time);

        /* Duty cycle */
        if (!is_last_wakeup) {
            /* set last_wakeup only the first time */
            is_last_wakeup = true;
            last_wakeup = xtimer_now();
        }
        xtimer_periodic_wakeup(&last_wakeup, LEAKAGE_TEST_PERIOD);
    }

    return 0;
}
