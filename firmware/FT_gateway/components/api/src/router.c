/**
 * @file router.c
 * @brief 
 * @details
 * @date 17 de nov. de 2024
 * @author Luiz Carlos
 */


/**************************
**		  INCLUDES	 	 **
**************************/

// C libraries
#include <stdio.h>
#include <string.h>
#include <stdint.h>

// ESP libraries
#include "esp_err.h"
#include "esp_http_server.h"
#include "esp_http_client.h"
#include "esp_log.h"
#include "cJSON.h"
#include "esp_netif.h"
#include "esp_netif_ip_addr.h"
#include "esp_netif_types.h"
#include "esp_wifi.h"
#include "esp_wifi_types_generic.h"

// Personal libraries
#include "router.h"
#include "httpServer.h"
#include "web_socket.h"
#include "otaUpdate.h"
#include "irrigator.h"


/**************************
**		DECLARATIONS	 **
**************************/

	/* Variables */

// Tag used for ESP serial console messages
static const char TAG[] = "http_app_server";

// Netif object for the Station and Access Point
extern esp_netif_t * esp_netif_sta;
extern esp_netif_t * esp_netif_ap;

// Buffer for LocalTime json string
char localJSONObjBuffer[BUFFER_MAX_SIZE] = {0};


/* Static Functions */

// URI handler functions
static esp_err_t APP_URI_FUNCTION_HANDLER_NAME(wifi_connect_json)(httpd_req_t *req);
static esp_err_t APP_URI_FUNCTION_HANDLER_NAME(wifi_connect_status_json)(httpd_req_t *req);
static esp_err_t APP_URI_FUNCTION_HANDLER_NAME(get_wifi_connect_info_json)(httpd_req_t *req);
static esp_err_t APP_URI_FUNCTION_HANDLER_NAME(wifi_disconnect_json)(httpd_req_t *req);
static esp_err_t APP_URI_FUNCTION_HANDLER_NAME(get_ssid_list)(httpd_req_t *req);
static esp_err_t APP_URI_FUNCTION_HANDLER_NAME(turn_valve_on_off)(httpd_req_t *req);
esp_err_t APP_URI_FUNCTION_HANDLER_NAME(http_server_OTA_update_handler)(httpd_req_t *req);
esp_err_t APP_URI_FUNCTION_HANDLER_NAME(http_server_OTA_status_handler)(httpd_req_t *req);
static void router_uri_register(void);



/**************************
**	   APP FUNCTIONS 	 **
**************************/

void router_setup(void)
{
	httpServer_setup(router_uri_register);
}



/**************************
**	 HANDLER FUNCTIONS 	 **
**************************/

/**
 * Receives the .bin file fia the web page and handles the firmware update
 * @param req HTTP request for which the uri needs to be handled.
 * @return ESP_OK, otherwise ESP_FAIL if timeout occurs and the update cannot be started.
 */
esp_err_t APP_URI_FUNCTION_HANDLER_NAME(http_server_OTA_update_handler)(httpd_req_t *req)
{
    esp_ota_handle_t ota_handle;
    char ota_buff[1024];
    int content_length = req->content_len;
    int content_received = 0;
    int recv_len;
    bool is_req_body_started = false;
    bool flash_successful = false;

    const esp_partition_t *update_partition = esp_ota_get_next_update_partition(NULL);

    do {
        recv_len = httpd_req_recv(req, ota_buff, MIN(content_length, sizeof(ota_buff)));
        if (recv_len < 0) {
            if (recv_len == HTTPD_SOCK_ERR_TIMEOUT) {
                ESP_LOGI(TAG, "Socket Timeout");
                continue;
            }
            ESP_LOGI(TAG, "OTA other Error %d", recv_len);
            return ESP_FAIL;
        }
        ESP_LOGI(TAG, "OTA RX: %d of %d", content_received, content_length);

        esp_err_t err;
        if (!is_req_body_started) {
            is_req_body_started = true;
            ESP_LOGI(TAG, "OTA file size: %d", content_length);
            err = ota_process_first_chunk(ota_buff, recv_len, &content_received, &ota_handle, update_partition);
        } else {
            err = ota_process_next_chunk(ota_buff, recv_len, &content_received, ota_handle);
        }
        if (err != ESP_OK) return ESP_FAIL;

    } while (recv_len > 0 && content_received < content_length);

    flash_successful = ota_finalize_and_set_boot(ota_handle, update_partition);
    ota_update_status(flash_successful);

    return ESP_OK;
}


/**
 * OTA status handler responds with the firmware update status after the OTA update is started
 * 
 * and responds with the compile time/date when the page is first requested
 * @param req HTTP request for which the uri needs to be handled
 * @return ESP_OK
 */
esp_err_t APP_URI_FUNCTION_HANDLER_NAME(http_server_OTA_status_handler)(httpd_req_t *req)
{
	char otaJSON[100];
	ESP_LOGI(TAG, "OTAstatus requested");

	sprintf(otaJSON, "{\"ota_update_status\":%d,\"compile_time\":\"%s\",\"compile_date\":\"%s\"}", g_fw_update_status, __TIME__, __DATE__);

	httpd_resp_set_type(req, "application/json");
	httpd_resp_send(req, otaJSON, strlen(otaJSON));

	return ESP_OK;
}


