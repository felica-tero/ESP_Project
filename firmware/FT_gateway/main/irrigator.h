#ifndef __IRRIGATOR_PERSONAL_LIB__
#define __IRRIGATOR_PERSONAL_LIB__

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

// C libraries
#include <stdint.h>

// ESP libraries
#include "esp_log.h"

// Personal libraries
#include "pipeWorker.h"



/**************************
**		DEFINITIONS		 **
**************************/

/**
 * @brief Creating Irrigator State list with X_MACRO 
 * 
 */
#define X_MACRO_IRRIGATION_STATE_LIST	\
	X(0, FULL_IRRIGATION				) \
	X(1, LITTLE_IRRIGATION				) \
	X(2, ENQUEUE_IRRIGATION				) \
	X(3, AWAIT							) \
	X(4, STOP							)


/**************************
**		STRUCTURES		 **
**************************/

/**
 * Enum for the IRRIGATION state
 */
typedef enum irrigation_state
{
	#define X(ID, ENUM) ENUM=ID, 
		X_MACRO_HTTP_SERVER_STATE_LIST
	#undef X
} irrigation_state_e;

/**
 * ENUM for the SOIL STATE
 */
typedef enum soil_state
{
	DRY_SOIL=0,
	EXTREME_DRY_SOIL=0,
	AWAITING_TO_IRRIGATE,
	IRRIGATING,
	MOIST_SOIL,
} soil_state_e;


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



#endif //__IRRIGATOR_PERSONAL_LIB__