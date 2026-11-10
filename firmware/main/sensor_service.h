#pragma once
#include "bmp280.h"
#include <stdbool.h>
typedef struct { bmp280_t bmp; bool ready; bool inject_timeout; } sensor_service_t;
esp_err_t sensor_service_init(sensor_service_t *s);
esp_err_t sensor_service_read(sensor_service_t *s,int32_t *temp_cdeg,uint32_t *pressure_pa);
