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
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

// Personal libraries
#include "projectConfig.h"
#include "hal_gpio.h"



/**************************
**		DEFINITIONS		 **
**************************/
#define PULL_UP		TRUE
#define PULL_DOWN	FALSE

#define X_MACRO_PIPE_LIST			 				\
	X(0, PIPE_0, DIG_OUT_0, PULL_UP, PULL_DOWN, LOW)\
	X(1, PIPE_1, DIG_OUT_1, PULL_UP, PULL_DOWN, LOW)\
	X(2, PIPE_2, DIG_OUT_2, PULL_UP, PULL_DOWN, LOW)\
	X(3, PIPE_3, DIG_OUT_3, PULL_UP, PULL_DOWN, LOW)
// 	X(id, pipe,	pin, output_type, initial_value)

/**************************
**		STRUCTURES		 **
**************************/

/**
 * ENUM for the Pipe STATE
 */
typedef enum pipework_state
{
	OPEN=0,
	CLOSE,
	AWAIT,
	ERROR,
} pipework_state_e;

typedef struct pipeworker_ctrl_s
{
	pipework_state_e state;
	dig_out_info_t config;
}pipeworker_ctrl_t;


/**
 * ENUM for Pipe ID
 */
typedef enum pipework_id
{
	#define X(ID, PIPE,	pin, pullUp, pullDown, initial_value)\
			PIPE=ID, 
		X_MACRO_PIPE_LIST
	#undef X
} pipework_id_e;


/**************************
**		FUNCTIONS		 **
**************************/
void pipeworker_setup(void);
pipework_state_e pipeworker_askToOpenValve(uint8_t pipeworkId);
void pipeworker_closeValve(uint8_t pipeworkId);
pipework_state_e pipeworker_getState(uint8_t pipeworkId);



#endif //__PIPE_WORKER_PERSONAL_LIB__