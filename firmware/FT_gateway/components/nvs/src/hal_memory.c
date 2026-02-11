#include "hal_memory.h"

static const char TAG[] = "memory";


void hal_memory_setup(mem_address_saver_t * strutcParams)
{
    ESP_LOGW(TAG, "LOADING...");
	if (ESP_OK != hal_memory_load(strutcParams)) { //Nenhuma configuração encontrada, criar default
		ESP_ERROR_CHECK(hal_memory_save(strutcParams));
		return;
	}
}

esp_err_t hal_memory_save(mem_address_saver_t * strutcParams)
{
    ESP_LOGW(TAG, "SALVANDO...");
    nvs_handle_t handle;
    esp_err_t err = nvs_open(strutcParams->namespace,
                             NVS_READWRITE,
                             &handle);

    if (err != ESP_OK)
    {
        ESP_LOGE("NVS", "Erro ao abrir NVS: %s", esp_err_to_name(err));
        return err;
    }

    err = nvs_set_blob( handle,
                        strutcParams->storage_key,
                        strutcParams->obj_addr,
                        strutcParams->obj_syze);

    if (err == ESP_OK)
    {
        err = nvs_commit(handle);
    }

    nvs_close(handle);
    return err;
}


esp_err_t hal_memory_load(mem_address_saver_t * strutcParams)
{
    nvs_handle_t handle;
    esp_err_t err = nvs_open(strutcParams->namespace,
                             NVS_READONLY,
                             &handle);
    if (err != ESP_OK) {
        ESP_LOGE("NVS", "Erro ao abrir NVS: %s", esp_err_to_name(err));
        return err;
    }

    ESP_LOGW("NVS", "Acessou NVS com sucesso");

    err = nvs_get_blob( handle,
                        strutcParams->storage_key,
                        strutcParams->obj_addr,
                        &(strutcParams->obj_syze));
    nvs_close(handle);
    return err;
}


esp_err_t hal_memory_erase(mem_address_saver_t * strutcParams)
{
    nvs_handle_t handle;
    esp_err_t err = nvs_open(strutcParams->namespace,
                             NVS_READWRITE,
                             &handle);

    if (err != ESP_OK) return err;

    err = nvs_erase_key(handle, strutcParams->storage_key);
    if (err == ESP_OK) {
        err = nvs_commit(handle);
    }

    nvs_close(handle);
    return err;
}

