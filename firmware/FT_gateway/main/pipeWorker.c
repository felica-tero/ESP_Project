/**
 * @file dateTimeNTP.c
 * @author Isabella Vecchi Ferreira
 * @brief
 * @details
 * @version 0.1
 * @date 2025-10-18
 * 
 */


/**************************
**		  INCLUDES	 	 **
**************************/
#include "pipeWorker.h"



/**************************
**		DECLARATIONS	 **
**************************/

	/* Variables */

// Array of pipework states, initiated in state CLOSE
#define X(id, pipe,	pin, output_type, initial_value) CLOSE, 
pipeWork_state pipeworker_arrayOfStates[] = {X_MACRO_VALVE_STATE_LIST}
#undef X

	/* FreeRTOS Structures */

// Semaphore handle
SemaphoreHandle_t pipework_semaphore = NULL;

	/* Static Functions */


/**************************
**	   APP FUNCTIONS	 **
**************************/

void pipeWorker_setup(void)
{
	// hal_gpio_init(pins);

	// Create semaphore
	pipework_semaphore = xSemaphoreCreateBinary();
}


pipeWork_state_e pipeWorker_openValveQueue(uint8_t pipeworkId)
{
	// get resource semaphore
	if (xSemaphoreTake(pipework_semaphore, portMAX_DELAY) == pdTRUE)
	{
		// open valve
		// hal_gpio_set(pin, TRUE);
		pipeworker_arrayOfStates[msg.msgId] = OPEN;
	}
	return pipeworker_arrayOfStates[msg.msgId];
}


pipeWork_state_e pipeWorker_closeValve(uint8_t pipeworkId)
{
	// check if it is this valve that is opened
	if(pipeworker_arrayOfStates[msg.msgId] == OPEN)
	{
		// devolve semaphore
		pipeworker_arrayOfStates[msg.msgId] = CLOSE;
		xSemaphoreGive(pipeworker_arrayOfStates);
	}
}