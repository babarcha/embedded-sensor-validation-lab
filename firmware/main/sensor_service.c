#include "sensor_service.h"
#include <string.h>
esp_err_t sensor_service_init(sensor_service_t *s)
{
    if (!s)
        return ESP_ERR_INVALID_ARG;
    memset(s, 0, sizeof(*s));
    esp_err_t e = bmp280_init(&s->bmp);
    s->ready = (e == ESP_OK);
    return e;
}

esp_err_t sensor_service_read(
    sensor_service_t *s,
    int32_t *t,
    uint32_t *p)
{
    if (!s || !t || !p)
        return ESP_ERR_INVALID_ARG;

    /* Invalidate outputs before attempting acquisition. */
    *t = 0;
    *p = 0;

    if (s->inject_timeout)
        return ESP_ERR_TIMEOUT;

    if (!s->ready)
        return ESP_ERR_INVALID_STATE;

    int32_t new_temp = 0;
    uint32_t new_pressure = 0;

    esp_err_t e = bmp280_read(
        &s->bmp,
        &new_temp,
        &new_pressure);

    if (e != ESP_OK)
        return e;

    /* Validate the acquired sample. */
    if (new_temp < -4000 || new_temp > 8500 ||
        new_pressure < 30000 || new_pressure > 110000)
        return ESP_ERR_INVALID_RESPONSE;

    /* Publish outputs only after successful validation. */
    *t = new_temp;
    *p = new_pressure;

    return ESP_OK;
}
