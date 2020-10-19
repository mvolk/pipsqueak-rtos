idf_component_register(SRCS "psq4_temperature.c"
                       INCLUDE_DIRS "include"
                       REQUIRES "esp32-ds18b20"
                       PRIV_REQUIRES "esp32-owb")
