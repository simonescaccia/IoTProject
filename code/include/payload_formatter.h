typedef struct {
    char* from;
    char* to;
    char* value;
} payload_t;

char* format_payload (char value[21], char from[3], char to[3]);

payload_t get_values (char message[32]);