/**
 * wifiConnect.json handler is invoked after the connect button is pressed
 * and handles receiving the SSID and password entered by the user
 * @param req HTTP request for which the uri needs to be handled.
 * @return ESP_OK
 */
static esp_err_t APP_URI_FUNCTION_HANDLER_NAME(wifi_connect_json)(httpd_req_t *req)
{	
	// char localJSONObjBuffer[BUFFER_MAX_SIZE];
	size_t lenBodyJson = 0;

	ESP_LOGI(TAG, "/wifiConnect.json requested");
	
	memset(localJSONObjBuffer,0, BUFFER_MAX_SIZE);
	
	// Get Request Body
	lenBodyJson = req->content_len;
	httpd_req_recv(req, localJSONObjBuffer, lenBodyJson);
	printf("Data sent by the client: %.*s\n",lenBodyJson, localJSONObjBuffer);

	// Parse the JSON data (example using cJSON)
	cJSON *body_json = cJSON_Parse(localJSONObjBuffer);
	if (!body_json) {
		ESP_LOGE(TAG, "Failed to parse JSON data");
		cJSON_Delete(body_json);
		return ESP_FAIL;
	}

	cJSON *ssid_json = cJSON_GetObjectItemCaseSensitive(body_json, "c_ssid");
	cJSON *pwd_json = cJSON_GetObjectItemCaseSensitive(body_json, "c_pwd");

	if (!ssid_json || !pwd_json) {
		ESP_LOGE(TAG, "Missing 'c_ssid' or 'c_pwd' in JSON data");
		cJSON_Delete(body_json);
		return ESP_FAIL;
	}

	if (!cJSON_IsString(ssid_json) || !cJSON_IsString(pwd_json)) {
		ESP_LOGE(TAG, "Invalid data type for 'c_ssid' or 'c_pwd'");
		cJSON_Delete(body_json);
		return ESP_FAIL;
	}


	// Update the WiFi networks configuration and let the WiFi applications know
	httpServer_tryToConnect(ssid_json->valuestring, pwd_json->valuestring);

	cJSON_Delete(body_json);
	
	return ESP_OK;
}



/**
 * wifiConnectStatus handler updates the connection status for the web page.
 * and handles receiving the SSID and password entered by the user
 * @param req HTTP request for which the uri needs to be handled.
 * @return ESP_OK
 */
static esp_err_t APP_URI_FUNCTION_HANDLER_NAME(wifi_connect_status_json)(httpd_req_t *req)
{	
	// char localJSONObjBuffer[BUFFER_MAX_SIZE] = {0};

	ESP_LOGI(TAG, "/wifiConnectStatus requested");
	
	memset(localJSONObjBuffer, 0, BUFFER_MAX_SIZE);
	
	sprintf(localJSONObjBuffer, "{\"wifi_connect_status_json\":%d}", httpServer_get_wifiConnectStatus());
	httpd_resp_set_type(req, "application/json");
	httpd_resp_send(req, localJSONObjBuffer, strlen(localJSONObjBuffer));
	
	return ESP_OK;
}


/**
 * wifiConnectInfo.json handler updates the web page with connection information.
 * and handles receiving the SSID and password entered by the user
 * @param req HTTP request for which the uri needs to be handled.
 * @return ESP_OK
 */
static esp_err_t APP_URI_FUNCTION_HANDLER_NAME(get_wifi_connect_info_json)(httpd_req_t *req)
{
	ESP_LOGI(TAG, "/wifiConnectInfo.json requested");
	
	char ipInfoJSON[200];
	memset(ipInfoJSON, 0x00, sizeof(ipInfoJSON));
	
    char ssid[33];	/**< SSID of AP */
	char ip[IP4ADDR_STRLEN_MAX];
	char netmask[IP4ADDR_STRLEN_MAX];
	char gateway[IP4ADDR_STRLEN_MAX];

	if (httpServer_get_wifiConnectStatus() == WIFI_STATUS_CONNECT_SUCCESS)
	{
		if (ESP_OK == wifiApp_getWifiConnectInfo(ssid, ip, netmask, gateway))
		{
			sprintf(ipInfoJSON, "{\"ip\":\"%s\",\"netmask\":\"%s\",\"gateway\":\"%s\",\"ap\":\"%s\"}", ip, netmask, gateway, ssid);
		}
	}
	
	httpd_resp_set_type(req, "application/json");
	httpd_resp_send(req, ipInfoJSON, strlen(ipInfoJSON));
	
	return ESP_OK;
}


/**
 * wifiDisconnect.json handler responds by sending a message to WiFi application to disconnect.
 * and handles receiving the SSID and password entered by the user
 * @param req HTTP request for which the uri needs to be handled.
 * @return ESP_OK
 */
