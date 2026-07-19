idf_component_register(
    SRC_DIRS "src"
    INCLUDE_DIRS "include"
    REQUIRES    personal_hal
                projectConfig
                webServer
                irrigator
                esp_http_server
)