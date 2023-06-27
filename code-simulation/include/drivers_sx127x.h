#include "params.h"

typedef int (*clb_msg_received)(node_t node, char message[32]);
typedef void (*clb_tx_completed)(void);

int lora_setup_cmd(int argc, char **argv);
int send_cmd(int argc, char **argv);
int listen_cmd(int argc, char **argv);
void *_recv_thread(void *arg);
int init_driver_127x(node_t node);
void set_callbacks(clb_msg_received ptr_clb_msg_received, clb_tx_completed ptr_clb_tx_completed);