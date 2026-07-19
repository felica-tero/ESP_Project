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
#include "pipework.h"



/**************************
**		DECLARATIONS	 **
**************************/

	/* Variables */
static const char TAG[] = "pipework";

// Array of pipework struct
static pipeworker_ctrl_t pipeworker[QTD_DIG_OUTS] = {0};

	/* FreeRTOS Structures */

// Semaphore handle
SemaphoreHandle_t pipework_semaphore = NULL;

	/* Static Functions */


/**************************
**	   APP FUNCTIONS	 **
**************************/

void pipeworker_setup(void)
{
	// setup gpio pins and start valve state as CLOSE
	#define X(id, pipe,	pin, pullUp, pullDown, initial_value)									\ 
			hal_gpio_setupDigOut(&(pipeworker[id].config), pin, pullUp, pullDown, initial_value);	\
			pipeworker[id].state = CLOSE;
		X_MACRO_PIPE_LIST
	#undef X

	// Create semaphore
	pipework_semaphore = xSemaphoreCreateBinary();
	xSemaphoreGive(pipework_semaphore);
}


pipework_state_e pipeworker_getState(uint8_t pipeworkId)
{
	return pipeworker[pipeworkId].state;
}


pipework_state_e pipeworker_askToOpenValve(uint8_t pipeworkId)
{
	// get resource semaphore
		ESP_LOGI(TAG, "pediu para abrir esse: pipeId[%d]", pipeworkId);
	if (xSemaphoreTake(pipework_semaphore, TIME_TO_WAIT_TO_OPEN_VALVE_MS) == pdTRUE)
	{
		// open valve
		ESP_LOGI(TAG, "abriu esse: pipeId[%d]", pipeworkId);
		hal_gpio_setOutput(pipeworker[pipeworkId].config.gpio_pin, HIGH);
		pipeworker[pipeworkId].state = OPEN;
	}
	else
	{
		pipeworker[pipeworkId].state = AWAIT;
	}

	return pipeworker[pipeworkId].state;
}


void pipeworker_closeValve(uint8_t pipeworkId)
{
	// check if it is this valve that is opened
	// if(pipeworker[pipeworkId].state == OPEN)
	// {
		// devolve semaphore
		ESP_LOGI(TAG, "desligar a valvula deu certo? %s", \
		hal_gpio_setOutput(pipeworker[pipeworkId].config.gpio_pin, LOW) ? "sim" : "nao");
		pipeworker[pipeworkId].state = CLOSE;
		vTaskDelay(pdMS_TO_TICKS(TIME_TO_RELEASE_VALVE_AFTER_CLOSING_MS));
		xSemaphoreGive(pipework_semaphore);
	// }
}