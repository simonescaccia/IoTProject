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

extern int lora_setup_cmd(int argc, char **argv);
extern int send_cmd(int argc, char **argv);
extern int listen_cmd(int argc, char **argv);
extern void *_recv_thread(void *arg);
extern int init_driver_127x(void *arg);