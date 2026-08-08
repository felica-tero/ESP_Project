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
// C libraries
#include <stdint.h>

// ESP libraries
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

// Personal libraries
#include "irrigator.h"
#include "uart_sensorUmidity.h"
#include "projectConfig.h"



/**************************
**		DECLARATIONS	 **
**************************/

	/* Variables */
static const char TAG[] = "irrigator";


	/* Static Functions */
static void irrigation_sm(uint8_t pipeworkId, irrigation_state_e irrigation_state);


/**************************
**	FreeRTOS FUNCTIONS	 **
**************************/

void irrigator_setup(nw_update_valve_state_cb nw_update_valve_state_fn)
{
	ESP_LOGI(TAG, "pipework_setup");
	pipework_setup(nw_update_valve_state_fn);
	ESP_LOGI(TAG, "irrigator_freeRTOS_setup");
}



/**************************
**	   APP FUNCTIONS	 **
**************************/

void irrigationDecisor_fromSensor(uint8_t pipework_id)
{
	weather_state_e	weather_state = GOOD;
	uint8_t	soil_state = uart_UmidtSensor_geState(pipework_id);

	switch (soil_state)
	{
		case PONTO_DE_MURCHA_PERMANENTE:
			switch (weather_state)
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
					// IRRIGA SOH PARA AGUENTAR ESPERAR UM POUCO MAIS
					irrigation_sm(pipework_id, LITTLE_IRRIGATION);
				break;
			}
		break;


		case SOLO_SECO:
			switch (weather_state)
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


		case SOLO_UMIDO:
			// STOP
			pipework_closeValve(pipework_id);
		break;


		case CAPACIDADE_DE_CAMPO:
			// STOP
			pipework_closeValve(pipework_id);
		break;


		default:
			break;
	}
}


void irrigationDecisor_client(uint8_t pipework_id, uint8_t valve_desired_state)
{
	weather_state_e	weather_state = GOOD;
	uint8_t	soil_state = uart_UmidtSensor_geState(pipework_id);

	ESP_LOGI(TAG, "irrigationDecisor_client, PIPE %d: %d", pipework_id, valve_desired_state);

	if (OPEN == valve_desired_state)
	{
		switch (soil_state)
		{
			case PONTO_DE_MURCHA_PERMANENTE:
			case SOLO_SECO:
			case SOLO_MEIO_TERMO:
			case SOLO_UMIDO:
				// OPEN
				irrigation_sm(pipework_id, FULL_IRRIGATION);
				break;
			

			case CAPACIDADE_DE_CAMPO:
				// STOP
				pipework_closeValve(pipework_id);
				break;
			
			
			default:
				break;
		}
	}
	else if (CLOSE == valve_desired_state)
	{
		pipework_closeValve(pipework_id);
	}
}


static void irrigation_sm(uint8_t pipework_id, irrigation_state_e irrigation_state)
{
	ESP_LOGI(TAG, "irrigation_sm, PIPE %d: %d", pipework_id, irrigation_state);

	switch (irrigation_state)
	{
		case FULL_IRRIGATION:
			uart_UmidtSensor_setDesiredLevel(pipework_id, CAPACIDADE_DE_CAMPO);
			if (CLOSE == pipework_getState(pipework_id))
				pipework_monitor_enqueueOpen(pipework_id);
		break;
		

		case LITTLE_IRRIGATION:
			uart_UmidtSensor_setDesiredLevel(pipework_id, SOLO_MEIO_TERMO);
			if (CLOSE == pipework_getState(pipework_id))
				pipework_monitor_enqueueOpen(pipework_id);
		break;

		
		default:
			return;
	}
}