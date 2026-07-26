#include "FreeRTOS.h"
#include "task.h"
#include<stdio.h>
#include "pico/stdlib.h"

#define LED_PIN 4

extern "C" {
void vApplicationMallocFailedHook( void )
{
    for( ;; )
    {
    }
}

void vApplicationStackOverflowHook( TaskHandle_t xTask,
                                     char * pcTaskName )
{
    ( void ) xTask;
    ( void ) pcTaskName;

    for( ;; )
    {
    }
}
}


//  here gonna use pvParameters coz FreeRTOS  enforces a strict function standard for all tasks
// p - > pointer , v -> void and c doesnot have oop[ features sos void pointers means that holds memeory address of anything)]
// lets say nothing needed to be passed then can put NULL during xTaskCreate()
void HeartbeatTask(void *pvParameters);
void vCLITask(void *pvParameters);
void vControllerTask(void *pvParameters);
int main()
{
    stdio_init_all();
    // taskfunction, name, stacksize, parameters passed, priority, task handle
    xTaskCreate(HeartbeatTask, "Heartbeating!!", 128, NULL, 1, NULL);
    xTaskCreate(vCLITask, "CLI Task", 256, NULL, 2  , NULL);
    xTaskCreate(vControllerTask, "Controller Task", 256, NULL, 3, NULL);
    vTaskStartScheduler();
}

void HeartbeatTask(void *pvParameters)
{
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    for(;;)
    {
        gpio_put(LED_PIN, 1);
        vTaskDelay(pdMS_TO_TICKS(1000));
        gpio_put(LED_PIN, 0);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void vCLITask(void *pvParameters)
{
    for(;;)
    {
        printf("CLI Task is running...\n");
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}

void vControllerTask(void *pvParameters)
{
    for(;;)
    {
        printf("controller is alive...\n");
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
