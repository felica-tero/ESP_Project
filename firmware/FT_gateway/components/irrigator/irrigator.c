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
#include "irrigator.h"



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
	pipeWorker_setup();
	irrigator_freeRTOS_setup();
	
	ESP_LOGI(TAG, "tenta abrir o 0: [%d]", (uint8_t)irrigator_monitor_sendMessage(0));
	ESP_LOGI(TAG, "tenta abrir o 1: [%d]", (uint8_t)irrigator_monitor_sendMessage(1));
	ESP_LOGI(TAG, "tenta abrir o 2: [%d]", (uint8_t)irrigator_monitor_sendMessage(2));
	ESP_LOGI(TAG, "tenta abrir o 3: [%d]", (uint8_t)irrigator_monitor_sendMessage(3));
}

/**
 * Setup the FreeRTOS environment for Irrigator App
 */
static void irrigator_freeRTOS_setup(void)
{
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
	
	// Create the message queue
	irrigator_monitor_queue_handle = xQueueCreate(QTD_DIG_OUTS, sizeof(pipework_to_irrigate_queue_message_t));
}

// Sends a message to the queue
BaseType_t irrigator_monitor_sendMessage(pipeWork_id_e pipeWork_id)
{
	pipework_to_irrigate_queue_message_t msg;
	msg.msgId = pipeWork_id;
	return xQueueGenericSend(irrigator_monitor_queue_handle, &msg, portMAX_DELAY, queueSEND_TO_BACK );
}



/**************************
**	   APP FUNCTIONS	 **
**************************/

void irrigationDecisor(uint8_t pipework_id)
{
	static soil_state_e		soil_state[QTD_DIG_OUTS] = {0};
	static weather_state_e	weather_state[QTD_DIG_OUTS] = {0};

	switch ((uint8_t) soil_state)
	{
		case PONTO_DE_MURCHA_PERMANENTE:
			switch ((uint8_t) weather_state)
			{
				case GOOD:
					// IRRIGA COMPLETAMENTE
					irrigation_sm(pipework_id, FULL_IRRIGATION);
				break;

				case VERY_SUNNY:
					// AWAIT
				break;

				case TO_RAIN:
					// AWAIT
				break;
			}
		break;


		case SOLO_SECO:
			switch ((uint8_t) weather_state)
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


		case AGUARDANDO:
			// AWAIT
		break;


		case IRRIGANDO:
			// AWAIT
		break;


		case SOLO_UMIDO:
			// AWAIT
		break;


		case CAPACIDADE_DE_CAMPO:
			// STOP
			// pipeWorker_closeValve(id);
		break;


		default:
			break;
	}
}

static void irrigation_sm(uint8_t pipeworkId, irrigation_state_e irrigation_state)
{
	switch (irrigation_state)
	{
	case FULL_IRRIGATION:
		if (OPEN == pipeWorker_askToOpenValve(pipeworkId))
		{
			// FULL_IRRIGATION
			// aguarda ate o solo ficar umido
			// onde a valvula eh fechada
			uart_UmidtSensor_setDesiredLevel(pipeworkId, SOLO_UMIDO);
		}
	break;
	

	case LITTLE_IRRIGATION:
			// FULL_IRRIGATION
			// aguarda ate o solo ficar menos seco
			// onde a valvula eh fechada
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
			ESP_LOGI(TAG, "quem quer abrir eh esse: pipeId[%d]", msg.msgId);
			// request resource semaphore
			while(OPEN != pipeWorker_askToOpenValve(msg.msgId));

			ESP_LOGI(TAG, "Abriu pipeId[%d]", msg.msgId);
			vTaskDelay(pdMS_TO_TICKS(2000));
			ESP_LOGI(TAG, "Aguardou...");
			
			// open valve
			pipeWorker_closeValve(msg.msgId);
			ESP_LOGI(TAG, "Fechou pipeId[%d]", msg.msgId);

		}
	}
}