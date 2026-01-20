/**
 * @file irrigator.h
 * @author Isabella Vecchi Ferreira
 * @brief
 * @details
 * @version 0.1
 * @date 2025-11-03
 * 
 */


/**************************
**		  INCLUDES	 	 **
**************************/

// Personal Libraries
#include "uart_sensorUmidity.h"



/**************************
**		DECLARATIONS	 **
**************************/

	/* Variables */


	/* FreeRTOS Structures */

    
	/* Static Functions */
static void uart_UmidtSensor_init(void);



/**************************
**		FUNCTIONS		 **
**************************/

void uart_UmidtSensor_setup(void)
{

}

void uart_UmidtSensor_setDesiredLevel(pipeWork_id_e pipeWork_id, soil_state_e soil_state)
{
	// 2 bits para pipeWork_id, 3 bits para soil_state
	// uart_UmidtSensor_sendByte()
}

void uart_UmidtSensor_reqReading(pipeWork_id_e pipeWork_id)
{
	// toggle output que dispara interrupcao no STM
}

// funcao de callback do toggle ready to send do STM
uint16_t uart_UmidtSensor_readData(void)
{
	return 0;
}

void uart_UmidtSensor_reqToSend(void)
{
	// toggle output que dispara interrupcao no STM
}

// funcao de callback do toggle clear to send do STM
uint8_t uart_UmidtSensor_sendByte(uint8_t data)
{
	// envia um byte na uart
	return 0;
}
