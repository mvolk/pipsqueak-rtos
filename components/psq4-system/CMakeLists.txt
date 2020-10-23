idf_component_register(SRCS "psq4_thermometers.c" "psq4_system.c" "psq4_time.c" "psq4_wifi.c"
                       INCLUDE_DIRS "include"
                       PRIV_REQUIRES "esp32-ds3231" "nvs_flash" "esp_event" "esp32-ds18b20" "esp32-owb")
