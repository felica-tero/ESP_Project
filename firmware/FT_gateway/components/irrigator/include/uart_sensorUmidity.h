/**
 * @file irrigator.h
 * @author Isabella Vecchi Ferreira
 * @brief
 * @details
 * @version 0.1
 * @date 2025-11-03
 * 
 */

#ifndef __UART_SENSOR_UMIDADE_PERSONAL_LIB__
#define __UART_SENSOR_UMIDADE_PERSONAL_LIB__


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
 * ENUM for the SOIL STATE
 */
typedef enum soil_state
{
	PONTO_DE_MURCHA_PERMANENTE=0,
	SOLO_SECO,
	AGUARDANDO,
	IRRIGANDO,
	SOLO_UMIDO,
	CAPACIDADE_DE_CAMPO,
} soil_state_e;


/**************************
**		FUNCTIONS		 **
**************************/

void uart_UmidtSensor_setup(void);
void uart_UmidtSensor_setDesiredLevel(pipeWork_id_e pipeWork_id, soil_state_e soil_state);
void uart_UmidtSensor_reqReading(pipeWork_id_e pipeWork_id);
uint16_t uart_UmidtSensor_readData(void);
void uart_UmidtSensor_reqToSend(void);
uint8_t uart_UmidtSensor_sendByte(uint8_t data);



#endif //__UART_SENSOR_UMIDADE_PERSONAL_LIB__