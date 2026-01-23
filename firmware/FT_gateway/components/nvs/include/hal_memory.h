#pragma once

#include "defines.h"

#include "esp_timer.h"
#include "esp_attr.h"
#include "stdio.h"
#include "nvs_flash.h"
#include "esp_check.h"
#include "esp_log.h"
#include "esp_err.h"

typedef struct mem_address_saver_s
{
    void *  obj_addr;
    size_t obj_syze;
    char * namespace;
    char * storage_key;
} mem_address_saver_t;

void hal_memory_setup(mem_address_saver_t * strutcParams);
esp_err_t hal_memory_save(mem_address_saver_t * strutcParams);
esp_err_t hal_memory_load(mem_address_saver_t * strutcParams);
esp_err_t hal_memory_erase(mem_address_saver_t * strutcParams);