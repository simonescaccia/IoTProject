#include "params.h"

typedef void (*callback)(node_t node, char message[32]);

int lora_setup_cmd(int argc, char **argv);
int send_cmd(int argc, char **argv);
int listen_cmd(int argc, char **argv);
void *_recv_thread(void *arg);
int init_driver_127x(node_t node);
void set_callback(callback ptr_reg_callback);