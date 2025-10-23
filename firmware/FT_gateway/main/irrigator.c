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
#include "irrigator.h"



/**************************
**		DECLARATIONS	 **
**************************/

	/* Variables */


	/* FreeRTOS Structures */

// Irrigation Ctrl monitor task handle
static TaskHandle_t task_irrigator_monitor = NULL;

// Queue handle used to manipulate the main queue of events
static QueueHandle_t irrigator_monitor_queue_handle;


	/* Static Functions */


/**************************
**	FreeRTOS FUNCTIONS	 **
**************************/

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
			// get resource semaphore

			// open valve

		}
	}
}

/**
 * Setup the FreeRTOS environment for HTTP server.
 */
static void irrigator_freeRTOS_setup(void)
{
	// Create HTTP server monitor task
	xTaskCreatePinnedToCore(&irrigator_freeRTOS_monitor,
							"irrigator_monitor",
							PIPEWORKER_MONITOR_STACK_SIZE,
							NULL,
							PIPEWORKER_MONITOR_PRIORITY,
							&task_http_server_monitor,
							PIPEWORKER_MONITOR_CORE_ID);
	
	// Create the message queue
	irrigator_monitor_queue_handle = xQueueCreate(4, sizeof(pipework_to_irrigate_queue_message_t));
}

static void irrigator_freeRTOS_endTask(void)
{
	if(task_http_server_monitor)
	{
		vTaskDelete(task_http_server_monitor);
		ESP_LOGI(TAG, "irrigator_freeRTOS_endTask: stopping HTTP server monitor");
		task_http_server_monitor = NULL;
	}
}

// Sends a message to the queue
BaseType_t irrigator_monitor_sendMessage(pipeWork_id_e msgId)
{
	pipework_to_irrigate_queue_message_t msg;
	msg.msgId = msgId;
	return xQueueGenericSend(irrigator_monitor_queue_handle, &msg, portMAX_DELAY, queueSEND_TO_BACK );
}



/**************************
**	   APP FUNCTIONS	 **
**************************/

void irrigationDecisor(uint8_t pipework_id, soil_state_e soil_state, weather_state_e weather_state)
{
	// irrigation_state_e next_irrigation_state = AWAIT;
	switch (soil_state)
	{
		case DRY_SOIL:
			switch (weather_state)
			{
				case GOOD:
					// FULL_IRRIGATION
					break;
				case VERY_SUNNY:
					// AWAIT
					break;
				case TO_RAIN:
					// AWAIT
					break;
			}
			break;


		case EXTREME_DRY_SOIL:
			switch (weather_state)
			{
				case GOOD:
					// FULL_IRRIGATION
					break;
				case VERY_SUNNY:
					// LITTLE_IRRIGATION
					break;
				case TO_RAIN:
					// AWAIT
					break;
			}
			break;
		

		case AWAITING_TO_IRRIGATE:
			// AWAIT
			break;
		

		case IRRIGATING:
			switch (weather_state)
			{
				case GOOD:
					// AWAIT
					break;
				case VERY_SUNNY:
					// STOP
					break;
				case TO_RAIN:
					// STOP
					break;
			}
			break;
		

		case MOIST_SOIL:
			// STOP
			break;


		default:
			break;
	}
}