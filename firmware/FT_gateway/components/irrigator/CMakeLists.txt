idf_component_register(
    SRCS    "irrigator.c"
            "pipeWorker"
            "uart_sensorUmidity"
    INCLUDE_DIRS "include"
    REQUIRES    hardware
                projectConfig
)