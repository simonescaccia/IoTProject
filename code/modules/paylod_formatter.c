#include "payload_formatter.h"

/**
 * @brief Return the message to send by lora p2p, using the following formatting: <0000 app_id>,<node from>,<node to>,<value>
 * Maximum lenght of the payload is 31, since the maximum lenght of the lora message is 32
 * 3 comma chars, 2 chars for from and to, 4 chars for the APP_ID, 20 chars for the value
 * 
*/
char* format_payload (char value[21], char from[3], char to[3]) {
    return sprintf("%s,%s,%s,%s", APP_ID, from, to, value);
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