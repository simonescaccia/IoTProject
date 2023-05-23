typedef struct {
    char* from;
    char* to;
    char* value;
} payload_t;

char* APP_ID = "0000";

char* format_payload (char value[21], char from[3], char to[3]);
payload_t get_values (char message[32]);