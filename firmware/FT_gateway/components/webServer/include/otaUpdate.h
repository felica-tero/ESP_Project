
#ifndef __MAIN_OTA_UPDATE_H__
#define __MAIN_OTA_UPDATE_H__

#include "esp_ota_ops.h"
#include "sys/param.h"

#define OTA_UPDATE_PENDING 		0
#define OTA_UPDATE_SUCCESSFUL	1
#define OTA_UPDATE_FAILED		-1

/// Firmware update status
extern int g_fw_update_status;

/**
 * Checks the g_fw_update_status and creates the fw_update_reset timer if g_fw_update_status is true.
 */
void ota_fw_update_reset_timer(void);

/**
 * @brief Timer callback function which calls esp_restart upon successful firmware update.
 * 
 * @param arg 
 */
void ota_fw_update_reset_callback(void *arg);

// Função auxiliar: inicia OTA
esp_err_t ota_begin_partition(const esp_partition_t *partition, esp_ota_handle_t *handle);

// Função auxiliar: escreve dados OTA
esp_err_t ota_write_data(esp_ota_handle_t handle, const char *data, int len);

// Função auxiliar: finaliza OTA e define partição de boot
bool ota_finalize_and_set_boot(esp_ota_handle_t handle, const esp_partition_t *partition);

// Função auxiliar: processa o primeiro bloco do corpo da requisição
esp_err_t ota_process_first_chunk(
    const char *ota_buff, int recv_len, int *content_received,
    esp_ota_handle_t *ota_handle, const esp_partition_t *update_partition);

// Função auxiliar: processa os próximos blocos do corpo da requisição
esp_err_t ota_process_next_chunk(
    const char *ota_buff, int recv_len, int *content_received,
    esp_ota_handle_t ota_handle);

// Função auxiliar: atualiza status global de OTA
void ota_update_status(bool flash_successful);

#endif // __MAIN_OTA_UPDATE_H__