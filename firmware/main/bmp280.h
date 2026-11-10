#pragma once
#include "esp_err.h"
#include <stdint.h>
typedef struct {
    uint16_t t1; int16_t t2,t3;
    uint16_t p1; int16_t p2,p3,p4,p5,p6,p7,p8,p9;
    int32_t t_fine;
    uint8_t addr;
} bmp280_t;
esp_err_t bmp280_init(bmp280_t *dev);
esp_err_t bmp280_read(bmp280_t *dev, int32_t *temp_cdeg, uint32_t *pressure_pa);
