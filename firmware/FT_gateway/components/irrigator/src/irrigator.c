/**
 * @file irrigator.c
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
// C libraries
#include <stdint.h>

// ESP libraries
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

// Personal libraries
#include "irrigator.h"
#include "uart_sensorUmidity.h"
#include "projectConfig.h"
#include "tasks_common.h"



/**************************
**		DECLARATIONS	 **
**************************/

	/* Variables */
	const char TAG[] = "irrigator";
	const uint8_t qtd_digs_out = 0;

	/* FreeRTOS Structures */

// Queue handle used to manipulate the main queue of events
static QueueHandle_t irrigator_monitor_queue_handle;


	/* Static Functions */
static void irrigator_freeRTOS_setup(void);
static void irrigator_freeRTOS_endTask(void);
static void irrigation_sm(uint8_t pipeworkId, irrigation_state_e irrigation_state);
static void irrigator_freeRTOS_monitor(void * parameter);


/**************************
**	FreeRTOS FUNCTIONS	 **
**************************/

void irrigator_setup(void)
{
	ESP_LOGI(TAG, "pipeworker_setup");
	pipeworker_setup();
	ESP_LOGI(TAG, "irrigator_freeRTOS_setup");
	irrigator_freeRTOS_setup();
}

/**
 * Setup the FreeRTOS environment for Irrigator App
 */
static void irrigator_freeRTOS_setup(void)
{
	ESP_LOGI(TAG, "xQueueCreate");
	
	// Create the message queue
	irrigator_monitor_queue_handle = xQueueCreate(QTD_DIG_OUTS, sizeof(pipework_to_irrigate_queue_message_t));
	
	ESP_LOGI(TAG, "CREATE_TASK");
	// Create HTTP server monitor task
	CREATE_TASK(&irrigator_freeRTOS_monitor,
				"irrigator_monitor",
				IRRIGATOR_MONITOR_STACK_SIZE,
				NULL,
				IRRIGATOR_MONITOR_PRIORITY,
#if defined BOARD_ESP32C6
				NULL);
#elif defined BOARD_ESP32S3
				NULL,
				IRRIGATOR_MONITOR_CORE);
#endif
}

// Sends a message to the queue
BaseType_t irrigator_monitor_enqueueOpen(uint8_t pipework_id)
{
	pipework_to_irrigate_queue_message_t msg;
	msg.pipework_id = pipework_id;
	return xQueueGenericSend(irrigator_monitor_queue_handle, &msg, portMAX_DELAY, queueSEND_TO_BACK );
}



/**************************
**	   APP FUNCTIONS	 **
**************************/

void irrigationDecisor_fromSensor(uint8_t pipework_id)
{
	weather_state_e	weather_state = GOOD;
	uint8_t	soil_state = uart_UmidtSensor_geState(pipework_id);

	switch (soil_state)
	{
		case PONTO_DE_MURCHA_PERMANENTE:
			switch (weather_state)
			{
				case GOOD:
					// IRRIGA COMPLETAMENTE
					irrigation_sm(pipework_id, FULL_IRRIGATION);
				break;

				case VERY_SUNNY:
					// IRRIGA SOH PARA AGUENTAR ESPERAR UM POUCO MAIS
					irrigation_sm(pipework_id, LITTLE_IRRIGATION);
				break;

				case TO_RAIN:
					// IRRIGA SOH PARA AGUENTAR ESPERAR UM POUCO MAIS
					irrigation_sm(pipework_id, LITTLE_IRRIGATION);
				break;
			}
		break;


		case SOLO_SECO:
			switch (weather_state)
			{
				case GOOD:
					// IRRIGA COMPLETAMENTE
					irrigation_sm(pipework_id, FULL_IRRIGATION);
					break;
				case VERY_SUNNY:
					// IRRIGA SOH PARA AGUENTAR ESPERAR UM POUCO MAIS
					irrigation_sm(pipework_id, LITTLE_IRRIGATION);
					break;
				case TO_RAIN:
					// AWAIT
					break;
			}
		break;		


		case SOLO_UMIDO:
			// STOP
			pipeworker_closeValve(pipework_id);
		break;


		case CAPACIDADE_DE_CAMPO:
			// STOP
			pipeworker_closeValve(pipework_id);
		break;


		default:
			break;
	}
}


void irrigationDecisor_client(uint8_t pipework_id, uint8_t valve_desired_state)
{
	weather_state_e	weather_state = GOOD;
	uint8_t	soil_state = uart_UmidtSensor_geState(pipework_id);

	ESP_LOGI(TAG, "irrigationDecisor_client, PIPE %d: %d", pipework_id, valve_desired_state);

	if (OPEN == valve_desired_state)
	{
		switch (soil_state)
		{
			case PONTO_DE_MURCHA_PERMANENTE:
			case SOLO_SECO:
			case SOLO_MEIO_TERMO:
			case SOLO_UMIDO:
				// OPEN
				irrigation_sm(pipework_id, FULL_IRRIGATION);
				break;
			

			case CAPACIDADE_DE_CAMPO:
				// STOP
				pipeworker_closeValve(pipework_id);
				break;
			
			
			default:
				break;
		}
	}
	else if (CLOSE == valve_desired_state)
	{
		pipeworker_closeValve(pipework_id);
	}
}


static void irrigation_sm(uint8_t pipework_id, irrigation_state_e irrigation_state)
{
	ESP_LOGI(TAG, "irrigation_sm, PIPE %d: %d", pipework_id, irrigation_state);

	switch (irrigation_state)
	{
		case FULL_IRRIGATION:
			uart_UmidtSensor_setDesiredLevel(pipework_id, CAPACIDADE_DE_CAMPO);
			if (AWAIT != pipeworker_getState(pipework_id)
			&&	OPEN  != pipeworker_getState(pipework_id))
				irrigator_monitor_enqueueOpen(pipework_id);
		break;
		

		case LITTLE_IRRIGATION:
			uart_UmidtSensor_setDesiredLevel(pipework_id, SOLO_MEIO_TERMO);
			if (AWAIT != pipeworker_getState(pipework_id)
			&&	OPEN  != pipeworker_getState(pipework_id))
				irrigator_monitor_enqueueOpen(pipework_id);
		break;

		
		default:
			return;
	}
}

/**
 * @brief Dut Ctrl monitor task used to track events of the Irrigation Ctrl
 * @param pvParameters parameter which can be passed to the task.
 */
static void irrigator_freeRTOS_monitor(void * parameter)
{
	pipework_to_irrigate_queue_message_t msg;
	
	for(;;)
	{
		if(xQueueReceive(irrigator_monitor_queue_handle, &msg, portMAX_DELAY))
		{
			ESP_LOGI(TAG, "quem quer abrir eh esse: pipeId[%d]", msg.pipework_id);
			// request resource semaphore
			while(OPEN != pipeworker_askToOpenValve(msg.pipework_id))
			{
				vTaskDelay(pdMS_TO_TICKS(TIME_TO_RETRY_OPEN_VALVE_MS));
			}

			ESP_LOGI(TAG, "Abriu pipeId[%d]", msg.pipework_id);
			// vTaskDelay(pdMS_TO_TICKS(10000));
			// ESP_LOGI(TAG, "Aguardou...");
			
			// // open valve
			// pipeworker_closeValve(msg.pipework_id);
			// ESP_LOGI(TAG, "Fechou pipeId[%d]", msg.pipework_id);

		}
	}
}