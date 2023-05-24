#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <debug.h>

#include "payload_formatter.h"

const char* APP_ID = "0000";

/* driver sx127x message length */
const int MESSAGE_LENGTH = 32;

/**
 * @brief Return the message to send by lora p2p, using the following formatting: <0000 app_id>,<node from>,<node to>,<value>
 * Maximum lenght of the payload is 31, since the maximum lenght of the lora message is 32
 * 3 comma chars, 2 chars for from and to, 4 chars for the APP_ID, 20 chars for the value
 * 
*/
char* format_payload (char value[21], char from[3], char to[3]) {
    char* payload = malloc(sizeof(char) * MESSAGE_LENGTH); 
    snprintf(payload, MESSAGE_LENGTH, "%s,%s,%s,%s", APP_ID, from, to, value);
    return payload;
}

/**
 * @brief from lora message return "node from" "node to" and "value" only if the app_id is the right one 
*/
payload_t* get_values (char message[32]) {
    printf("Message: %s, length: %d\n", message, strlen(message));
    /* Check app id, at least 3 comma chars, 2 chars for from and to, 4 chars for the APP_ID */
    if (strlen(message) > 11 && strncmp(message, APP_ID, 4)) 
    {
        /*  Parse the string */
        payload_t *payload = malloc(sizeof(payload_t));
        memcpy(payload->from, &message[5], 2*sizeof(char));
        memcpy(payload->to, &message[8], 2*sizeof(char));
        memcpy(payload->value, &message[11], (strlen(message)-11)*sizeof(char));

        DEBUG("from: %s\n",payload->from);
        DEBUG("to: %s\n", payload->to);
        DEBUG("value: %s\n", payload->value);

        return payload;
    } else
    {
        return NULL;
    }
}