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

// Personal libraries
#include "projectConfig.h"



/**************************
**		DEFINITIONS		 **
**************************/

/**
 * @brief X MACRO for DIG_OUT pins
 */
// TODO: Abstract this on hal_gpio
#define X_MACRO_DIG_OUT \
	X(0, DIG_OUT_0, TRUE, FALSE, LOW) \
	X(1, DIG_OUT_1, TRUE, FALSE, LOW) \
	X(2, DIG_OUT_2, TRUE, FALSE, LOW) \
	X(3, DIG_OUT_3, TRUE, FALSE, LOW)
//  X(PIPE_ID, pin, pull_up, pull_down, start_level)

/**************************
**		STRUCTURES		 **
**************************/

/**
 * @brief RGB LED configuration
 */
typedef struct dig_out_info_s
{
	uint8_t gpio_pin;
	uint8_t pull_up;
	uint8_t pull_down;
	uint8_t start_level;
} dig_out_info_t;

/**************************
**		FUNCTIONS		 **
**************************/

/**
 * Initializes the RGB LED settings per channel, including
 * the GPIO for each color, mode and timer configuration.
 */
void hal_gpio_init(void);


#endif /* HAL_GPIO_H_ */
