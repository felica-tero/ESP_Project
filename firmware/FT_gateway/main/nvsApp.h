#ifndef MAIN_NVS_APP_H_
#define MAIN_NVS_APP_H_

#include "esp_err.h"

/**
 * @brief Saves station mode WiFI credentials to NVS
 * @return ESP_OK if successful
 */
esp_err_t nvs_app_save_sta_creds(void);

/**
 * @brief Loads the previously saved credentials from NVS
 * @return true if previously saved credentials were found
 */
bool nvs_app_load_sta_creds(void);

/**
 * @brief Clears station mode credentials from NVS
 * @return ESP_OK if successful
 */
esp_err_t nvs_app_clear_sta_creds(void);

#endif // MAIN_NVS_APP_H_