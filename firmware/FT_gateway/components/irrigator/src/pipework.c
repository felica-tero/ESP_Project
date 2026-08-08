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
#include "tasks_common.h"



/**************************
**		DECLARATIONS	 **
**************************/

	/* Variables */
static const char TAG[] = "pipework";

// Array of pipework struct
static pipework_ctrl_t pipework[QTD_DIG_OUTS] = {0};

	/* FreeRTOS Structures */

// Semaphore handle
SemaphoreHandle_t pipework_semaphore = NULL;
// Queue handle used to manipulate the main queue of events
static QueueHandle_t irrigator_monitor_queue_handle;

	/* Static Functions */
static nw_update_valve_state_cb nw_update_valve_state_cb_p = NULL;

static void pipework_sensorValveClose_cb(uint8_t pipeworkId);
static void pipework_freeRTOS_monitor(void * parameter);
static pipework_state_e pipework_askToOpenValve(uint8_t pipeworkId);
static void pipework_setState(uint8_t pipeworkId, uint8_t state);

static void irrigator_freeRTOS_setup(void);
static void irrigator_freeRTOS_endTask(void);

/**************************
**	   APP FUNCTIONS	 **
**************************/

void pipework_setup(nw_update_valve_state_cb nw_update_valve_state_fn)
{
	nw_update_valve_state_cb_p = nw_update_valve_state_fn;

	// setup gpio pins and start valve state as CLOSE
	#define X(id, pipe,	pin, pullUp, pullDown, initial_value)									\ 
			hal_gpio_setupDigOut(&(pipework[id].config), pin, pullUp, pullDown, initial_value);	\
			pipework_setState(id, CLOSE);
		X_MACRO_PIPE_LIST
	#undef X

	// Initialize freeRTOS structures
	pipework_semaphore = xSemaphoreCreateBinary();
	irrigator_freeRTOS_setup();

	xSemaphoreGive(pipework_semaphore);
}


pipework_state_e pipework_getState(uint8_t pipeworkId)
{
	return pipework[pipeworkId].state;
}


static void pipework_setState(uint8_t pipeworkId, uint8_t state)
{
	switch(state)
	{
		case OPEN:
			pipework[pipeworkId].state = OPEN;
			nw_update_valve_state_cb_p(pipeworkId, "open");
			break;

			
		case CLOSE:
			pipework[pipeworkId].state = CLOSE;
			nw_update_valve_state_cb_p(pipeworkId, "close");
			break;

			
		case AWAIT:
			pipework[pipeworkId].state = AWAIT;
			nw_update_valve_state_cb_p(pipeworkId, "await");
			break;

			
		case ERROR:
			pipework[pipeworkId].state = ERROR;
			nw_update_valve_state_cb_p(pipeworkId, "error");
			break;

		default:
			ESP_LOGW(TAG, "Received invalid request");
			break;
	}
}


static pipework_state_e pipework_askToOpenValve(uint8_t pipeworkId)
{
	// get resource semaphore
		ESP_LOGI(TAG, "pediu para abrir esse: pipeId[%d]", pipeworkId);
	if (xSemaphoreTake(pipework_semaphore, portMAX_DELAY) == pdTRUE)
	{
		// open valve
		ESP_LOGI(TAG, "abriu esse: pipeId[%d]", pipeworkId);
		hal_gpio_setOutput(pipework[pipeworkId].config.gpio_pin, HIGH);
		pipework_setState(pipeworkId, OPEN);
	}
	else
	{
		pipework_setState(pipeworkId, AWAIT);
	}

	return pipework[pipeworkId].state;
}


void pipework_closeValve(uint8_t pipeworkId)
{
		ESP_LOGI(TAG, "desligar a valvula deu certo? %s",
			hal_gpio_setOutput(pipework[pipeworkId].config.gpio_pin, LOW) ? "sim" : "nao"
		);
		pipework_sensorValveClose_cb(pipeworkId);
}


static void pipework_sensorValveClose_cb(uint8_t pipeworkId)
{
	// check if it is this valve that is opened
	if(pipework[pipeworkId].state == OPEN)
	{
		// vTaskDelay(pdMS_TO_TICKS(TIME_TO_RELEASE_VALVE_AFTER_CLOSING_MS));
		// devolve semaphore
		xSemaphoreGive(pipework_semaphore);
	}
	
	pipework_setState(pipeworkId, CLOSE);
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
	CREATE_TASK(&pipework_freeRTOS_monitor,
				"pipework_freeRTOS_monitor",
				IRRIGATOR_MONITOR_STACK_SIZE,
				NULL,
				IRRIGATOR_MONITOR_PRIORITY,
#if defined BOARD_ESP32C6
				NULL
#elif defined BOARD_ESP32S3
				NULL,
				IRRIGATOR_MONITOR_CORE
#endif
	);
}


// Sends a message to the queue
BaseType_t pipework_monitor_enqueueOpen(uint8_t pipework_id)
{
	pipework_to_irrigate_queue_message_t msg;
	
	pipework_setState(pipework_id, AWAIT);
	msg.pipework_id = pipework_id;
	return xQueueGenericSend(irrigator_monitor_queue_handle, &msg, portMAX_DELAY, queueSEND_TO_BACK);
}


/**
 * @brief Dut Ctrl monitor task used to track events of the Irrigation Ctrl
 * @param pvParameters parameter which can be passed to the task.
 */
static void pipework_freeRTOS_monitor(void * parameter)
{
	pipework_to_irrigate_queue_message_t msg;
	
	for(;;)
	{
		if(xQueueReceive(irrigator_monitor_queue_handle, &msg, portMAX_DELAY))
		{
			ESP_LOGI(TAG, "quem quer abrir eh esse: pipeId[%d]", msg.pipework_id);
			// request resource semaphore
			while(OPEN != pipework_askToOpenValve(msg.pipework_id))
			{
				vTaskDelay(pdMS_TO_TICKS(TIME_TO_RETRY_OPEN_VALVE_MS));
			}

			ESP_LOGI(TAG, "Abriu pipeId[%d]", msg.pipework_id);
			// vTaskDelay(pdMS_TO_TICKS(10000));
			// ESP_LOGI(TAG, "Aguardou...");
			
			// // open valve
			// pipework_closeValve(msg.pipework_id);
			// ESP_LOGI(TAG, "Fechou pipeId[%d]", msg.pipework_id);

		}
	}
}