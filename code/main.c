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

/* Debug */
#define DEBUG 1

/* Replace with configuration variables */
int IS_TTN = 0;
    
node_t node;

int my_node_config(int argc, char **argv) {
    if (argc <= 3) {
        puts("usage: myconfig <self node-name st-lrwan1-x> <parent node-name st-lrwan1-x> <children node-name st-lrwan1-x>");
        return -1;
    }

    int length;

    if (strcmp("NULL", argv[1]) == 0) {
        node.node_self = NULL;
    } else {
        length = strlen(argv[1]);
        node.node_self = malloc(++length);
        strncpy(node.node_self, argv[1], ++length);
    }

    if (strcmp("NULL", argv[2]) == 0) {
        node.node_father = NULL;
        node.node_type = 0;
    } else {
        length = strlen(argv[2]);
        node.node_father = malloc(++length);
        strncpy(node.node_father, argv[2], ++length);
    }

    if (strcmp("NULL", argv[3]) == 0) {
        node.node_children = NULL;
        node.node_type = 2;
    } else {
        length = strlen(argv[3]);
        /* static modifier since we want the variable last during the runtime */
        static char* children_str;
        children_str = malloc(++length);
        strncpy(children_str, argv[3], ++length);
        node.node_children = &children_str; 
    }

    return 0;
}

int node_config(int argc, char **argv)
{
    if (argc <= 1) {
        puts("usage: config <node-code> with node format st-lrwan1-<node-code>");
        return -1;
    }

    node.node_father = NULL;
    node.node_children = NULL;
    node.node_type = 1;
    
    int children_count = 0;

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

        /* Separate father from child for current pair */
        char *elem = strtok(pairs[i], "-");
        length = strlen(elem);
        char *father = malloc(length + 1);
        strncpy(father, elem, length + 1);

        elem = strtok(NULL, "-");
        length = strlen(elem);
        char *child = malloc(length + 1);
        strncpy(child, elem, length + 1);
        
        //printf("#%d Father: st-lrwan1-%s\t Child: st-lrwan1-%s\n", i, father, child);

        if (strcmp(child, argv[1]) == 0 && node.node_father == NULL) {
            length = strlen(father);
            node.node_father = malloc(length + 1);
            strncpy(node.node_father, father, length + 1);
        }

        if (strcmp(father, argv[1]) == 0) {
            children_count++;
            node.node_children = realloc(node.node_children, children_count*sizeof(char*));
            length = strlen(child);
            node.node_children[children_count - 1] = malloc(length + 1);
            strncpy(node.node_children[children_count - 1], child, length + 1);
        }
        
        /* Free allocated memory */
        free(father);
        free(child);
        father = NULL;
        child = NULL;

    }

    /* Display father information */
    printf("Father of %s: ", argv[1]);
    if (node.node_father == NULL) {
        node.node_type = 0;
        printf("undefined, CHIEF is the root of the tree.\n");
    }
    else {
        printf("st-lrwan1-%s\n", node.node_father);
    }

    /* Display children information */
    printf("Children of %s: ", argv[1]);
    if (node.node_children == NULL) {
        node.node_type = 2;
        printf("undefined, BRANCH is a leaf of the tree.\n");
    }
    else {
        for (int i = 0; i < children_count; i++) printf("st-lrwan1-%s ", node.node_children[i]);
        printf("\n");
    }

    printf("Node type: ");
    if (node.node_type == 0) printf("CHIEF\n");
    else if (node.node_type == 1) printf("FORK\n");
    else printf("BRANCH\n");

    return 0;
}

int check_configuration(void) {

    if (DEBUG) {
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
    if (!node.node_children && !node.node_father && !IS_TTN) {
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
    if(node.node_type == 0 && IS_TTN) {
        /* semtech-loramac  drivers to connect to ttn */
        if(semtech_init()) {
            puts("Unable to init semtech-loramac drivers");
            return 1;
        }
    } else {
        if(init_driver_127x()) {
            puts("Unable to init 127x drivers for lora p2p");
            return 1;
        }
        /* Setup sx127x drivers */
        char* list[4] = {"lora_setup_cmd","125","7","5"};
        char** argv = (char**)&list;
        if(lora_setup_cmd(4, argv)) {
            return -1;
        }
    }

    /* Define behaviours */
    if(node.node_type == 0) {
        /* CHIEF node */
        if(IS_TTN) {
            if (source_lora_ttn(node))
                return 1;
        } else {
            if (source_lora_p2p(node))
                return 1;         
        }
    } else if (node.node_type == 1) {
        if (fork_lora_p2p(node)) {
            return 1;
        }  
    } else {
        if (branch_lora_p2p(node)) {
            return 1;
        } 
    }

    return 0;
}

static const shell_command_t commands[] = {
    { "config",         "Configure node location in the tree",  node_config },
    { "myconfig",       "Configure node parent and children",   my_node_config},
    { "start",          "Start the normal mode",                start },
    { "setup",          "Initialize LoRa modulation settings",  lora_setup_cmd },
    { "send",           "Send raw payload string",              send_cmd },
    { "listen",         "Start raw payload listener",           listen_cmd },
    { "loramac",        "Control Semtech loramac stack",        loramac_handler }
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
