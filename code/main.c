#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string.h>

#include "thread.h"
#include "fmt.h"

#include "xtimer.h"

#include "drivers_sx127x.h"
#include "lorawan.h"

// debug
#define DEBUG 1

int setup_infra(int argc, char **argv)
{
    printf("arguments received %d\n", argc);
    for(int arg=0; arg<argc; arg++){
        printf("argument #%d: %s\n", arg+1,argv[arg]);
    }

    return 0;
}

static const shell_command_t commands[] = {
    { "setup_infra",    "Setup the infrastructure",             setup_infra },
    { "setup",          "Initialize LoRa modulation settings",  lora_setup_cmd },
    { "send",           "Send raw payload string",              send_cmd },
    { "listen",         "Start raw payload listener",           listen_cmd },
};

int main(void)
{

    xtimer_sleep(1);

    puts("Hello World!");

    if(send_messages_to_ttn(NULL)){
        return 1;
    }

    // init driver 127x
    //if(init_driver_127x(NULL)){
    //    return 1;
    //}

    // start shell
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}