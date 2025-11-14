/**
 * @file hal_gpio.h
 * @brief 
 * @details
 * @author Isabella V. Ferreira
 * @date 2025-10-01
 */

#ifndef HAL_GPIO_H_
#define HAL_GPIO_H_


/**************************
**		  INCLUDES	 	 **
**************************/

// C libraries
#include <stdio.h>
#include <stdint.h>


// ESP libraries
#include "driver/gpio.h"

// Personal libraries
#include "projectConfig.h"



/**************************
**		DEFINITIONS		 **
**************************/


/**************************
**		STRUCTURES		 **
**************************/

/**
 * @brief General Digital Output configuration
 */
typedef struct dig_out_info_s
{
	uint8_t gpio_pin;
	uint8_t pull_up;
	uint8_t pull_down;
	uint8_t start_level;
} dig_out_info_t;

/**
 * @brief General Digital Input configuration
 */
typedef void (*gpio_in_interrupt_fn_t)(void*);
typedef struct dig_in_info_s
{
	uint8_t gpio_pin;
	uint8_t pull_up;
	uint8_t pull_down;
	uint8_t intr_type;
	gpio_in_interrupt_fn_t cb_function;
} dig_in_info_t;

/**************************
**		FUNCTIONS		 **
**************************/

/**
 * Initializes the RGB LED settings per channel, including
 * the GPIO for each color, mode and timer configuration.
 */
void hal_gpio_init(void);

void hal_gpio_setupDigOut(	dig_out_info_t * dig_out_info,
                            uint8_t gpio_pin,
                            uint8_t pull_up,
                            uint8_t pull_down,
                            uint8_t start_level);

void hal_gpio_setupDigIn(	dig_in_info_t * dig_in_info,
                            uint8_t gpio_pin,
                            uint8_t pull_up,
                            uint8_t pull_down,
                            uint8_t intr_type,
                            gpio_in_interrupt_fn_t cb_function);

uint8_t hal_gpio_setOutput(uint8_t pin, uint8_t level);


#endif /* HAL_GPIO_H_ */
