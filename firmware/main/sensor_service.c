#include "sensor_service.h"
#include <string.h>
esp_err_t sensor_service_init(sensor_service_t *s) {
    if(!s)return ESP_ERR_INVALID_ARG;
    memset(s,0,sizeof(*s));
    esp_err_t e=bmp280_init(&s->bmp);
    s->ready=(e==ESP_OK);
    return e;
}
esp_err_t sensor_service_read(sensor_service_t *s,int32_t *t,uint32_t *p) {
    if(!s||!t||!p)return ESP_ERR_INVALID_ARG;
    if(s->inject_timeout)return ESP_ERR_TIMEOUT;
    if(!s->ready)return ESP_ERR_INVALID_STATE;
    esp_err_t e=bmp280_read(&s->bmp,t,p);
    if(e!=ESP_OK)return e;
    if(*t < -4000 || *t > 8500 || *p < 30000 || *p > 110000)
        return ESP_ERR_INVALID_RESPONSE;
    return ESP_OK;
}
