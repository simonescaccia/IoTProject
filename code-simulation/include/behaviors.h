#include "params.h"

int source_lora_ttn(node_t node);

int lora_p2p(node_t node);

int message_received_clb (node_t node, char message[32]);

void transmission_complete_clb (void);
