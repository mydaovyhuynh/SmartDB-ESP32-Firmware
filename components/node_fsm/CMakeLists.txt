idf_component_register(
    SRCS "node_fsm.c"
    INCLUDE_DIRS "include"
    REQUIRES freertos wifi_manager
)