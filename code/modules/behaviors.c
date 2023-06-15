#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#include "xtimer.h"

#include "behaviors.h"
#include "sample_generator.h"
#include "drivers_sx127x.h"
#include "payload_formatter.h"
#include "semtech-loramac.h"
#include "fmt.h"
#include "config.h"

#include "params.h"
#include "app_debug.h"

/* Check payload_formatter for more details */
#define VALUE_MAXIMUM_LENGTH 14
#define LOGIC_TIME_MAXIMUM_LENGTH 5

/* Leakage */
#define LEAKAGE_CONDITION 0 /* L/min */
#define LEAKAGE_THRESHOLD 1.0

/* Setting TTN parameters */
#define DEV_EUI "70B3D57ED005D1D6"
#define APP_EUI "0000000000000011"
#define APP_KEY "5F129D225F930EB831FBE861B3B307D0"

uint32_t LATENCY_P2P = US_PER_SEC * 0;

int tx_complete_child;
char message[20];

/* Thread stack */
#define SX127X_STACKSIZE        (THREAD_STACKSIZE_DEFAULT)
static char stack_listen[SX127X_STACKSIZE];
static char stack_send[SX127X_STACKSIZE];

int source_lora_ttn(node_t node) 
{
    puts("Beahvior: source_lora_ttn");

    /* json to publish on TTN */
    char json[128];

    /* Sampling time */
    int s_time = -1;
    /* Current date and time */
    //char datetime[20];
    //time_t t;

    /* Set TTN application parameters */
    char* deveui_list[4] = {"loramac", "set", "deveui", DEV_EUI};
    char** argv = (char**)&deveui_list;
    loramac_handler(4,argv);

    char* appeui_list[4] = {"loramac", "set", "appeui", APP_EUI};
    argv = (char**)&appeui_list;
    loramac_handler(4,argv);

    char* appkey_list[4] = {"loramac", "set", "appkey", APP_KEY};
    argv = (char**)&appkey_list;
    loramac_handler(4,argv);

    char* dr_list[4] = {"loramac", "set", "dr", "5"};
    argv = (char**)&dr_list;
    loramac_handler(4,argv);

    char* join_list[3] = {"loramac", "join", "otaa"};
    argv = (char**)&join_list;
    loramac_handler(3,argv);

    /* Get water flow value */
    float water_flow = get_water_flow(node.node_type, 0, s_time);
    /* Send source water flow information */
    sprintf(json, "{\"Id\": \"flow\", \"Flow\": \"%f\"}", water_flow);
    puts(json);

    char* tx_list[3] = {"loramac", "tx", json};
    argv = (char**)&tx_list;
    loramac_handler(3, argv);

    /* Sleeping for five seconds */
    xtimer_sleep(5);

    /* Extracting configuration information */
    char* buffer = config();
    int buf_len = strlen(buffer);
    char buf[buf_len + 1];
    strncpy(buf, buffer, buf_len + 1);

    char** pairs = NULL;
    int pair_count = 0;
 
    /* Do not consider first pair */
    char* pair = strtok(buf, " ");
    int length;

    /* Extracting pairs */
    while (pair != NULL) {
        pair = strtok(NULL, " ");
        pair_count++;
        pairs = realloc(pairs, pair_count*sizeof(char*));
        length = strlen(pair);
        pairs[pair_count - 1] = malloc(length + 1);
        strncpy(pairs[pair_count - 1], pair, length + 1);
    }

    node_t** nodes = NULL;
    int node_count = 0;

    /* Retrieving node information from topology pairs */
    for (int i=0; i < pair_count; i++) {

        /* Separate elements for current pair */
        char *elem = strtok(pairs[i], "-");
        length = strlen(elem);

        /* First node code */
        char *node_code_1 = malloc(length + 1);
        strncpy(node_code_1, elem, length + 1);

        /* First node name */
        char* node_name_1 = malloc(15*sizeof(char));
        strcpy(node_name_1, "st-lrwan1-");
        strcat(node_name_1, node_code_1);

        /* Getting second element of the pair */
        elem = strtok(NULL, "-");
        length = strlen(elem);

        /* Second node code */
        char *node_code_2 = malloc(length + 1);
        strncpy(node_code_2, elem, length + 1);

        /* Second node name */
        char* node_name_2 = malloc(15*sizeof(char));
        strcpy(node_name_2, "st-lrwan1-");
        strcat(node_name_2, node_code_2);

        /* Check if the nodes of the pair are already in nodes */
        int node_1_in = 0, node_2_in = 0;

        for (int i = 0; i < node_count; i++) {
            if (nodes[i]->node_self == node_name_1) {
                node_1_in = 1;
            }
            if (nodes[i]->node_self == node_name_2) {
                node_2_in = 1;
            }
        }

        if (!node_1_in) {

            node_count++;
            nodes = realloc(nodes, node_count*sizeof(node_t*));
            node_t* node = malloc(sizeof(node_t));
            node->node_self = node_name_1;

            if (i == 0) {
                /* CHIEF node type */
                node->node_type = 1;
            }
            else {
                /* Default: BRANCH node type */
                node->node_type = 2;
            }

            node->children_count++;
            node->node_children = realloc(node->node_children, node->children_count*sizeof(char*));
            length = strlen(node_name_2);
            node->node_children[node->children_count - 1] = malloc((length + 1)*sizeof(char));
            strncpy(node->node_children[node->children_count - 1], node_name_2, length + 1);
            node->self_children_position = node->children_count;

            /* Add node to the nodes array */
            nodes[node_count - 1] = malloc(sizeof(node_t));
            memcpy(node, nodes[node_count - 1], sizeof(node_t)); 

        }

        if (!node_2_in) {

            node_count++;
            nodes = realloc(nodes, node_count*sizeof(node_t*));
            node_t* node = malloc(sizeof(node_t));
            node->node_self = node_name_2;

            /* Default: FORK node type */
            node->node_type = 2;

            length = strlen(node_name_1);
            node->node_father = malloc((length + 1)*sizeof(char));
            strncpy(node->node_father, node_name_1, length + 1);

            /* Add node to the nodes array */
            nodes[node_count - 1] = malloc(sizeof(node_t));
            memcpy(node, nodes[node_count - 1], sizeof(node_t));
        }
        
        /* Free allocated memory */
        free(node_code_1);
        free(node_code_2);
        free(node_name_1);
        free(node_name_2);
        node_code_1 = NULL;
        node_code_2 = NULL;
        node_name_1 = NULL;
        node_name_2 = NULL;

    }

    /* Update node types */
    for (int i = 0; i < node_count; i++) {
        if (nodes[i]->node_type != 1 && nodes[i]->node_children == NULL) {
            /* BRANCH node type */
            nodes[i]->node_type = 3;
        }
    }

    while(1) {
        /* Set time for sampling: [0, 60] */
        s_time = (s_time+1) % 10;


        for (int i=0; i < pair_count; i++) {

            /* Separate elements for current pair */
            char *elem = strtok(pairs[i], "-");
            length = strlen(elem);

            /* First node code */
            char *node_code_1 = malloc(length + 1);
            strncpy(node_code_1, elem, length + 1);

            /* First node name */
            char* node_name_1 = malloc(15*sizeof(char));
            strcpy(node_name_1, "st-lrwan1-");
            strcat(node_name_1, node_code_1);
        
            /* Second node code */
            elem = strtok(NULL, "-");
            length = strlen(elem);
            char *node_code_2 = malloc(length + 1);
            strncpy(node_code_2, elem, length + 1);

            /* Second node name */
            char* node_name_2 = malloc(15*sizeof(char));
            strcpy(node_name_2, "st-lrwan1-");
            strcat(node_name_2, node_code_2);

            node_t* father_node = malloc(sizeof(node_t));
            node_t* child_node = malloc(sizeof(node_t));

            for (int i = 0; i < node_count; i++) {
                if (nodes[i]->node_self == node_name_1) {
                    memcpy(father_node, nodes[i], sizeof(node_t));
                }
                if (nodes[i]->node_self == node_name_2) {
                    memcpy(child_node, nodes[i], sizeof(node_t));
                }
            }

            /* Get water flow value */
            float father_water_flow = get_water_flow(father_node->node_type, father_node->self_children_position, s_time);
            float child_water_flow = get_water_flow(child_node->node_type, child_node->self_children_position, s_time);

            float difference = fabs(child_water_flow - father_water_flow);

            if (difference > LEAKAGE_THRESHOLD) {
                sprintf(json, "{\"Id\": \"leakage\", \"Child\": \"%s\", \"Father\": \"%s\", \"Leakage\": \"%f\"}", node_name_2, node_name_1, difference); 
                puts(json);

                char* lx_list[3] = {"loramac", "tx", json};
                argv = (char**)&lx_list;
                loramac_handler(3, argv);

                xtimer_sleep(5);
            }
        
            /* Free allocated memory */
            free(node_name_1);
            free(node_name_2);
            node_name_1 = NULL;
            node_name_2 = NULL;

        }

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

static void _start_listening (void) 
{
    /* listen for lora messages */
    char* list[1] = {"listen_cmd"};
    char** argv = (char**)&list;
    listen_cmd(1, argv);
}

/** Restart listening only in the following conditions (checked in listen_cmd):
 * Alway listening: DUTY_CYCLE = 0 or node.node_type = 1 (CHIEF)
 * I was listening before the message send
*/
static void _restart_listening (void) {
    char* list[2] = {"listen_cmd", "resend"};
    char** argv = (char**)&list;
    listen_cmd(2, argv);
}

static void _sample (sample_t* sample, node_t node, int time) 
{
    /* Count the number of sensors for this board */
    int sensors_number;
    if (node.children_count <= 1) 
        sensors_number = 1;
    else 
        sensors_number = node.children_count;

    /* Check water flow for each sensor and send a message to its children if any */
    sample->water_flow = (float*)malloc(sizeof(float)*node.children_count);
    sample->water_flow_sum = 0.0;
    printf("Sampling: ");
    for (int i = 0; i < sensors_number; i++) {
        /* Sample */
        sample->water_flow[i] = get_water_flow(node.node_type, i, time);
        printf("Sensor %d, value: ", i); print_float(sample->water_flow[i], 2); printf(". ");
        /* Sum */
        sample->water_flow_sum += sample->water_flow[i];
    }
    printf("Total water flow: "); print_float(sample->water_flow_sum, 2); printf("\n\n");
}

static void _send_water_flow_to_children(node_t node, int time) 
{    
    /* Get the value of the water flow */
    sample_t sample;
    _sample(&sample, node, time);

    if (sample.water_flow_sum) {

        if(APP_DEBUG) { printf("Water flow sum: "); print_float(sample.water_flow_sum, 2); printf("\n\n"); }
        int printed_chars;

        /* Convert the time from int to string */
        char str_time[LOGIC_TIME_MAXIMUM_LENGTH];
        sprintf(str_time, "%d", time); 
        /* Convert the water flow from int to char* and split it between children */
        char** str_water_flow = (char**)malloc(sizeof(char*));
        for (int i = 0; i < node.children_count; i++) {
            str_water_flow[i] = (char*)malloc(sizeof(char)*VALUE_MAXIMUM_LENGTH);
            printed_chars = fmt_float(str_water_flow[i], sample.water_flow[i], 2);
            str_water_flow[i][printed_chars] = '\0';
        }

        free(sample.water_flow);            

        char* str_payload = NULL;

        /* Send water flow to children */
        for (int i = 0; i < node.children_count; i++) {
            tx_complete_child = 0;
            str_payload = format_payload(str_water_flow[i], node.node_self, node.node_children[i], "V", str_time);
            char* list[2] = {"send_cmd", str_payload};
            char** argv = (char**)&list;
            send_cmd(2, argv);

            /* Wait for transmission complete */
            while (!tx_complete_child) {
                /* The sendere thread has less priority, so we need to sleep a little bit */
                xtimer_msleep(100);
            }
        }

        /* Restart listening */
        _restart_listening();

        free(str_payload);
        /* Free memory */
        for (int i = 0; i < node.children_count; i++) {
            free(str_water_flow[i]);
        }
        free(str_water_flow);
    }
}

void _check_leakage (node_t node, payload_t* payload) {
    /* Get the value of the water flow */
    sample_t sample;
    _sample(&sample, node, atoi(payload->logic_time));
    free(sample.water_flow);
    printf("Current water flow: "); print_float(sample.water_flow_sum, 2); printf(". ");

    /* Compute the difference */
    float difference = atof(payload->value) - sample.water_flow_sum;

    if (difference > LEAKAGE_CONDITION) {
        /* Leakage detected */
        puts("Leakage detected, sending a message to the source");

        int printed_chars;

        /* Convert the differece from float to char* */
        char str_difference[VALUE_MAXIMUM_LENGTH];
        printed_chars = fmt_float(str_difference, difference, 2);
        str_difference[printed_chars] = '\0';

        /* Wait for source switch to listen mode */
        if (strcmp(node.node_father, node.node_source_p2p) == 0) {
            xtimer_msleep(100);
        }

        /* Send a message to the source */
        char* str_payload = format_payload(str_difference, node.node_self, node.node_source_p2p, "L", payload->logic_time);
        char* list[2] = {"send_cmd", str_payload};
        char** argv = (char**)&list;
        send_cmd(2, argv);

        /* Restart listening */
        _restart_listening();

        /* Free memory */
        free(str_payload);

    } else {
        puts("No leakage detected\n");
    }
}

void transmission_complete_clb (void) {
    if (APP_DEBUG) puts("Callback on trasmission complete");
    tx_complete_child = 1;
}

/**
 * @return 1 if the message is sent from the parent, then you need to stop listening, else return 0
*/
int message_received_clb (node_t node, char message[32]) {
    if (APP_DEBUG) puts("Callback invoked, starting message parsing");

    if (strlen(message) > 31) {
        printf("Extraneous message received, message lenght: %d\n", strlen(message));
        return 0;
    }

    /* Message parsing */
    payload_t* payload = get_values(message);
    if (!payload) {
        /* Not a message from our application */
        if (APP_DEBUG) puts("Not a message from our application");
        
        return 0;
    }

    /* Check destination */
    if (strcmp(payload->to, node.node_self) != 0) {
        /* Message not sent to me */
        if (APP_DEBUG) puts("Message not sent to me");

        free_payload(payload);
        return 0;
    }
    
    /* Compute the sender of the message */
    if (strcmp(payload->from, node.node_father) == 0) {
        /* Message sent from the parent */
        printf("Message from the parent received: %s\n", message);

        /* Check leakage */
        _check_leakage(node, payload);

        free_payload(payload);
        return 1;
    }

    /* The CHIEF receive all the leakage messages */
    if (node.node_type == 1 && strcmp(payload->is_leak, "L") == 0) {
        printf("Message of leakage received: %s\n\n", message);

        /* UART send message to SOURCE TTN*/

        free_payload(payload);
        return 0;        
    }

    return 0;
}

void *_periodic_listening(void *arg) {

    (void)arg;
    xtimer_ticks32_t last_wakeup;
    bool is_last_wakeup = false;

    while (1) {
        _start_listening();

        /* Duty cycle */
        if (!is_last_wakeup) {
            /* set last_wakeup only the first time */
            is_last_wakeup = true;
            last_wakeup = xtimer_now();
        }
        xtimer_periodic_wakeup(&last_wakeup, US_PER_SEC * SIMULATED_DAY);
    }
}

void *_periodic_sending(void *arg) {

    xtimer_ticks32_t last_wakeup;
    bool is_last_wakeup = false;
    /* Starting logic time for the sample generator */
    int time = 3;
    node_t node = *(node_t *)arg;

    while (1) {
        /* Set time for sampling: [0, inf) */
        time = (time + 1);

        _send_water_flow_to_children(node, time);

        /* Duty cycle */
        if (!is_last_wakeup) {
            /* set last_wakeup only the first time */
            is_last_wakeup = true;
            last_wakeup = xtimer_now();
        }
        xtimer_periodic_wakeup(&last_wakeup, US_PER_SEC * LEAKAGE_TEST_PERIOD);
    }
}

int lora_p2p(node_t node) {

    puts("Behavior: lora_p2p");

    /* Start listening: periodic if DUTY_CYCLE is setted, else continuous listening */
    if (DUTY_CYCLE && node.node_type != 1) {
        kernel_pid_t _listen_pid = thread_create(stack_listen, sizeof(stack_listen), THREAD_PRIORITY_MAIN - 1,
                                THREAD_CREATE_STACKTEST, _periodic_listening, NULL,
                                "_periodic_listening");

        if (_listen_pid <= KERNEL_PID_UNDEF) {
            puts("Creation of _periodic_listening thread failed");
            return 1;
        }
    } else {
        _start_listening();
    }

    /* Start sending: only if the current node is not a BRANCH */
    if (node.node_type != 3) {
        kernel_pid_t _sending_pid = thread_create(stack_send, sizeof(stack_send), THREAD_PRIORITY_MAIN - 1,
                                THREAD_CREATE_STACKTEST, _periodic_sending, (void *)&node,
                                "_periodic_sending");

        if (_sending_pid <= KERNEL_PID_UNDEF) {
            puts("Creation of _periodic_sending thread failed");
            return 1;
        } 
    }

    return 0;
}
