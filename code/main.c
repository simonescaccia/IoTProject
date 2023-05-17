#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string.h>

#include "thread.h"
#include "fmt.h"

#include "xtimer.h"

#include "shell.h"

#include "drivers_sx127x.h"
#include "semtech-loramac.h"

/* Debug */
#define DEBUG 1

/* Configuration file */
#define FILENAME "config.txt"


int node_config(int argc, char **argv)
{
    if (argc <= 1) {
        puts("usage: config <node-name> with node format <st-lrwan1-x>");
        return -1;
    }

    /* Open file for reading */
    char *line_buf = NULL;
    size_t line_buf_size = 0;
    ssize_t line_size;
    FILE *fp = fopen(FILENAME, "r");

    if (!fp) {
        fprintf(stderr, "Error while opening file '%s'\n", FILENAME);
        return -1;
    }

    /* Get the first line of the file */
    line_size = getline(&line_buf, &line_buf_size, fp);

    int line_count = 0;
    char* node_father = NULL;
    int children_count = 0;
    char** node_children = NULL;

    /* Loop through until we are done with the file */
    while (line_size >= 0) {
        /* Increment line count */
        line_count++;

        /* Separate father from child for current line */
        char *elem = strtok(line_buf, " ");
        char *father = malloc(strlen(elem) + 1);
        strcpy(father, elem);

        elem = strtok(NULL, " ");
        /* Removing '\n' */
        elem[strlen(elem) - 1] = '\0';
        char *child = malloc(strlen(elem) + 1);
        strcpy(child, elem);
        
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

        /* Get next line */
        line_size = getline(&line_buf, &line_buf_size, fp);

        /* Free allocated memory */
        free(father);
        free(child);
        father = NULL;
        child = NULL;
    }

    printf("\n");

    /* Defining node type: 0 for CHIEF, 1 for FORK, 2 for BRANCH */
    int node_type = 1;

    /* Display father information */
    printf("Father of %s: ", NODE_NAME);
    if (node_father == NULL) {
    node_type = 0;
    printf("undefined, CHIEF is the root of the tree.\n");
    }
    else {
    printf("%s\n", node_father);
    }

    /* Display children information */
    printf("Children of %s: ", NODE_NAME);
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

    /* Close the file */
    fclose(fp);

    return 0;
}

static const shell_command_t commands[] = {
    { "config",         "Configure node location in the tree",  node_config },
    { "setup",          "Initialize LoRa modulation settings",  lora_setup_cmd },
    { "send",           "Send raw payload string",              send_cmd },
    { "listen",         "Start raw payload listener",           listen_cmd },
    { "loramac",        "Control Semtech loramac stack",        loramac_handler }
};

int main(void)
{

    xtimer_sleep(1);

    puts("Hello World!");

    // init driver 127x
    if(init_driver_127x()){
        return 1;
    }

    /* semtech-loramac init */
    if(semtech_init()) {
        return 1;
    }

    // start shell
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
