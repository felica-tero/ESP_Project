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
static TaskHandle_t task_pipeWorker_monitor = NULL;

// Queue handle used to manipulate the main queue of events
static QueueHandle_t pipeWorker_monitor_queue_handle;


	/* Static Functions */


/**************************
**	FreeRTOS FUNCTIONS	 **
**************************/

/**
 * @brief Dut Ctrl monitor task used to track events of the Irrigation Ctrl
 * @param pvParameters parameter which can be passed to the task.
 */
static void pipeWorker_freeRTOS_monitor(void * parameter)
{
	http_server_queue_message_t msg;
	
	for(;;)
	{
		if(xQueueReceive(pipeWorker_monitor_queue_handle, &msg, portMAX_DELAY))
		{
			// get resource semaphore

			// open valve

		}
	}
}

/**
 * Setup the FreeRTOS environment for HTTP server.
 */
static void pipeWorker_freeRTOS_setup(void)
{
	// Create HTTP server monitor task
	xTaskCreatePinnedToCore(&pipeWorker_freeRTOS_monitor,
							"pipeWorker_monitor",
							PIPEWORKER_MONITOR_STACK_SIZE,
							NULL,
							PIPEWORKER_MONITOR_PRIORITY,
							&task_http_server_monitor,
							PIPEWORKER_MONITOR_CORE_ID);
	
	// Create the message queue
	pipeWorker_monitor_queue_handle = xQueueCreate(4, sizeof(http_server_queue_message_t));
}

static void pipeWorker_freeRTOS_endTask(void)
{
	if(task_http_server_monitor)
	{
		vTaskDelete(task_http_server_monitor);
		ESP_LOGI(TAG, "pipeWorker_freeRTOS_endTask: stopping HTTP server monitor");
		task_http_server_monitor = NULL;
	}
}

// Sends a message to the queue
BaseType_t pipeWorker_monitor_sendMessage(pipeWork_id_e msgId)
{
	pipework_to_irrigate_queue_message_t msg;
	msg.msgId = msgId;
	return xQueueGenericSend(pipeWorker_monitor_queue_handle, &msg, portMAX_DELAY, queueSEND_TO_BACK );
}



/**************************
**	   APP FUNCTIONS	 **
**************************/