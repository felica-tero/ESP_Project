# See the build system documentation in IDF programming guide
# for more information about component CMakeLists.txt files.

idf_component_register(
    SRC_DIRS "src"
    INCLUDE_DIRS "include"
    REQUIRES    projectConfig
				personal_hal
				driver
                nvs_flash
                esp_system
                esp_timer
                esp_event
                esp_wifi
                esp_http_server
                esp_http_client
                esp_netif
                app_update
				json
    EMBED_FILES	"webPage/app.css"
    			"webPage/app.js"
    			"webPage/favicon.ico"
    			"webPage/index.html"
    			"webPage/jquery-3.3.1.min.js"
)
