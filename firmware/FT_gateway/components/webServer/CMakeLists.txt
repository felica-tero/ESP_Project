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
    			"webPage/app.html"
    			"webPage/dados.png"
    			"webPage/favicon.ico"
    			"webPage/historico.html"
    			"webPage/historico.png"
    			"webPage/home.png"
    			"webPage/index.html"
    			"webPage/jquery-3.3.1.min.js"
    			"webPage/IrrigaAI.png"
    			"webPage/tempo/arrow-max.svg"
    			"webPage/tempo/arrow-min.svg"
    			"webPage/tempo/irrigation.svg"
    			"webPage/tempo/umidade.svg"
    			"webPage/tempo/animated/1.svg"
    			"webPage/tempo/animated/2.svg"
    			"webPage/tempo/animated/3.svg"
    			"webPage/tempo/animated/4.svg"
    			"webPage/tempo/animated/5.svg"
    			"webPage/tempo/animated/6.svg"
    			"webPage/tempo/animated/7.svg"
    			"webPage/tempo/animated/8.svg"
    			"webPage/tempo/animated/9.svg"
    			"webPage/tempo/animated/10.svg"
    			"webPage/tempo/animated/11.svg"
    			"webPage/tempo/animated/12.svg"
    			"webPage/tempo/animated/13.svg"
    			"webPage/tempo/animated/weather_sunset.svg"
)
