#include <stdio.h>
#include <string.h>

#include "shell.h"
#include "xtimer.h"

int setup(int argc, char **argv)
{
    printf("arguments received %d\n", argc);
    for(int arg=0; arg<argc; arg++){
        printf("argument #%d: %s\n", arg+1,argv[arg]);
    }

    return 0;
}

static const shell_command_t commands[] = {
    { "setup", "setup the infrastructure", setup }
};

int main(void)
{

    puts("Hello World!");

    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}