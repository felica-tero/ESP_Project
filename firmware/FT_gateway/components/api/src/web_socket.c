#include "web_socket.h"

#include <esp_http_server.h>
#include <esp_log.h>

#include "cJSON.h"

#include "httpServer.h"

static const char *TAG = "WS_SERVER";
static httpd_handle_t server = NULL;

// Callback chamado para cada mensagem recebida/handshake no /ws
static esp_err_t ws_handler(httpd_req_t *req)
{
    if (req->method == HTTP_GET) {
        ESP_LOGI(TAG, "Cliente WebSocket conectado. Handshake OK.");
        return ESP_OK;
    }

    httpd_ws_frame_t ws_pkt;
    uint8_t *buf = NULL;
    memset(&ws_pkt, 0, sizeof(httpd_ws_frame_t));
    ws_pkt.type = HTTPD_WS_TYPE_TEXT;

    // Obtém o tamanho da mensagem
    esp_err_t ret = httpd_ws_recv_frame(req, &ws_pkt, 0);
    if (ret != ESP_OK) return ret;

    if (ws_pkt.len) {
        buf = calloc(1, ws_pkt.len + 1);
        if (buf == NULL) return ESP_ERR_NO_MEM;
        ws_pkt.payload = buf;
        ret = httpd_ws_recv_frame(req, &ws_pkt, ws_pkt.len);
        if (ret == ESP_OK) {
            ESP_LOGI(TAG, "Mensagem recebida do cliente: %s", ws_pkt.payload);
        }
        free(buf);
    }
    return ret;
}

// Estrutura para passar dados na chamada assíncrona
typedef struct {
    int bomba_id;
    const char *status; // "open" ou "close"
} ws_event_arg_t;

// Executado no contexto de cada socket conectado
static void send_ws_message_to_client(httpd_handle_t hd, int fd, void *arg)
{
    ws_event_arg_t *data = (ws_event_arg_t *)arg;

    // Monta o JSON da mensagem
    cJSON *root = cJSON_CreateObject();
    cJSON_AddNumberToObject(root, "id", data->bomba_id);
    cJSON_AddStringToObject(root, "status", data->status);
    char *json_str = cJSON_PrintUnformatted(root);

    httpd_ws_frame_t ws_pkt;
    memset(&ws_pkt, 0, sizeof(httpd_ws_frame_t));
    ws_pkt.payload = (uint8_t *)json_str;
    ws_pkt.len = strlen(json_str);
    ws_pkt.type = HTTPD_WS_TYPE_TEXT;

    // Tenta enviar para o cliente conectado
    httpd_ws_send_frame_async(hd, fd, &ws_pkt);

    cJSON_Delete(root);
    free(json_str);
}

// Função pública para disparar o broadcast para TODOS os clientes conectados
void broadcast_bomba_status(int bomba_id, const char *status)
{
    ESP_LOGW(TAG, "broadcast_bomba_status");

    if (server == NULL) return;
    ESP_LOGW(TAG, "server present");

    ws_event_arg_t args = {
        .bomba_id = bomba_id,
        .status = status
    };

    // Filtra e chama a função para todos os clientes com o endpoint "/ws"
    httpd_queue_work(server, (httpd_work_fn_t)httpd_get_client_list, NULL); // Opcional se usar a iteração padrão:
    
    // Varre todas as conexões TCP ativas no HTTP Server
    size_t clients = 10;
    int client_fds[10];
    if (httpd_get_client_list(server, &clients, client_fds) == ESP_OK) {
        for (size_t i = 0; i < clients; i++) {
            if (httpd_ws_get_fd_info(server, client_fds[i]) == HTTPD_WS_CLIENT_WEBSOCKET) {
                send_ws_message_to_client(server, client_fds[i], &args);
            }
        }
    }
}

// Configuração do Endpoint no registro da rota do servidor
void register_ws_route(void)
{
    server = httpServer_uri_registerWebSocket(
        "/ws",
        HTTP_GET,
        ws_handler
    );
}