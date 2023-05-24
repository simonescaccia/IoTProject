#include "params.h"

int source_lora_ttn(node_t node);

int source_lora_p2p(node_t node);

int fork_lora_p2p(node_t node);

int branch_lora_p2p(node_t node);

void message_received_clb (node_t node, char message[32]);
