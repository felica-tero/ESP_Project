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
#include "pipework.h"

/**************************
**		DEFINITIONS		 **
**************************/

typedef void (*dataRead_callback_fn)(void);


/**************************
**		STRUCTURES		 **
**************************/

/**
 * ENUM for the SOIL STATE
 */
typedef enum soil_state
{
	CAPACIDADE_DE_CAMPO=0,
	SOLO_UMIDO,
	SOLO_MEIO_TERMO,
	SOLO_SECO,
	PONTO_DE_MURCHA_PERMANENTE,
} soil_state_e;


/**************************
**		FUNCTIONS		 **
**************************/

void uart_UmidtSensor_setup(void);
void uart_UmidtSensor_setDesiredLevel(pipework_id_e pipework_id, soil_state_e soil_state);
void uart_UmidtSensor_reqReading(pipework_id_e pipework_id);
uint16_t uart_UmidtSensor_readData(void);
void uart_UmidtSensor_reqToSend(void);
uint8_t uart_UmidtSensor_sendByte(uint8_t data);

uint8_t uart_UmidtSensor_geState(uint8_t pipework_id);
void uart_UmidtSensor_calibrarSensor(void);
void uart_UmidtSensor_dataReady_callback(dataRead_callback_fn);


#endif //__UART_SENSOR_UMIDADE_PERSONAL_LIB__