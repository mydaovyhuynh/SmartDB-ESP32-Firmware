idf_component_register(
    SRCS "i2c_init.c" "ads1115.c"
    INCLUDE_DIRS "include"
    REQUIRES driver freetos esp_wifi esp_netif
    
)