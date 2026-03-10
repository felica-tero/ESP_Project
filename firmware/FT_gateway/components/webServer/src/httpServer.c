/**
 * @file httpServer.c
 * @brief 
 * @details
 * @author Isabella Vecchi
 * @date 2024-11-17
 */

/**************************
**		  INCLUDES	 	 **
**************************/

// C libraries

// ESP libraries
#include "freertos/FreeRTOS.h"
#include "freertos/portmacro.h"
#include "esp_http_server.h"
#include "esp_log.h"
#include "freertos/idf_additions.h"

// Personal libraries
#include "httpServer.h"
#include "wifiApp.h"
#include "tasks_common.h"



/**************************
**		DECLARATIONS	 **
**************************/

	/* Variables */

// Tag used for ESP serial console messages
static const char TAG[] = "http_server";

// HTTP server task handle
static httpd_handle_t http_server_handle = NULL; ///> used on start and stop server

// Embedded files: JQuery, index.html, app.css, app.js and favicon.ico files
#define X(uri_handler, file, http_resp_type, start, end) \
	extern const uint8_t uri_handler##_start[]	asm(#start); \
	extern const uint8_t uri_handler##_end[]	asm(#end);
	X_MACRO_HTTP_SERVER_URI_HANDLER_LIST
#undef X
	
	/* Function Pointers */
api_routes_register_fn httpServer_uri_setApiRoutes_callback = NULL;


	/* Static Functions */

// App functions
static void httpServer_configure(httpd_config_t * config);
static void httpServer_setWifiCallback(void);
static void httpServer_uri_setFilesHandlersAndRoutes(void);
static void httpServer_uri_setRoutesFromOtherFiles(void);



/**************************
** UPPERLAYER FUNCTIONS	 **
**************************/

// Returns the WIFI connectStatus
uint8_t httpServer_get_wifiConnectStatus(void)
{
	return wifiApp_getConnStatus();
}

// Function to get routers from another file to be declared here.
void httpServer_setApiRoutes_cb(void (*apiFunction)(void))
{
	httpServer_uri_setApiRoutes_callback = apiFunction;
}

// Function to register an HTTP uri.
void httpServer_uri_registerHandler(const char* route, httpd_method_t method, esp_err_t (*handler)(httpd_req_t *req))
{
	httpd_uri_t uri_handler = {
		.uri 		= route,
		.method 	= method,
		.handler	= handler,
		.user_ctx	= NULL,
	};
	httpd_register_uri_handler(http_server_handle, &(uri_handler));
}



/**************************
**	   APP FUNCTIONS	 **
**************************/

void httpServer_setup(api_routes_register_fn apiFunction)
{

	httpServer_setWifiCallback();
	
	// Allocate the api routes inside the httpServer code
	httpServer_setApiRoutes_cb(apiFunction);
	
	// Start WiFi
	wifiApp_start();
}


void httpServer_tryToConnect(char * ssid, char * passwd)
{
	wifiApp_setCredentials(ssid, passwd);
	
	wifiApp_sendMessage(WIFI_APP_TRY_TO_CONNECT);
}

/**
 * Functions that get uri handler for when the files are requested when accessing the web page.
 * @param req HTTP request for which the uri needs to be handled.
 * @return ESP_OK
 */
#define X(uri_handler, file, http_resp_type, start, end) \
	static esp_err_t URI_FUNCTION_HANDLER_NAME(uri_handler)(httpd_req_t *req)	\
	{ 																			\
		ESP_LOGI(TAG, "%s requested", #uri_handler);							\
		\
		httpd_resp_set_type(req, http_resp_type);								\
		httpd_resp_send(req, (const char *)uri_handler##_start, uri_handler##_end - uri_handler##_start);\
		\
		return ESP_OK;\
	}
	X_MACRO_HTTP_SERVER_URI_HANDLER_LIST
	
#undef X
 
/**
 * Sets up the default http server configuration
 * @param httpd_config_t the httpd config structure for the HTTP server task
 */
static void httpServer_configure(httpd_config_t * config)
{	 
	// Adjust the default priority to 1 less than the WiFi application task
	config->task_priority = HTTP_SERVER_TASK_PRIORITY;
	
	// Bump up the stack size (default is 4096)
	config->stack_size = HTTP_SERVER_STACK_SIZE;
	
	// Increase uri handlers
	config->max_uri_handlers = HTTP_SERVER_MAX_URI_HANDLERS;
	
	// Increase the timeout limits
	config->recv_wait_timeout = HTTP_SERVER_TIMEOUT_LIMIT;
	config->send_wait_timeout = HTTP_SERVER_TIMEOUT_LIMIT;
	
	ESP_LOGI(TAG, "httpServer_configure: Starting server on port: '%d' with task priority: '%d'",
					config->server_port,
					config->task_priority);
}

/**
 * Registers the URI handlers
 */
static void httpServer_uri_setFilesHandlersAndRoutes(void)
{
	#define X(uri_handler, file, http_resp_type, start, end)	\
	httpServer_uri_registerHandler(file, HTTP_GET,URI_FUNCTION_HANDLER_NAME(uri_handler));
	
		X_MACRO_HTTP_SERVER_URI_HANDLER_LIST
		
	#undef X
}

static void httpServer_uri_setRoutesFromOtherFiles(void)
{
	if (httpServer_uri_setApiRoutes_callback)
	{
		httpServer_uri_setApiRoutes_callback();
	}
}
 
/**
 * Starts the HTTP server.
 */
void httpServer_start(void)
{
	if (http_server_handle == NULL)
	{
		// Initializing and configurating the structure
		httpd_config_t config = HTTPD_DEFAULT_CONFIG();
		httpServer_configure(&config);

		//Start the httpd server
		if (httpd_start(&http_server_handle, &config) == ESP_OK)
		{
			ESP_LOGI(TAG, "httpServer_configure: Registering URI handlers");
			httpServer_uri_setFilesHandlersAndRoutes();
			httpServer_uri_setRoutesFromOtherFiles();
		} else
		{
			http_server_handle = NULL;
		}
	}
}

/**
 * Stops the HTTP server.
 */
void httpServer_stop(void)
{
	if (http_server_handle)
	{
		httpd_stop(http_server_handle);
		ESP_LOGI(TAG, "httpServer_stop: stopping HTTP server");
		http_server_handle = NULL;
	}
}

/**
 * Callback for starting http server after WiFi signal is ready.
 */
static void httpServer_setWifiCallback(void)
{
	wifiApp_setCallbacks(
		httpServer_start,
		NULL
	);
}