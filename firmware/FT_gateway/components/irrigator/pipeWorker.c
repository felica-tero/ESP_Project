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

// Array of pipework struct
static pipeWorker_ctrl_t pipeworker[] = {0};

	/* FreeRTOS Structures */

// Semaphore handle
SemaphoreHandle_t pipework_semaphore = NULL;

	/* Static Functions */


/**************************
**	   APP FUNCTIONS	 **
**************************/

void pipeWorker_setup(void)
{
	// setup gpio pins and start valve state as CLOSE
	#define X(id, pipe,	pin, pullUp, pullDown, initial_value)									\ 
			hal_gpio_setupDigOut(&(pipeworker[id].config), pin, pullUp, pullDown, initial_value);	\
			pipeworker[id].state = CLOSE;
		X_MACRO_PIPE_LIST
	#undef X

	// Create semaphore
	pipework_semaphore = xSemaphoreCreateBinary();
}


pipeWork_state_e pipeWorker_getState(uint8_t pipeworkId)
{
	return pipeworker[pipeworkId].state;
}


pipeWork_state_e pipeWorker_askToOpenValve(uint8_t pipeworkId)
{
	// get resource semaphore
	if (xSemaphoreTake(pipework_semaphore, portMAX_DELAY) == pdTRUE)
	{
		// open valve
		hal_gpio_setOutput(pipeworker[pipeworkId].config.gpio_pin, HIGH);
		pipeworker[pipeworkId].state = OPEN;
	}
	return pipeworker[pipeworkId].state;
}


void pipeWorker_closeValve(uint8_t pipeworkId)
{
	// check if it is this valve that is opened
	if(pipeworker[pipeworkId].state == OPEN)
	{
		// devolve semaphore
		hal_gpio_setOutput(pipeworker[pipeworkId].config.gpio_pin, LOW);
		pipeworker[pipeworkId].state = CLOSE;
		xSemaphoreGive(pipework_semaphore);
	}
}