#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "thread.h"
#include "fmt.h"

#include "xtimer.h"

#include "shell.h"

#include "drivers_sx127x.h"
#include "semtech-loramac.h"
#include "behaviors.h"
#include "config.h"
#include "app_debug.h"
    
static node_t node;

int node_config(int argc, char **argv)
{
    if (argc <= 1) {
        puts("usage: config <node-code>");
        return -1;
    }

    node.node_self = NULL;
    node.node_alt_self = NULL;
    node.node_source_p2p = NULL;
    node.node_father = NULL;
    node.node_children = NULL;
    node.node_type = 2;
    node.children_count = 0;

    int valid_node = 0;

    /* Extracting configuration information */
    char* buffer = config();
    int buf_len = strlen(buffer);
    char buf[buf_len + 1];
    strncpy(buf, buffer, buf_len + 1);

    char** pairs = NULL;
    int pair_count = 0;
 
    /* First pair */
    char* pair = strtok(buf, " ");
    int length;

    /* Extracting pairs */
    while (pair != NULL) {
        pair_count++;
        pairs = realloc(pairs, pair_count*sizeof(char*));
        length = strlen(pair);
        pairs[pair_count - 1] = malloc(length + 1);
        strncpy(pairs[pair_count - 1], pair, length + 1);
        pair = strtok(NULL, " ");
    }

    for (int i=0; i < pair_count; i++) {

        /* Separate elements for current pair */
        char *elem = strtok(pairs[i], "-");
        length = strlen(elem);
        char *elem1 = malloc(length + 1);
        strncpy(elem1, elem, length + 1);

        elem = strtok(NULL, "-");
        length = strlen(elem);
        char *elem2 = malloc(length + 1);
        strncpy(elem2, elem, length + 1);
        
        if (i == 0) {
            /* Store source_p2p */
            length = strlen(elem2);
            node.node_source_p2p = (char*)malloc(length + 1);
            strncpy(node.node_source_p2p, elem2, length + 1);
            
            //printf("#%d Source TTN: st-lrwan1-%s\t Source P2P: st-lrwan1-%s\n", i, elem1, elem2);
            if (strcmp(elem1, argv[1]) == 0) {
                valid_node = 1;
                /* Source TTN is simply considered as a node without father and without children */
                node.node_type = 0;
                length = strlen(elem2);
                node.node_alt_self = malloc(length + 1);
                strncpy(node.node_alt_self, elem2, length + 1);
            }
            if (strcmp(elem2, argv[1]) == 0) {
                length = strlen(elem1);
                node.node_alt_self = malloc(length + 1);
                strncpy(node.node_alt_self, elem1, length + 1);
            }
        }
        else {
            //printf("#%d Father: st-lrwan1-%s\t Child: st-lrwan1-%s\n", i, elem1, elem2);

            if (strcmp(elem2, argv[1]) == 0) {
                valid_node = 1;
                length = strlen(elem1);
                node.node_father = malloc(length + 1);
                strncpy(node.node_father, elem1, length + 1);
            }

            if (strcmp(elem1, argv[1]) == 0) {
                valid_node = 1;
                node.children_count++;
                node.node_children = realloc(node.node_children, node.children_count*sizeof(char*));
                length = strlen(elem2);
                node.node_children[node.children_count - 1] = malloc(length + 1);
                strncpy(node.node_children[node.children_count - 1], elem2, length + 1);
            }
        }
        
        /* Free allocated memory */
        free(elem1);
        free(elem2);
        elem1 = NULL;
        elem2 = NULL;

    }
    
    /* Check if node is valid */
    if (!valid_node) {
        printf("config: node not valid for current topology.\n");
        return -1;
    }

    /* Assign node_self*/
    int node_length = strlen(argv[1]);
    node.node_self = (char*)malloc(sizeof(char)*(node_length+1));
    strncpy(node.node_self, argv[1], node_length + 1);

    /* Display node information */
    printf("Node: st-lrwan1-%s\n", argv[1]);

    if (node.node_type != 0) {

        /* Display father information */
        printf("Father of %s: ", argv[1]);
        if (node.node_father == NULL) {
            node.node_type = 1;
            printf("undefined, CHIEF is the root of the tree.\n");
        }
        else {
            printf("st-lrwan1-%s\n", node.node_father);
        }

        /* Display children information */
        printf("Children of %s: ", argv[1]);
        if (node.node_children == NULL) {
            node.node_type = 3;
            printf("undefined, BRANCH is a leaf of the tree.\n");
        }
        else {
            for (int i = 0; i < node.children_count; i++) printf("st-lrwan1-%s ", node.node_children[i]);
            printf("\n");
        }
    }

    printf("Node type: ");
    if (node.node_type == 0) {
        printf("Source TTN\n");
        printf("Known CHIEF (P2P source): st-lrwan1-%s\n", node.node_alt_self);
    }
    else if (node.node_type == 1) {
        printf("CHIEF\n");
        printf("Known TTN source: st-lrwan1-%s\n", node.node_alt_self);
    }
    else if (node.node_type == 2) printf("FORK\n");
    else printf("BRANCH\n");

    return 0;
}

int check_configuration(void) {

    if (APP_DEBUG) {
        if (node.node_self == NULL) {
            printf("node_self is NULL\n");
        } else {
            printf("node_self: %s\n", node.node_self);
        }
        if (node.node_father == NULL) {
            printf("node_father is NULL\n");
        } else {
            printf("node_father: %s\n", node.node_father);
        }
        if (node.node_children == NULL) {
            printf("node_children is NULL\n");
        } else {
            printf("node_children: %s\n", *(node.node_children));
        }
    }

    /* node_self should always be set, also one between node_father and node_children */
    if (!node.node_self) {
        puts("Configuration error: node_self is NULL");
        return 1;
    } 
    if (!node.node_children && !node.node_father && node.node_type != 0) {
        puts("Configuration error: at least one between node_children and node_father ");
        return 1;
    }
    return 0;
}

int start(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    /* Check configuration done*/
    if (check_configuration()) {
        return 1;
    }

    /* Init drivers */
    if(node.node_type == 0) {
        /* semtech-loramac drivers to connect to ttn */
        if(semtech_init()) {
            puts("Unable to init semtech-loramac drivers");
            return 1;
        }
    } else {
        if(init_driver_127x(node)) {
            puts("Unable to init 127x drivers for lora p2p");
            return 1;
        }

        /* Set on messsage received callback */
        set_callback(message_received_clb);

        /* Setup sx127x drivers */
        char* list[4] = {"lora_setup_cmd","250","7","5"};
        char** argv = (char**)&list;
        if(lora_setup_cmd(4, argv)) {
            return -1;
        }
    }

    /* Define behaviours */
    if (node.node_type == 0) {
        if (source_lora_ttn(node)) return 1; 
    }
    else {
        if (lora_p2p(node)) return 1; 
    }

    return 0;
}

static const shell_command_t commands[] = {
    { "config",         "Configure node location in the tree",  node_config },
    { "start",          "Start the normal mode",                start },
    { "setup",          "Initialize LoRa modulation settings",  lora_setup_cmd },
    { "send",           "Send raw payload string",              send_cmd },
    { "listen",         "Start raw payload listener",           listen_cmd },
    { "loramac",        "Control Semtech loramac stack",        loramac_handler },
    { NULL, NULL, NULL }
};

int main(void)
{

    xtimer_sleep(1);

    puts("Hello World!");

    // start shell
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
