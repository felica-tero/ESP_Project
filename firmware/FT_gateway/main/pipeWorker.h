#ifndef __PIPE_WORKER_PERSONAL_LIB__
#define __PIPE_WORKER_PERSONAL_LIB__

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
#include "projectConfig.h"
#include "hal_gpio.h"



/**************************
**		DEFINITIONS		 **
**************************/
#define X_MACRO_PIPE_LIST			 	\
	X(0, PIPE_0, DIG_OUT_0, PULL_UP, LOW)\
	X(1, PIPE_1, DIG_OUT_1, PULL_UP, LOW)\
	X(2, PIPE_2, DIG_OUT_2, PULL_UP, LOW)\
	X(3, PIPE_3, DIG_OUT_3, PULL_UP, LOW)
// 	X(id, pipe,	pin, output_type, initial_value)


/**
 * @brief List used to create all the structures for the Irrigation state machine
 * based on X definition
 * @details
 */
#define X_MACRO_VALVE_STATE_LIST \
	X(0, OPEN     ) \
	X(1, AWAITING   ) \
	X(2, CLOSE       ) \
	X(3, ERROR      )
//  X(enum, PIPE_STATE)




/**************************
**		STRUCTURES		 **
**************************/

/**
 * ENUM for the Pipe STATE
 */
typedef enum pipeWork_state
{
	#define X(ID, ENUM) ENUM=ID, 
		X_MACRO_VALVE_STATE_LIST
	#undef X
} pipeWork_state_e;

/**
 * ENUM for Pipe ID
 */
typedef enum pipeWork_id
{
	#define X(ID, PIPE, pin, output_type, initial_value) PIPE=ID, 
		X_MACRO_PIPE_LIST()
	#undef X
} pipeWork_id_e;


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
void pipeWorker_setup(void);
pipeWork_state_e pipeWorker_openValve(uint8_t pipeworkId);
pipeWork_state_e pipeWorker_closeValve(uint8_t pipeworkId);
void pipeWorker_loop(void);



#endif //__PIPE_WORKER_PERSONAL_LIB__