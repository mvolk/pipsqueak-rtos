idf_component_register(SRCS "temperatureSensor.c"
                       INCLUDE_DIRS "include"
                       REQUIRES "esp32-ds18b20"
                       PRIV_REQUIRES "esp32-owb")
