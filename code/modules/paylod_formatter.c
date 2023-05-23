#include "payload_formatter.h"

/**
 * @brief Return the message to send by lora p2p, using the following formatting: <0000 app_id>,<node from>,<node to>,<value>
 * 
*/
char* format_payload (char* value, char* from, char* to) {
    return sprintf("%s,%s,%s,%s", app_id, from, to, value);
}

/**
 * @brief from lora message return "node from" "node to" and "value" only if the app_id is the right one 
*/
payload_t get_values (char message[32]) {
    /* Check app id */
    print("%s", message);
    payload_t payload;
    return payload;
}