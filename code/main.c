#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "xtimer.h"

// driver 127x
#include <errno.h>
#include <inttypes.h>

#include "thread.h"
#include "shell.h"

#include "net/netdev.h"
#include "net/netdev/lora.h"
#include "net/lora.h"

#include "board.h"

#include "sx127x_internal.h"
#include "sx127x_params.h"
#include "sx127x_netdev.h"

#include "fmt.h"

#define SX127X_LORA_MSG_QUEUE   (16U)
#define SX127X_STACKSIZE        (THREAD_STACKSIZE_DEFAULT)

#define MSG_TYPE_ISR            (0x3456)

static char stack[SX127X_STACKSIZE];
static kernel_pid_t _recv_pid;

static char message[32];
static sx127x_t sx127x;

/* Debug */
#define DEBUG 1

/* Configuration file */
#define FILENAME "config.txt"

/* Defining node type: 0 for CHIEF, 1 for FORK, 2 for BRANCH */
static int node_type;

/* Node father and children */
static char* node_father;
static char** node_children;


int node_config(int argc, char **argv)
{
    if (argc <= 1) {
        puts("usage: config <node-name> with node format <st-lrwan1-x>");
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

int lora_setup_cmd(int argc, char **argv)
{

    if (argc < 4) {
        puts("usage: setup "
             "<bandwidth (125, 250, 500)> "
             "<spreading factor (7..12)> "
             "<code rate (5..8)>");
        return -1;
    }

    /* Check bandwidth value */
    int bw = atoi(argv[1]);
    uint8_t lora_bw;

    switch (bw) {
    case 125:
        puts("setup: setting 125KHz bandwidth");
        lora_bw = LORA_BW_125_KHZ;
        break;

    case 250:
        puts("setup: setting 250KHz bandwidth");
        lora_bw = LORA_BW_250_KHZ;
        break;

    case 500:
        puts("setup: setting 500KHz bandwidth");
        lora_bw = LORA_BW_500_KHZ;
        break;

    default:
        puts("[Error] setup: invalid bandwidth value given, "
             "only 125, 250 or 500 allowed.");
        return -1;
    }

    /* Check spreading factor value */
    uint8_t lora_sf = atoi(argv[2]);

    if (lora_sf < 7 || lora_sf > 12) {
        puts("[Error] setup: invalid spreading factor value given");
        return -1;
    }

    /* Check coding rate value */
    int cr = atoi(argv[3]);

    if (cr < 5 || cr > 8) {
        puts("[Error ]setup: invalid coding rate value given");
        return -1;
    }
    uint8_t lora_cr = (uint8_t)(cr - 4);

    /* Configure radio device */
    netdev_t *netdev = &sx127x.netdev;

    netdev->driver->set(netdev, NETOPT_BANDWIDTH,
                        &lora_bw, sizeof(lora_bw));
    netdev->driver->set(netdev, NETOPT_SPREADING_FACTOR,
                        &lora_sf, sizeof(lora_sf));
    netdev->driver->set(netdev, NETOPT_CODING_RATE,
                        &lora_cr, sizeof(lora_cr));

    puts("[Info] setup: configuration set with success");

    return 0;
}

int setup_infra(int argc, char **argv)
{
    printf("arguments received %d\n", argc);
    for(int arg=0; arg<argc; arg++){
        printf("argument #%d: %s\n", arg+1,argv[arg]);
    }

    return 0;
}

int send_cmd(int argc, char **argv)
{
    if (argc <= 1) {
        puts("usage: send <payload>");
        return -1;
    }

    printf("sending \"%s\" payload (%u bytes)\n",
           argv[1], (unsigned)strlen(argv[1]) + 1);

    iolist_t iolist = {
        .iol_base = argv[1],
        .iol_len = (strlen(argv[1]) + 1)
    };

    netdev_t *netdev = &sx127x.netdev;

    if (netdev->driver->send(netdev, &iolist) == -ENOTSUP) {
        puts("Cannot send: radio is still transmitting");
    }

    return 0;
}

int listen_cmd(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    netdev_t *netdev = &sx127x.netdev;
    /* Switch to continuous listen mode */
    const netopt_enable_t single = false;

    netdev->driver->set(netdev, NETOPT_SINGLE_RECEIVE, &single, sizeof(single));
    const uint32_t timeout = 0;

    netdev->driver->set(netdev, NETOPT_RX_TIMEOUT, &timeout, sizeof(timeout));

    /* Switch to RX state */
    netopt_state_t state = NETOPT_STATE_RX;

    netdev->driver->set(netdev, NETOPT_STATE, &state, sizeof(state));

    printf("Listen mode set\n");

    return 0;
}

static const shell_command_t commands[] = {
    { "config",         "Configure node location in the tree",  node_config },
    { "setup_infra",    "Setup the infrastructure",             setup_infra },
    { "setup",          "Initialize LoRa modulation settings",  lora_setup_cmd },
    { "send",           "Send raw payload string",              send_cmd },
    { "listen",         "Start raw payload listener",           listen_cmd },
};

static void _event_cb(netdev_t *dev, netdev_event_t event)
{
    if (event == NETDEV_EVENT_ISR) {
        msg_t msg;

        msg.type = MSG_TYPE_ISR;
        msg.content.ptr = dev;

        if (msg_send(&msg, _recv_pid) <= 0) {
            puts("gnrc_netdev: possibly lost interrupt.");
        }
    }
    else {
        size_t len;
        netdev_lora_rx_info_t packet_info;
        switch (event) {
        case NETDEV_EVENT_RX_STARTED:
            puts("Data reception started");
            break;

        case NETDEV_EVENT_RX_COMPLETE:
            len = dev->driver->recv(dev, NULL, 0, 0);
            dev->driver->recv(dev, message, len, &packet_info);
            printf(
                "{Payload: \"%s\" (%d bytes), RSSI: %i, SNR: %i, TOA: %" PRIu32 "}\n",
                message, (int)len,
                packet_info.rssi, (int)packet_info.snr,
                sx127x_get_time_on_air((const sx127x_t *)dev, len));
            break;

        case NETDEV_EVENT_TX_COMPLETE:
            sx127x_set_sleep(&sx127x);
            puts("Transmission completed");
            break;

        case NETDEV_EVENT_CAD_DONE:
            break;

        case NETDEV_EVENT_TX_TIMEOUT:
            sx127x_set_sleep(&sx127x);
            break;

        default:
            printf("Unexpected netdev event received: %d\n", event);
            break;
        }
    }
}

void *_recv_thread(void *arg)
{
    (void)arg;

    static msg_t _msg_q[SX127X_LORA_MSG_QUEUE];

    msg_init_queue(_msg_q, SX127X_LORA_MSG_QUEUE);

    while (1) {
        msg_t msg;
        msg_receive(&msg);
        if (msg.type == MSG_TYPE_ISR) {
            netdev_t *dev = msg.content.ptr;
            dev->driver->isr(dev);
        }
        else {
            puts("Unexpected msg type");
        }
    }
}

int init_driver_127x(void *arg) {

    (void)arg;

    if(DEBUG) 
        puts("[init_driver_127x] starting driver_127x init");

    sx127x.params = sx127x_params[0];
    netdev_t *netdev = &sx127x.netdev;

    netdev->driver = &sx127x_driver;

    netdev->event_callback = _event_cb;

    if (netdev->driver->init(netdev) < 0) {
        puts("Failed to initialize SX127x device, exiting");
        return 1;
    }

    _recv_pid = thread_create(stack, sizeof(stack), THREAD_PRIORITY_MAIN - 1,
                              THREAD_CREATE_STACKTEST, _recv_thread, NULL,
                              "recv_thread");

    if (_recv_pid <= KERNEL_PID_UNDEF) {
        puts("Creation of receiver thread failed");
        return 1;
    }

    if(DEBUG) 
        puts("[init_driver_127x] driver_127x init done");

    return 0;
}

int main(void)
{

    puts("Hello World!");

    // init driver 127x
    if(init_driver_127x(NULL)){
        return 1;
    }

    // start shell
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
