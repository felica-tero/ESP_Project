/**
 * @file router.h
 * @brief 
 * @details
 * @date 17 de nov. de 2024
 * @author Luiz Carlos
 */

#ifndef MAIN_ROUTER_H_
#define MAIN_ROUTER_H_


/**************************
**		  INCLUDES	 	 **
**************************/

// C libraries
#include <string.h>

// ESP libraries
#include "esp_err.h"
#include "esp_http_server.h"
#include "esp_log.h"

// Personal libraries
#include "esp_wifi_types_generic.h"
#include "wifiApp.h"
#include "httpServer.h"


/**************************
**		DEFINITIONS		 **
**************************/

#define BUFFER_MAX_SIZE 100

#define APP_URI_FUNCTION_HANDLER_NAME(uri) webRouter_##uri##_handler

/**
 * @brief Routes list with X_MACRO
 * @details
 */
#define X_MACRO_API_ROUTES_LIST \
	X(0, wifi_connect_json, 			"/wifiConnect.json",		HTTP_POST, 		"application/json") \
	X(1, wifi_connect_status_json,		"/wifiConnectStatus",		HTTP_POST, 		"application/json") \
	X(2, get_wifi_connect_info_json,	"/wifiConnectInfo.json",	HTTP_GET,		"application/json") \
	X(3, wifi_disconnect_json,			"/wifiDisconnect.json",		HTTP_DELETE,	"application/json") \
	X(4, http_server_OTA_update_handler, "/OTAupdate",				HTTP_POST, 		"application/octet-stream") \
	X(5, http_server_OTA_status_handler, "/OTAstatus",				HTTP_POST, 		"application/json")

/**************************
**		FUNCTIONS		 **
**************************/

void router_setup(void);

#endif /* MAIN_ROUTER_H_ */
