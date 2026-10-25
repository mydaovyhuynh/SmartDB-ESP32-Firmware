idf_component_register(
    SRCS "main.c"
    INCLUDE_DIRS "."
    REQUIRES nvs_flash wifi sensors safety_layer telemetry node_fsm thermal uart_jetson
)
