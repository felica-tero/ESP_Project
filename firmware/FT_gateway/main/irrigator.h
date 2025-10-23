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




/**************************
**		STRUCTURES		 **
**************************/


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