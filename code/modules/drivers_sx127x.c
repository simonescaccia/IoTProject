#include <errno.h>
#include <stdio.h>
#include <inttypes.h>

#include "shell.h"

#include "net/netdev.h"
#include "net/netdev/lora.h"
#include "net/lora.h"

#include "board.h"

#include "sx127x_internal.h"
#include "sx127x_params.h"
#include "sx127x_netdev.h"

#include "drivers_sx127x.h"
#include "app_debug.h"
#include "params.h"

#include "xtimer.h"
#include "ztimer64.h"

#define SX127X_LORA_MSG_QUEUE   (16U)
#define SX127X_STACKSIZE        (THREAD_STACKSIZE_DEFAULT)

#define MSG_TYPE_ISR            (0x3456)

static char stack[SX127X_STACKSIZE];
static kernel_pid_t _recv_pid;

static char message[32];
static sx127x_t sx127x;

static int (*callback_on_msg_receive)(node_t, char[32]);
static void (*callback_tx_complete)(void);

static uint64_t timestamp_last_timeout_ms = 0;
static uint64_t last_setted_timeout_ms = 0;

static node_t node;

int lora_setup_cmd(int argc, char **argv)
{

    if (argc < 4) {
        puts("usage: setup "
             "<bandwidth (125, 250, 500)> "
             "<spreading factor (7..12)> "
             "<code rate (5..8)>");
        return -1;
    }

    // Check bandwidth value 
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

    // Check spreading factor value
    uint8_t lora_sf = atoi(argv[2]);

    if (lora_sf < 7 || lora_sf > 12) {
        puts("[Error] setup: invalid spreading factor value given");
        return -1;
    }

    // Check coding rate value
    int cr = atoi(argv[3]);

    if (cr < 5 || cr > 8) {
        puts("[Error ]setup: invalid coding rate value given");
        return -1;
    }
    uint8_t lora_cr = (uint8_t)(cr - 4);

    // Configure radio device
    netdev_t *netdev = &sx127x.netdev;

    netdev->driver->set(netdev, NETOPT_BANDWIDTH,
                        &lora_bw, sizeof(lora_bw));
    netdev->driver->set(netdev, NETOPT_SPREADING_FACTOR,
                        &lora_sf, sizeof(lora_sf));
    netdev->driver->set(netdev, NETOPT_CODING_RATE,
                        &lora_cr, sizeof(lora_cr));

    if (APP_DEBUG) puts("[Info] setup: configuration set with success");

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
    (void)argv;

    uint32_t timeout;
    if (!DUTY_CYCLE || node.node_type == 1) {
        /* Continue listening */
        timeout = 0;
    } else {
        /** DUTY_CYCLE = 1 && node.node_type != 1 
         * Check if the timeout was expired, then you need to sleep
        */
        uint64_t timestamp_now_ms = ztimer64_now(ZTIMER64_MSEC);
        if (argc == 1 || !timestamp_last_timeout_ms) {
            /* Periodic listening or first time resending */
            timeout = LISTENING_TIMEOUT * MS_PER_SEC;
        } else {
            /* Restarting listening after a send */
            uint64_t timestamp_difference_ms = timestamp_now_ms - timestamp_last_timeout_ms;
            if (timestamp_difference_ms <= last_setted_timeout_ms) {
                /* Remaining timeout */
                timeout = LISTENING_TIMEOUT * MS_PER_SEC - timestamp_difference_ms;
            } else {
                /* Continue sleeping */
                return 0;
            }
        }
        /* Save the timeout in order to evaluate it the next time */
        last_setted_timeout_ms = timeout;
        /* Update last timeout */
        timestamp_last_timeout_ms = timestamp_now_ms;
    }

    netdev_t *netdev = &sx127x.netdev;
    // Switch to continuous listen mode
    const netopt_enable_t single = false;

    netdev->driver->set(netdev, NETOPT_SINGLE_RECEIVE, &single, sizeof(single));

    // Set liisten timeout
    if (APP_DEBUG) printf("Listen timeout setted to %" PRIu32 "\n", timeout);
    netdev->driver->set(netdev, NETOPT_RX_TIMEOUT, &timeout, sizeof(timeout));

    // Switch to RX state
    netopt_state_t state = NETOPT_STATE_RX;

    netdev->driver->set(netdev, NETOPT_STATE, &state, sizeof(state));

    puts("Listen mode set\n");

    return 0;
}

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
            if (APP_DEBUG) puts("Data reception started");
            break;

        case NETDEV_EVENT_RX_COMPLETE:
            len = dev->driver->recv(dev, NULL, 0, 0);
            dev->driver->recv(dev, message, len, &packet_info);
            if (APP_DEBUG) printf(
                "{Payload: \"%s\" (%d bytes), RSSI: %i, SNR: %i, TOA: %" PRIu32 "}\n",
                message, (int)len,
                packet_info.rssi, (int)packet_info.snr,
                sx127x_get_time_on_air((const sx127x_t *)dev, len));
            /* Callback for message handling */ 
            int stop_listen = (*callback_on_msg_receive)(node, message);
            if (stop_listen && DUTY_CYCLE && node.node_type != 1) {
                sx127x_set_sleep(&sx127x); 
                puts("Rx power off");
            }
            break;

        case NETDEV_EVENT_RX_TIMEOUT:
            if (DUTY_CYCLE && node.node_type != 1) {
                sx127x_set_sleep(&sx127x);
                puts("Rx timeout");
            }
            break;

        case NETDEV_EVENT_TX_COMPLETE:
            puts("Transmission completed\n");
            /* Callback for tx completed */ 
            (*callback_tx_complete)();
            break;

        case NETDEV_EVENT_CAD_DONE:
            break;

        case NETDEV_EVENT_TX_TIMEOUT:
            break;

        default:
            if (APP_DEBUG) printf("Unexpected netdev event received: %d\n", event);
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

void set_callbacks (clb_msg_received ptr_clb_msg_received, clb_tx_completed ptr_clb_tx_completed)
{
    callback_on_msg_receive = ptr_clb_msg_received;
    callback_tx_complete = ptr_clb_tx_completed;
}

int init_driver_127x(node_t callback_node)
{
    if(APP_DEBUG) puts("[init_driver_127x] starting driver_127x init");; 

    node = callback_node;

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

    if(APP_DEBUG) puts("[init_driver_127x] driver_127x init done");

    return 0;
}