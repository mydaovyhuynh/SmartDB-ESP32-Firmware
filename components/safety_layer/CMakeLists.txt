idf_component_register(
    SRCS "safety.c"
    INCLUDE_DIRS "."
    REQUIRES esp_driver_gpio freertos log
)
