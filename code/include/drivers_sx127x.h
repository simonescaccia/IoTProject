extern int lora_setup_cmd(int argc, char **argv);
extern int send_cmd(int argc, char **argv);
extern int listen_cmd(int argc, char **argv);
extern void *_recv_thread(void *arg);
extern int init_driver_127x(void);