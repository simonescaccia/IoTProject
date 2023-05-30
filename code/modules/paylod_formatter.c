#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <app_debug.h>

#include "payload_formatter.h"

const char* APP_ID = "0000";

/* driver sx127x message length */
const int MESSAGE_LENGTH = 32;

/**
 * @brief Return the message to send by lora p2p, using the following formatting: <0000 app_id>,<node from>,<node to>,<L for leakage or V for value>,<value>
 * Maximum lenght of the payload is 31, since the maximum lenght of the lora message is 32
 * 5 comma chars, 2 chars for from and to, 4 chars for the APP_ID, 1 char for leakage info, 13 chars for the value, 4 chars for the logic time
 * 
*/
char* format_payload (char value[21], char from[3], char to[3], char leak[2], char logic_time[55]) {
    char* payload = malloc(sizeof(char) * MESSAGE_LENGTH); 
    snprintf(payload, MESSAGE_LENGTH, "%s,%s,%s,%s,%s,%s", APP_ID, from, to, leak, value, logic_time);
    return payload;
}

/**
 * @brief from lora message return "node from" "node to" and "value" only if the app_id is the right one 
*/
payload_t* get_values (char message[32]) {
    if(APP_DEBUG) printf("Message: %s, length: %d\n", message, strlen(message));
    
    /* Check app id, at least 3 comma chars, 2 chars for from and to, 4 chars for the APP_ID */
    if (strlen(message) > 11 && strlen(message) < 32 && strncmp(message, APP_ID, 4) == 0) 
    {
        /* strtkn needs an array */
        char msg[32];
        sprintf(msg, "%s", message);
        
        /*  Parse the string */
        payload_t *payload = (payload_t*)malloc(sizeof(payload_t));
        char* token = strtok(msg, ",");
        int i = 0;
        while (token != NULL) {
            switch (i) {
                case 1:
                    payload->from = (char*)malloc(sizeof(char)*(strlen(token)+1));
                    strncpy(payload->from, token, strlen(token) + 1); 
                    break;
                case 2:
                    payload->to = (char*)malloc(sizeof(char)*(strlen(token)+1));
                    strncpy(payload->to, token, strlen(token) + 1); 
                    break;
                case 3:
                    payload->is_leak = (char*)malloc(sizeof(char)*(strlen(token)+1));
                    strncpy(payload->is_leak, token, strlen(token) + 1); 
                    break;
                case 4:
                    payload->value = (char*)malloc(sizeof(char)*(strlen(token)+1));
                    strncpy(payload->value, token, strlen(token) + 1); 
                    break;
                case 5:
                    payload->logic_time = (char*)malloc(sizeof(char)*(strlen(token)+1));
                    strncpy(payload->logic_time, token, strlen(token) + 1); 
                    break;
            }
            token = strtok(NULL, ",");
            i++;
        }

        if (APP_DEBUG) {
            printf("from: %s\n",payload->from);
            printf("to: %s\n", payload->to);
            printf("value: %s\n", payload->value);
            printf("is_leak: %s\n", payload->is_leak);
        }

        return payload;
    } else
    {
        return NULL;
    }
}

void free_payload (payload_t* payload) {
    free(payload->from);
    free(payload->is_leak);
    free(payload->logic_time);
    free(payload->to);
    free(payload->value);
    free(payload);
}