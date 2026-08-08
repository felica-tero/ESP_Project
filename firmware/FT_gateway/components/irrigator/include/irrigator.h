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

// Personal libraries
#include "pipework.h"



/**************************
**		DEFINITIONS		 **
**************************/
/**
 * @brief Creating Irrigator State list with X_MACRO 
 * 
 */
#define X_MACRO_IRRIGATION_STATE_LIST	\
	X(0, FULL_IRRIGATION				)\
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


/**************************
**		FUNCTIONS		 **
**************************/
void irrigator_setup(nw_update_valve_state_cb nw_update_valve_state_fn);

void irrigationDecisor_fromSensor(uint8_t pipework_id);
void irrigationDecisor_client(uint8_t pipework_id, uint8_t valve_desired_state);


#endif //__IRRIGATOR_PERSONAL_LIB__