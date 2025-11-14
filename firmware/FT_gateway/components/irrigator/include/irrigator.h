/**
 * @file irrigator.h
 * @author Isabella Vecchi Ferreira
 * @brief
 * @details
 * @version 0.1
 * @date 2025-10-18
 * 
 */

#ifndef __IRRIGATOR_PERSONAL_LIB__
#define __IRRIGATOR_PERSONAL_LIB__


/**************************
**		  INCLUDES	 	 **
**************************/

// C libraries
#include <stdint.h>

// ESP libraries
#include "esp_log.h"

// Personal libraries
#include "pipeWorker.h"
#include "tasks_common.h"
#include "uart_sensorUmidity.h"



/**************************
**		DEFINITIONS		 **
**************************/
/**
 * @brief Creating Irrigator State list with X_MACRO 
 * 
 */
#define X_MACRO_IRRIGATION_STATE_LIST	\
	X(0, FULL_IRRIGATION				) \
	X(1, LITTLE_IRRIGATION				) 


/**************************
**		STRUCTURES		 **
**************************/

/**
 * Enum for the IRRIGATION state
 */
typedef enum irrigation_state
{
	#define X(ID, ENUM) ENUM=ID, 
		X_MACRO_IRRIGATION_STATE_LIST
	#undef X
} irrigation_state_e;

/**
 * ENUM for the WEATHER STATE
 */
typedef enum weather_state
{
	GOOD=0,
	VERY_SUNNY,
	TO_RAIN,
} weather_state_e;

/**
 * Structure for the PIPE to irrigate QUEUE
 */
typedef struct pipework_to_irrigate_queue_message_s
{
	pipeWork_id_e msgId;
} pipework_to_irrigate_queue_message_t;


/**************************
**		FUNCTIONS		 **
**************************/
void irrigator_setup(void);
BaseType_t irrigator_monitor_sendMessage(pipeWork_id_e pipeWork_id);

#endif //__IRRIGATOR_PERSONAL_LIB__