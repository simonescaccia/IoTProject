#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string.h>

#include "thread.h"
#include "fmt.h"

#include "xtimer.h"

#include "shell.h"

#include "drivers_sx127x.h"
#include "semtech-loramac.h"

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
    { "listen",         "Start raw payload listener",           listen_cmd }
};

int main(void)
{

    xtimer_sleep(1);

    puts("Hello World!");

    // init driver 127x
    if(init_driver_127x()){
        return 1;
    }

    // init semtech-loramac
    if(semtech_init()){
        return 1;
    }

    // start shell
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}