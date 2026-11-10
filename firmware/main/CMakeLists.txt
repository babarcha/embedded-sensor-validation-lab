idf_component_register(SRCS "app_main.c" "bmp280.c" "sensor_service.c"
                       INCLUDE_DIRS "."
                       REQUIRES driver esp_timer)
