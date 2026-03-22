/**
 * @file httpServer.h
 * @brief 
 * @details
 * @author Isabella Vecchi
 * @date 2024-11-17
 */

#ifndef MAIN_HTTPSERVER_H_
#define MAIN_HTTPSERVER_H_


/**************************
**		  INCLUDES	 	 **
**************************/

// ESP libraries
#include "esp_http_server.h"

/**************************
**		DEFINITIONS		 **
**************************/

#define URI_FUNCTION_HANDLER_NAME(uri_handler) http_server_ ## uri_handler ## _handler
#define BINARY_START(bin,uri_handler,s) #bin###uri_handler###s

#define HTTP_SERVER_MAX_URI_HANDLERS	40

/**
 * @brief Server timeout in seconds
 */
#define HTTP_SERVER_TIMEOUT_LIMIT		10 //seconds

/**
 * @brief Creating Server State list with X_MACRO 
 * 
 */
#define X_MACRO_HTTP_SERVER_STATE_LIST	\
	X(0, HTTP_WIFI_CONNECT_INIT			) \
	X(1, HTTP_WIFI_CONNECT_SUCCESS		) \
	X(2, HTTP_WIFI_CONNECT_FAIL			) \
	X(3, HTTP_WIFI_USER_DISCONNECT)

/**
 * @brief Creating Routes with X_MACRO
 * 
 * Explanation of how to add more routes using X_MACRO
 */
#define X_MACRO_HTTP_SERVER_URI_HANDLER_LIST\
	X(app_css,				"/app.css",				"text/css"				, _binary_app_css_start, 				_binary_app_css_end				) \
	X(app_js,				"/app.js",				"application/javascript", _binary_app_js_start, 				_binary_app_js_end				) \
	X(app_html,				"/app.html",			"text/html"				, _binary_app_html_start, 				_binary_app_html_end			) \
	X(dados_png,			"/dados.png",			"image/x-icon"			, _binary_dados_png_start, 				_binary_dados_png_end			) \
	X(favicon_ico,			"/favicon.ico",			"image/x-icon"			, _binary_favicon_ico_start, 			_binary_favicon_ico_end			) \
	X(historico_html,		"/historico.html",		"text/html"				, _binary_historico_html_start, 		_binary_historico_html_end			) \
	X(historico_png,		"/historico.png",		"image/x-icon"			, _binary_historico_png_start, 			_binary_historico_png_end		) \
	X(home_png,				"/home.png",			"image/x-icon"			, _binary_home_png_start, 				_binary_home_png_end			) \
	X(index_html,			"/index.html",			"text/html"				, _binary_index_html_start, 			_binary_index_html_end			) \
	X(jquery_3_3_1_min_js,	"/jquery-3.3.1.min.js",	"application/javascript", _binary_jquery_3_3_1_min_js_start,	_binary_jquery_3_3_1_min_js_end	) \
	X(irrigaAI_png,			"/irrigaAI.png",		"image/x-icon"			, _binary_IrrigaAI_png_start, 			_binary_IrrigaAI_png_end		) \
	X(arrow_max_svg,		"/arrow-max.svg",		"image/x-icon"			, _binary_arrow_max_svg_start, 			_binary_arrow_max_svg_end		) \
	X(arrow_min_svg,		"/arrow-min.svg",		"image/x-icon"			, _binary_arrow_min_svg_start, 			_binary_arrow_min_svg_end		) \
	X(irrigation_svg,		"/irrigation.svg",		"image/x-icon"			, _binary_irrigation_svg_start, 		_binary_irrigation_svg_end		) \
	X(animated_1_svg,		"/1.svg",				"image/x-icon"			, _binary_1_svg_start,					_binary_1_svg_end				) \
	X(animated_2_svg,		"/2.svg",				"image/x-icon"			, _binary_2_svg_start,					_binary_2_svg_end				) \
	X(animated_3_svg,		"/3.svg",				"image/x-icon"			, _binary_3_svg_start,					_binary_3_svg_end				) \
	X(animated_4_svg,		"/4.svg",				"image/x-icon"			, _binary_4_svg_start,					_binary_4_svg_end				) \
	X(animated_5_svg,		"/5.svg",				"image/x-icon"			, _binary_5_svg_start,					_binary_5_svg_end				) \
	X(animated_6_svg,		"/6.svg",				"image/x-icon"			, _binary_6_svg_start,					_binary_6_svg_end				) \
	X(animated_7_svg,		"/7.svg",				"image/x-icon"			, _binary_7_svg_start,					_binary_7_svg_end				) \
	X(animated_8_svg,		"/8.svg",				"image/x-icon"			, _binary_8_svg_start,					_binary_8_svg_end				) \
	X(animated_9_svg,		"/9.svg",				"image/x-icon"			, _binary_9_svg_start,					_binary_9_svg_end				) \
	X(animated_10_svg,		"/10.svg",				"image/x-icon"			, _binary_10_svg_start,					_binary_10_svg_end				) \
	X(animated_11_svg,		"/11.svg",				"image/x-icon"			, _binary_11_svg_start,					_binary_11_svg_end				) \
	X(animated_12_svg,		"/12.svg",				"image/x-icon"			, _binary_12_svg_start,					_binary_12_svg_end				) \
	X(animated_13_svg,		"/13.svg",				"image/x-icon"			, _binary_13_svg_start,					_binary_13_svg_end				) \
	X(weather_sunset_svg,	"/weather_sunset.svg",	"image/x-icon"			, _binary_weather_sunset_svg_start,		_binary_weather_sunset_svg_end	)

