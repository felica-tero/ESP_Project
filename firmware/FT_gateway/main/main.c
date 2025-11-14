/**
 * @file main.c
 * @author your name (you@domain.com)
 * @brief Application Entry Point.
 * @version 0.1
 * @date 2025-05-21
 * 
 * @copyright Copyright (c) 2025
 * 
 */

/**************************
**		  INCLUDES	 	 **
**************************/

// C libraries

// ESP libraries
#include "esp_err.h"
#include "nvs.h"
#include "nvs_flash.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// Personal libraries
#include "tasks_common.h"
#include "ledRGB.h"
#include "displayOled.h"
#include "router.h"
#include "dateTimeNTP.h"
#include "irrigator.h"


/**************************
**		DECLARATIONS	 **
**************************/

	/* Variables */

/**
 * @brief Tag used for ESP serial console messages
 * 
 */
static const char TAG[] = "Main";


/**************************
**		 FUNCTIONS		 **
**************************/


static void mainLoop_task(void);
static void vTaskLoop(void * pvParameters);

void app_main(void)
{
	// TODO: separate the init functions of tasks, to be called here

	// Initialize NVS (Non Volatile Storage)
	esp_err_t ret = nvs_flash_init();
	if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
	{
		ESP_ERROR_CHECK(nvs_flash_erase());
		ret = nvs_flash_init();
	}
	ESP_ERROR_CHECK(ret);
	
	// Initialize the LEDS
	ledRGB_ledPWM_init();

	#if DISPLAY_OLED_PRESENT == TRUE
	// Initialize the display OLED	
	displayOled_setup();
	#endif

	// Web Router
	router_setup();

	// NTP clock setup
	dateTimeNTP_setup();

	// Irrigator setup
	irrigator_setup();

	// Main Infinite Loop
	mainLoop_task();
}


//task to be created
static void vTaskLoop(void * pvParameters)
{
	// TODO: add here, before loop the tasks of components
	for(;;)
	{
		#if DISPLAY_OLED_PRESENT == TRUE
		displayOled_printDateTime(dateTimeNTP_getData(), dateTimeNTP_getTime());
		#endif
        vTaskDelay(60000 / portTICK_PERIOD_MS); // Sync every 60 seconds (1 minute)
	}
}

/**
 * Function to instantiate the infinite state of the Felica-Tero product
 */
static void mainLoop_task(void)
{
	ESP_LOGI(TAG, "Infinite Loop State");

	// Start the fetch dateTime Task
	xTaskCreate(&vTaskLoop,
				"main_loop",
				MAIN_LOOP_TASK_STACK_SIZE,
				NULL,
				MAIN_LOOP_TASK_PRIORITY,
				NULL);
}