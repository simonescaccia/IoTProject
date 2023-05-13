#include <stdio.h>
#include "xtimer.h"

int main(void)
{

    xtimer_sleep(1);
    puts("Hello World!");

    printf("You are running RIOT on a(n) %s board.\n", RIOT_BOARD);
    printf("This board features a(n) %s MCU.\n", RIOT_MCU);

    return 0;
}