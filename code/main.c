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

/* Debug */
#define DEBUG 1

/* Configuration file */
#define FILENAME "config.txt"

/* Defining node type: 0 for CHIEF, 1 for FORK, 2 for BRANCH */
static int node_type;

/* Node father and children */
static char* node_father;
static char* node_self;
static char** node_children;

int my_node_config(int argc, char **argv) {
    if (argc < 3) {
        puts("usage: myconfig <self node-name with node format st-lrwan1-x> <parent node-name with node format st-lrwan1-x> <children node-name with node format st-lrwan1-x>");
        return -1;
    }

    if(strcmp("NULL", argv[2]) == 0) {
        node_father = NULL;
    } else {
        node_father = argv[2];
    }
    if(strcmp("NULL", argv[3]) == 0) {
        *node_children = NULL;
    } else {
        *node_children = argv[3]; 
    }
    node_self = argv[1];

    printf("configuration done: self:%s parent:%s childrens:%s", node_self, node_father, node_children[0]);

    return 0;
}

int node_config(int argc, char **argv)
{
    if (argc <= 1) {
        puts("usage: config <node-name with node format st-lrwan1-x>");
        return -1;
    }

    node_father = NULL;
    node_children = NULL;
    node_type = 1;

    /* Open the file for reading */
    FILE *fp = fopen(FILENAME, "r");

    if (!fp) {
        fprintf(stderr, "Error while opening file '%s'\n", FILENAME);
        return EXIT_FAILURE;
    }
    
    char* node_father = NULL;
    int children_count = 0;
    char** node_children = NULL;
    
    int max_line_buf_size = 128;
    char* line_buf = malloc(max_line_buf_size);
    int line_count = 0;
    
    /* Loop until we are done with the file */
    while (EOF != fscanf(fp, "%[^\n]\n", line_buf)) {

        /* Increment line count */
        line_count++;

        /* Separate father from child for current line */
        char *elem = strtok(line_buf, " ");
        int length = strlen(elem);
        char *father = malloc(++length);
        strncpy(father, elem, ++length);

        elem = strtok(NULL, " ");
        length = strlen(elem);
        char *child = malloc(++length);
        strncpy(child, elem, ++length);
        
        printf("Line #%d -> Father: %s\t Child: %s\n", line_count, father, child);

        if (strcmp(child, argv[1]) == 0 && node_father == NULL) {
        node_father = malloc(strlen(father) + 1);
        strcpy(node_father, father);
        }

        if (strcmp(father, argv[1]) == 0) {
        children_count++;
        node_children = realloc(node_children, children_count*sizeof(char*));
        node_children[children_count - 1] = malloc(strlen(child) + 1);
        strcpy(node_children[children_count - 1], child);
        }
        
        /* Free allocated memory */
        free(father);
        free(child);
        father = NULL;
        child = NULL;
        
        free(line_buf);
        line_buf = malloc(max_line_buf_size);
    }

    printf("\n");

    /* Defining node type: 0 for CHIEF, 1 for FORK, 2 for BRANCH */
    int node_type = 1;

    /* Display father information */
    printf("Father of %s: ", argv[1]);
    if (node_father == NULL) {
        node_type = 0;
        printf("undefined, CHIEF is the root of the tree.\n");
    }
    else {
        printf("%s\n", node_father);
    }

    /* Display children information */
    printf("Children of %s: ", argv[1]);
    if (node_children == NULL) {
        node_type = 2;
        printf("undefined, BRANCH is a leaf of the tree.\n");
    }
    else {
        for (int i = 0; i < children_count; i++) printf("%s ", node_children[i]);
        printf("\n");
    }

    printf("Node type: ");
    if (node_type == 0) printf("CHIEF\n");
    else if (node_type == 1) printf("FORK\n");
    else printf("BRANCH\n");

    /* Free allocated memory */
    free(line_buf);
    line_buf = NULL;

    free(node_father);
    node_father = NULL;

    for (int i=0; i < children_count; i++) {
        free(node_children[i]);
        node_children[i] = NULL;
    }
    free(node_children);
    node_children = NULL;

    /* Close the file now that we are done with it */
    fclose(fp);


    return 0;
}

int check_configuration(void) {
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

    /* Replace with configuration variables */
    const int IS_TTN = 0;

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
