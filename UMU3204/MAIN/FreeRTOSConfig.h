

#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

#define portREMOVE_STATIC_QUALIFIER //  сделаем переменные и функции task.c non-static


#define configUSE_PREEMPTION                1
#define configUSE_IDLE_HOOK                        0
#define configUSE_TICK_HOOK                        0
#define configCPU_CLOCK_HZ                        ( ( unsigned long ) 60000000 )        // =12.0MHz xtal multiplied by 5 using the PLL. */
#define configTICK_RATE_HZ                        ( ( portTickType ) 2000 )
#define configMAX_PRIORITIES                ( ( unsigned portBASE_TYPE )7 )

//ifdef _DEBUGPRINT
#define configMINIMAL_STACK_SIZE        ( ( unsigned short ) 150 )
/*
else
define configMINIMAL_STACK_SIZE        ( ( unsigned short ) 100 )
#endif
*/

#define configTOTAL_HEAP_SIZE                ( ( size_t ) 8000 )
#define configMAX_TASK_NAME_LEN                ( 16 )
#define configUSE_TRACE_FACILITY              0
#define configUSE_16_BIT_TICKS                0
#define configIDLE_SHOULD_YIELD                1

// Co-routine definitions.
#define configUSE_CO_ROUTINES                 0
#define configMAX_CO_ROUTINE_PRIORITIES ( 2 )

// Set the following definitions to 1 to include the API function, or zero
//to exclude the API function.

#define INCLUDE_vTaskPrioritySet                  1
#define INCLUDE_uxTaskPriorityGet                1
#define INCLUDE_vTaskDelete                        1
#define INCLUDE_vTaskCleanUpResources    1
#define INCLUDE_vTaskSuspend                     1
#define INCLUDE_vTaskDelayUntil                   1
#define INCLUDE_vTaskDelay                          1
#define INCLUDE_xTaskGetSchedulerState    1

#endif /* FREERTOS_CONFIG_H */
