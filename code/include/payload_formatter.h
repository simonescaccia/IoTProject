typedef struct {
    char* from;             /* source node of the message */
    char* to;               /* destination node of the message */
    char* is_leak;          /* 'L' if 'value' contains the amount of leakage, 'V' if 'value' contains the water flow sampled */
    char* value;            /* water flow sampled or difference between water flows */
    char* logic_time;       /* Simulation time of the sample */
} payload_t;

char* format_payload (char value[21], char from[3], char to[3], char leak[2], char logic_time[55]);

payload_t* get_values (char message[32]);