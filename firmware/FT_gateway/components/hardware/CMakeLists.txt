idf_component_register(
    SRCS    "hal_gpio.c"
    INCLUDE_DIRS "include"
    REQUIRES    esp_driver_gpio
                projectConfig
)