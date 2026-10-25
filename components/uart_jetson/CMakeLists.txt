idf_component_register(
    SRCS "uart_jetson.c"
    INCLUDE_DIRS "include"
    REQUIRES driver freertos
)