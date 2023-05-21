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

/* Defining node type: 0 for CHIEF, 1 for FORK, 2 for BRANCH */
static int node_type;

/* Replace with configuration variables */
static int IS_TTN = 0;

/* Node father and children */
static char* node_father;
static char* node_self;
static char** node_children;

int my_node_config(int argc, char **argv) {
    if (argc <= 3) {
        puts("usage: myconfig <self node-name st-lrwan1-x> <parent node-name st-lrwan1-x> <children node-name st-lrwan1-x>");
        return -1;
    }

    int length;

    if(strcmp("NULL", argv[1]) == 0) {
        node_self = NULL;
    } else {
        length = strlen(argv[1]);
        node_self = malloc(++length);
        strncpy(node_self, argv[1], ++length);
    }

    if(strcmp("NULL", argv[2]) == 0) {
        node_father = NULL;
    } else {
        length = strlen(argv[2]);
        node_father = malloc(++length);
        strncpy(node_father, argv[2], ++length);
    }

    if(strcmp("NULL", argv[3]) == 0) {
        node_children = NULL;
    } else {
        length = strlen(argv[3]);
        /* static modifier since we want the variable last during the runtime */
        static char* children_str;
        children_str = malloc(++length);
        strncpy(children_str, argv[3], ++length);
        node_children = &children_str; 
    }

    return 0;
}

int node_config(int argc, char **argv)
{
    if (argc <= 1) {
        puts("usage: config <node-code> with node format st-lrwan1-<node-code>");
        return -1;
    }

    node_father = NULL;
    node_children = NULL;
    node_type = 1;
    
    char* node_father = NULL;
    int children_count = 0;
    char** node_children = NULL;

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

        if (strcmp(child, argv[1]) == 0 && node_father == NULL) {
            length = strlen(father);
            node_father = malloc(length + 1);
            strncpy(node_father, father, length + 1);
        }

        if (strcmp(father, argv[1]) == 0) {
            children_count++;
            node_children = realloc(node_children, children_count*sizeof(char*));
            length = strlen(child);
            node_children[children_count - 1] = malloc(length + 1);
            strncpy(node_children[children_count - 1], child, length + 1);
        }
        
        /* Free allocated memory */
        free(father);
        free(child);
        father = NULL;
        child = NULL;

    }

    /* Display father information */
    printf("Father of %s: ", argv[1]);
    if (node_father == NULL) {
        node_type = 0;
        printf("undefined, CHIEF is the root of the tree.\n");
    }
    else {
        printf("st-lrwan1-%s\n", node_father);
    }

    /* Display children information */
    printf("Children of %s: ", argv[1]);
    if (node_children == NULL) {
        node_type = 2;
        printf("undefined, BRANCH is a leaf of the tree.\n");
    }
    else {
        for (int i = 0; i < children_count; i++) printf("st-lrwan1-%s ", node_children[i]);
        printf("\n");
    }

    printf("Node type: ");
    if (node_type == 0) printf("CHIEF\n");
    else if (node_type == 1) printf("FORK\n");
    else printf("BRANCH\n");

    /* Free allocated memory */
    free(node_father);
    node_father = NULL;

    for (int i=0; i < children_count; i++) {
        free(node_children[i]);
        node_children[i] = NULL;
    }

    free(node_children);
    node_children = NULL;

    return 0;
}

int check_configuration(void) {

    if (DEBUG) {
        if (node_self == NULL) {
            printf("node_self is NULL\n");
        } else {
            printf("node_self: %s\n", node_self);
        }
        if (node_father == NULL) {
            printf("node_father is NULL\n");
        } else {
            printf("node_father: %s\n", node_father);
        }
        if (node_children == NULL) {
            printf("node_children is NULL\n");
        } else {
            printf("node_children: %s\n", *node_children);
        }
    }

    /* node_self should always be set, also one between node_father and node_children */
    if (!node_self) {
        puts("Configuration error: node_self is NULL");
        return 1;
    } 
    if (!node_children && !node_father && !IS_TTN) {
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
    if(node_type == 0 && IS_TTN) {
        /* semtech-loramac  drivers to connect to ttn */
        if(semtech_init()) {
            puts("Unable to init semtech-loramac drivers");
            return 1;
        }
    } else {
        if(init_driver_127x()) {
            puts("Inable to init 127x drivers for lora p2p");
            return 1;
        }
    }

    /* Define behaviours */
    if(node_type == 0) {
        /* CHIEF node */
        if(IS_TTN) {
            if (source_lora_ttn())
                return 1;
        } else {
            if (source_lora_p2p())
                return 1;         
        }
    } else if (node_type == 1) {
        if (fork_lora_p2p()) {
            return 1;
        }  
    } else {
        if (branch_lora_p2p()) {
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
