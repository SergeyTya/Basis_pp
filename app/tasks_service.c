/*
 * Tasks.c
 *
 *  Created on: 22 РјР°СЏ 2018 Рі.
 *      Author: Sergey Tyagushev
 */

#include "stdint.h"
#include "stdlib.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "croutine.h"

uint16_t MCULOAD = 0;

/*-----------------------RTOS SERVICE FUNC-----------------------------------*/
void vApplicationIdleHook(void)
{

	// static int counter;
	// static int max_count;
	// static portTickType LastTick;
	// counter++;
	//     if(xTaskGetTickCount() - LastTick>1000)
	//     {
	//         LastTick = xTaskGetTickCount();
	//         if(counter > max_count) max_count = counter;
	//         MCULOAD = 100-(counter*100/max_count);
	//         counter = 0;
	//     }
	for (;;) {

		vCoRoutineSchedule();
	}
}
void vApplicationStackOverflowHook(TaskHandle_t pxTask, char *pcTaskName)
{
	(void)pcTaskName;
	(void)pxTask;

	taskDISABLE_INTERRUPTS();
	vMCUReset();
	for (;;)
		;
}

void vApplicationMallocFailedHook(void)
{
	taskDISABLE_INTERRUPTS();
	vMCUReset();
	for (;;)
		;
}
