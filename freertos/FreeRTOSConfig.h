/*
  it is kernel settings file.. so generally freeRTOS ships as generic source code
  this header tells how to behave on the chip
*/

#ifndef FREERTOS_CONFIG
#define FREERTOS_CONFIG

#define configCPU_CLOCK_HZ        133000000UL
#define configTICK_RATE_HZ          1000UL
// how much the timer should go befoer getting back to 0, SO HERE 32 bits is used 
#define configUSE_16_BIT_TICKS    0
// enabling preemption here means that high priority taska can be executred ny stopping the low priority ones
#define configUSE_PREEMPTION       1
#define configUSE_TIME_SLICING    1
#define configMAX_PRIORITIES       5
#define configMINIMAL_STACK_SIZE   128
#define configMAX_TASK_NAME_LEN     16
#define configIDLE_SHOULD_YIELD       1

/// memory
#define configSUPPORT_DYNAMIC_ALLOCATION        1
#define configSUPPORT_STATIC_ALLOCATION         0
#define configTOTAL_HEAP_SIZE       (128* 1024)
// hooks are created by freeRTOS that creates an idle task of priority 0 when no other task is getting executed
#define configUSE_IDLE_HOOK      0
// if enabled triggers ultra fast background checks and software watchdog timers
#define configUSE_TICK_HOOK                     0
// if set as 2 means thorough checking happens  so 
/*
freeRTOS fills the top 16 bytes of every task stack with a known pattern  when taks is created, so if context switching happens freeRTOS checks those 16 bytes were overwritten or modified, if 
task is too much used the stack and crosses the threshold , freeRTOS immediately calls 
vApplicationsStackOverflowHook() callback!
*/
#define configCHECK_FOR_STACK_OVERFLOW          2
#define configUSE_MALLOC_FAILED_HOOK            1

/* --- Sync primitives (needed from Milestone 2 onward) ------------ */
#define configUSE_MUTEXES                       1
#define configUSE_RECURSIVE_MUTEXES             1
#define configUSE_COUNTING_SEMAPHORES           1
#define configQUEUE_REGISTRY_SIZE               10
#define configUSE_TASK_NOTIFICATIONS            1
/* --- Debug / stats (report section 5.1) --------------------------- */
#define configUSE_TRACE_FACILITY                1
#define configUSE_STATS_FORMATTING_FUNCTIONS    1
#define configGENERATE_RUN_TIME_STATS           0   /* enable later at M6 */
/* --- Software timers --------------------------------------------- */
#define configUSE_TIMERS                        1
#define configTIMER_TASK_PRIORITY               (configMAX_PRIORITIES - 1)
#define configTIMER_QUEUE_LENGTH                10
#define configTIMER_TASK_STACK_DEPTH            1024
/* --- API functions to include (strip = smaller binary) ------------ */
#define INCLUDE_vTaskPrioritySet                1
#define INCLUDE_uxTaskPriorityGet               1
#define INCLUDE_vTaskDelete                     1
#define INCLUDE_vTaskSuspend                    1
#define INCLUDE_vTaskDelayUntil                 1
#define INCLUDE_vTaskDelay                      1
#define INCLUDE_xTaskGetSchedulerState          1
#define INCLUDE_xTaskGetCurrentTaskHandle       1
#define INCLUDE_uxTaskGetStackHighWaterMark     1
#define INCLUDE_xTimerPendFunctionCall          1
/* --- RP2040 port specific (SMP: we use core 0 only) --------------- */
#define configNUMBER_OF_CORES                   1
#define configSUPPORT_PICO_SYNC_INTEROP         1
#define configSUPPORT_PICO_TIME_INTEROP         1
#endif /* FREERTOS_CONFIG_H */