idf_component_register(
    SRC_DIRS "src"
    INCLUDE_DIRS "include"
    REQUIRES    driver
                nvs_flash
                esp_system
                esp_timer
                esp_event
                esp_wifi
                esp_http_server
                esp_http_client
                esp_netif
                app_update
                projectConfig
    EMBED_FILES	"webPage/app.css"
    			"webPage/app.js"
    			"webPage/app.html"
    			"webPage/dados.png"
    			"webPage/favicon.ico"
    			"webPage/historico.html"
    			"webPage/home.png"
    			"webPage/index.html"
    			"webPage/jquery-3.3.1.min.js"
    			"webPage/IrrigaAI.png"
)