/**
 * @file wifiApp.h
 * @brief 
 * @details
 * @date 16 de nov. de 2024
 * @author Luiz Carlos
 */

#ifndef MAIN_WIFIAPP_H_
#define MAIN_WIFIAPP_H_


/**************************
**		  INCLUDES	 	 **
**************************/

// ESP libraries
#include "esp_netif.h"
#include "esp_wifi_types_generic.h"
#include "freertos/portmacro.h"

// Personal libraries
#include "projectConfig.h"


/**************************
**		DEFINITIONS		 **
**************************/


/**
 * @brief Macro to define the function name of each state
 * @details
 */
#define WIFI_STATE_FUNC_NAME(state) wifiApp_sm_ ## state ## _fn

/**
 * @brief List used to create all the structures for the WiFi state machine
 * based on X definition
 * @details
 */
#define X_MACRO_WIFI_STATE_LIST 				\
	X(0, WIFI_APP_START_HTTP_SERVER				) \
	X(1, WIFI_APP_CONNECTING_FROM_HTTP_SERVER	) \
	X(2, WIFI_APP_STA_CONNECTED_GOT_IP			) \
	X(3, WIFI_APP_USER_REQUESTED_STA_DISCONNECT	) \
	X(4, WIFI_APP_STA_DISCONNECTED				) \
	X(5, WIFI_APP_LOAD_SAVED_CREDENTIALS)



/**************************
**		STRUCTURES		 **
**************************/

/**
 * @brief Wifi callback function type
 * @details
 */
typedef void(*wifi_connected_event_callback_t)(void);

/**
 * @brief Message IDs for the WiFi application task
 * @details
 */
typedef enum
{
	#define X(ID, ENUM) ENUM=ID,
		X_MACRO_WIFI_STATE_LIST
	#undef X
} sm_wifi_app_state_e;

/**
 * @brief Messages for each WiFi application task State
 * @details
 */
static const char * sm_wifi_app_state_names[] = 
{
#define X(ID, ENUM) #ENUM,
	X_MACRO_WIFI_STATE_LIST
#undef X
};

/**
 * @brief Structure for the message queue
 * @details
 */
typedef struct wifi_app_queue_message_s
{
    sm_wifi_app_state_e  wifiApp_state;
} wifi_app_queue_message_t;

// Defining a type for every function of the WiFi state machine
typedef void (*sm_wifi_app_function)(wifi_app_queue_message_t * st);

// Creating a struct to hold those functions later on an array
typedef struct sm_wifi_table_fn_s
{
	sm_wifi_app_function func;
} sm_wifi_table_fn_t;



/**************************
**		  GETTERS		 **
**************************/

/**
 * @brief Returns the Address of the first char of SSID string
 * @details
 * @return char * the Address of the first char of SSID string
 */
char * wifiApp_getStationSSID(void);

/**
 * @brief Returns the Address of the first char of password string
 * @details
 * @return char * the Address of the first char of password string
 */
char * wifiApp_getStationPassword(void);

/**
 * @brief Gets the WiFi configuration
 * @details
 * @return wifi_config_t* 
 */
wifi_config_t * wifiApp_getWifiConfig(void);



/**************************
**		FUNCTIONS		 **
**************************/

/**
 * @brief Sends a message to the queue
 * @param msgId message ID from the wifi_app_message_e enum
 * @return pdTRUE if an item was successfully sent to the queue, otherwise pdFALSE
 */
BaseType_t wifiApp_sendMessage(sm_wifi_app_state_e msgId);

/**
 * @brief Starts the WiFi RTOS task
 * @details
 */
void wifiApp_start(void);

/**
 * @brief Sets the callback function
 * @details
 * @param cbFunction 
 */
void wifiApp_setCallback(wifi_connected_event_callback_t cbFunction);


#endif /* MAIN_WIFIAPP_H_ */