/**************************
**		STRUCTURES		 **
**************************/


/**
 * Enum for the HTTP monitor
 */
typedef enum http_server_state
{
	#define X(ID, ENUM) ENUM=ID, 
		X_MACRO_HTTP_SERVER_STATE_LIST
	#undef X
} http_server_state_e;

/**
 * Structure for the message queue
 */
typedef struct http_server_queue_message_s
{
	http_server_state_e msgId;
} http_server_queue_message_t;

typedef void (*api_routes_register_fn)(void);


/**************************
**		FUNCTIONS		 **
**************************/

/**
 * @brief Sends a message to the queue
 * @param msgId message ID from the http_server_state_e enum.
 * @return pdTRUE if an item was successfully sent to the queue, otherwise pdFALSE.
 */
BaseType_t httpServer_monitor_sendMessage(http_server_state_e msgId);
 

/**
 * Setup the HTTP server.
 */
void httpServer_setup(api_routes_register_fn apiFunction);


void httpServer_tryToConnect(char * ssid, char * passwd);

/**
 * Starts the HTTP server.
 */
void httpServer_start(void);

/**
 * Stops the HTTP server.
 */
void httpServer_stop(void);

/**
 * Timer callback function which calls esp_restart upon successful firmware update.
 */
void ota_fw_update_reset_callback(void *arg);


/**************************
** UPPERLAYER FUNCTIONS	 **
**************************/

/**
 * @brief Returns the wifi_connect_status_t
 * 
 * @return http_server_wifi_connect_status_e * 
 */
uint8_t httpServer_get_wifiConnectStatus(void);

/**
 * Function to get routers from another file to be declared here.
 * Separating the include files routes, from the api ones.
 * 
 * @param apiFunction a function from an upper layer, where other
 * uri routes are declared with the httpServer_uri_registerHandler function.
 */
void httpServer_setApiRoutes_cb(api_routes_register_fn);

/**
 * Function to register an HTTP uri.
 * 
 * @param route the http route on the IP server
 * @param method if the route is accessed by GET, POST, PUT...
 * @param handler the function handler that specifies what happens when the route is accessed.
 */
void httpServer_uri_registerHandler(const char* route, httpd_method_t method, esp_err_t (*handler)(httpd_req_t *req));

#endif /* MAIN_HTTPSERVER_H_ */