static esp_err_t APP_URI_FUNCTION_HANDLER_NAME(wifi_disconnect_json)(httpd_req_t *req)
{
	ESP_LOGI(TAG, "/wifiDisconnect.json requested");
	
	wifiApp_sendMessage(WIFI_APP_USER_REQUESTED_STA_DISCONNECT);
	
	return ESP_OK;
}


static esp_err_t APP_URI_FUNCTION_HANDLER_NAME(get_ssid_list)(httpd_req_t *req)
{
    uint16_t count = 0;
	uint16_t i;
	char entry[160];
	wifiApp_ssidInfo_t * ssidList = wifiApp_getSsidList(&count);

	httpd_resp_set_type(req, "application/json");
    httpd_resp_sendstr_chunk(req, "[");

	for (i = 0; i < count; i++)
	{
        snprintf(entry, sizeof(entry),
                 "{\"ssid\":\"%s\",\"rssi\":%d}%s",
                 (char *)ssidList[i].ssid, ssidList[i].rssi,
                 (i < count-1) ? "," : "");
        httpd_resp_sendstr_chunk(req, entry);
    }

    httpd_resp_sendstr_chunk(req, "]");
    httpd_resp_sendstr_chunk(req, NULL);
    free(ssidList);

    return ESP_OK;
}


static esp_err_t APP_URI_FUNCTION_HANDLER_NAME(turn_valve_on_off)(httpd_req_t *req)
{
    // char localJSONObjBuffer[BUFFER_MAX_SIZE] = {0};
    size_t lenBodyJson = req->content_len;
    int valve_id = 0;

    ESP_LOGI(TAG, "/turnValveOnOff/<id> requested. Size: %d", lenBodyJson);

    // Procura pelo padrão no final da string da URI recebida
    if (sscanf(req->uri, "/turnValveOnOff/%d", &valve_id) == 1) {
        ESP_LOGI(TAG, "ID extraído da URL: %d", valve_id);
    } else {
        ESP_LOGE(TAG, "Não foi possível extrair o ID da URL: %s", req->uri);
        return ESP_FAIL;
    }
    
    // 1. Proteção contra estouro de buffer
    if (lenBodyJson >= BUFFER_MAX_SIZE) {
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "JSON body too large");
        return ESP_FAIL;
    }

    // 2. Receber o corpo da requisição
    // O retorno nos diz quantos bytes foram realmente lidos
    int ret = httpd_req_recv(req, localJSONObjBuffer, lenBodyJson);
    if (ret <= 0) {
        if (ret == HTTPD_SOCK_ERR_TIMEOUT) {
            httpd_resp_send_408(req);
        }
        return ESP_FAIL;
    }
    
    // Garante que a string está terminada em NULL para o cJSON não se perder
    localJSONObjBuffer[lenBodyJson] = '\0'; 

    // 3. Parsear o JSON
    cJSON *body_json = cJSON_Parse(localJSONObjBuffer);
    if (body_json == NULL) {
        ESP_LOGE(TAG, "Erro ao parsear o JSON");
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Invalid JSON format");
        return ESP_FAIL;
    }

    // 4. Extrair e validar o campo "valve"
    cJSON *valve_item = cJSON_GetObjectItemCaseSensitive(body_json, "valve");
    if (cJSON_IsString(valve_item) && (valve_item->valuestring != NULL)) {
        
        if (strcmp(valve_item->valuestring, "open") == 0) {
            ESP_LOGI(TAG, "Comando recebido: ABRIR válvula");
            // Fazer_Acao_Abrir_Valvula();
            irrigationDecisor_client(
                (uint8_t) valve_id,
                OPEN
            );
            
        } else if (strcmp(valve_item->valuestring, "close") == 0) {
            ESP_LOGI(TAG, "Comando recebido: FECHAR válvula");
            // Fazer_Acao_Fechar_Valvula();
            irrigationDecisor_client(
                (uint8_t) valve_id,
                CLOSE
            );
            
        } else {
            ESP_LOGW(TAG, "Comando desconhecido: %s", valve_item->valuestring);
        }
        
    } else {
        ESP_LOGE(TAG, "Chave 'valve' nao encontrada ou invalida");
        cJSON_Delete(body_json); // Sempre limpe o cJSON antes de sair!
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Missing 'valve' field");
        return ESP_FAIL;
    }

    // 5. SEMPRE limpe a memória alocada pelo cJSON
    cJSON_Delete(body_json);

    // 6. Responder ao cliente (obrigatório no protocolo HTTP)
    const char *resp_str = "{\"status\":\"success\"}";
    httpd_resp_set_type(req, "application/json");
    httpd_resp_sendstr(req, resp_str);

    return ESP_OK;
}


/**
 * A function that will make the uri's available to the server.
 */
static void router_uri_register(void)
{
	#define X(handler, route, method, ansType) \
		httpServer_uri_registerHandler(route, method, APP_URI_FUNCTION_HANDLER_NAME(handler));
		X_MACRO_API_ROUTES_LIST
	#undef X

    register_ws_route();
}