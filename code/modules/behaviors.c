#include <stdio.h>
#include <string.h>

#include "xtimer.h"

#include "behaviors.h"
#include "sample_generator.h"
#include "drivers_sx127x.h"
#include "payload_formatter.h"
#include "semtech-loramac.h"

#include "app_debug.h"

/* Check payload_formatter for more details */
#define MESSAGE_MAXIMUM_LENGTH 21

uint32_t SOURCE_LEAKAGE_PERIOD = US_PER_SEC * 4;
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

        /* Set time for sampling: [0, 60] */
        s_time = (s_time+1) % 10;

        /* Get water flow value */
        int water_flow = get_water_flow(node.node_type, node.node_self, s_time);
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
    /* Check water flow and send a message to its children if any */
    int water_flow = get_water_flow(node.node_type, node.node_self, time);
    if(water_flow) {

        /* Convert the water flow int to char* and split it between children */
        char str_water_flow[MESSAGE_MAXIMUM_LENGTH];
        sprintf(str_water_flow, "%d", water_flow / node.children_count);

        /* Send water flow to children */
        for (int i = 0; i < node.children_count; i++) {
            char* list[2] = {"send_cmd", format_payload(str_water_flow, node.node_self, node.node_children[i], "V")};
            char** argv = (char**)&list;
            send_cmd(2, argv);

            /* Restart listening */
            _start_listening();
        }
    }
}

void message_received_clb (node_t node, char message[32]) {
    puts("Callback invoked, starting message parsing");
    
    /* Message parsing */
    payload_t* payload = get_values(message);
    if (!payload) {
        /* Not a message from our application */
        if (APP_DEBUG) puts("Not a message from our application");
        return;
    }

    xtimer_sleep(2);
    printf("node.node_self -%c-%c-%c \n", node.node_self[0], node.node_self[1], node.node_self[2]);

    xtimer_sleep(2);
    printf("payload->to -%s- \n", payload->to);

    xtimer_sleep(2);
    printf("strcmp-%d- \n", strcmp(payload->to, node.node_self));

    /* Check destination */
    if (strcmp(payload->to, node.node_self) != 0) {
        /* Message not sent to me */
        if (APP_DEBUG) puts("Message not sent to me");
        return;
    }
    
    /* Compute the sender of the message */
    if (strcmp(payload->from, node.node_father) == 0) {
        /* Message sent from the parent */
        puts("Message from the parent received");

        /*  */
        return;
    }

    /* The CHIEF receive all the leakage messages */
    if (node.node_type == 1 && strcmp(payload->is_leak, "L") == 0) {
        puts("Message of leakage received");

        /* UART send message so SOURCE TTN*/
        return;        
    }

}

int source_lora_p2p(node_t node) {
    puts("Beahvior: source_lora_p2p");
    
    xtimer_ticks32_t last_wakeup;
    bool is_last_wakeup = false;
    int time = 0;

    _start_listening();

    while (1) {
        /* Set time for sampling: [0, 60] */
        time = (time+1) % 10;

        _send_water_flow_to_children(node, time);

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

    puts("Beahvior: fork_lora_p2p");
    return 0;
}

int branch_lora_p2p(node_t node) {
    (void)node;

    puts("Beahvior: branch_lora_p2p");



    return 0;
}
