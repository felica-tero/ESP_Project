/**
 * @file wifiApp.h
 * @brief 
 * @details
 * @date 16 de nov. de 2024
 * @author Isabella Vecchi
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

#define WIFI_APP_NAMESPACE	"wifi"
#define WIFI_APP_KEY		"creds"

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
	X(0, WIFI_APP_SIGNAL_READY					) \
	X(1, WIFI_APP_TRY_TO_CONNECT				) \
	X(2, WIFI_APP_STA_CONNECTED_GOT_IP			) \
	X(3, WIFI_APP_USER_REQUESTED_STA_DISCONNECT	) \
	X(4, WIFI_APP_STA_DISCONNECTED				) \
	X(5, WIFI_APP_LOAD_SAVED_CREDENTIALS		)



/**************************
**		STRUCTURES		 **
**************************/

typedef struct wifiApp_credentials_s
{
	char ssid[WIFI_SSID_LENGTH];
	char passwd[WIFI_PASSWORD_LENGTH];
} wifiApp_credentials_t;

/**
 * @brief Wifi callback function type
 * @details
 */
typedef void(*wifi_event_callback_t)(void);

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


typedef struct wifi_fn_callbacks_s
{
	wifi_event_callback_t	signal_ready_cb;
	wifi_event_callback_t	sta_connected_cb;
} wifi_fn_callbacks_t;


/**
 * Connection status for WiFi
 */
typedef enum wifi_connect_status_e
{
	NONE = 0,
	WIFI_STATUS_CONNECTING,
	WIFI_STATUS_CONNECT_FAILED,
	WIFI_STATUS_CONNECT_SUCCESS,
	WIFI_STATUS_DISCONNECTED,
} wifi_connect_status_t;

/**************************
**		  GETTERS		 **
**************************/

/**
 * @brief Gets the WiFi connection status
 * @details
 * @return wifi_connect_status_t 
 */
uint8_t wifiApp_getConnStatus(void);

esp_err_t wifiApp_getWifiConnectInfo(char * out_ssid, char * out_ip, char * out_netmask, char * out_gateway);


/**************************
**		  SETTERS		 **
**************************/

// Sets the WiFi credentials info
void wifiApp_setCredentials(char * ssid, char * passwd);


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
 * @brief Sets the callbacks functions
 * @details
 * @param wifi_fn_callbacks_t functions 
 */
void wifiApp_setCallbacks(
	wifi_event_callback_t signal_ready_cb,
	wifi_event_callback_t sta_connected_cb
);


#endif /* MAIN_WIFIAPP_H_ */
