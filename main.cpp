#include <stdio.h>
#include "pico/stdlib.h"


int main()
{
    stdio_init_all();
    uint32_t last = to_ms_since_boot(get_absolute_time());

    while (true) {
        
        uint32_t now = to_ms_since_boot(get_absolute_time());
        printf("Current time: %lu ms\n", (unsigned long)now);
        printf("\n");
        if(now - last >= 1000) {
            last = now;
            printf("Hi Rudraaa Hema\n");
            fflush(stdout);
        }
    }
}
