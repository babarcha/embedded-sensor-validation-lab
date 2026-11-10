#include <assert.h>
#include <stdio.h>
#include "sensor_service.h"

static esp_err_t init_result = ESP_OK;
static esp_err_t read_result = ESP_OK;
static int32_t fake_temp = 2345;
static uint32_t fake_pressure = 101325;

esp_err_t bmp280_init(bmp280_t *dev)
{
    (void)dev;
    return init_result;
}

esp_err_t bmp280_read(bmp280_t *dev, int32_t *t, uint32_t *p)
{
    (void)dev;
    if (read_result != ESP_OK)
        return read_result;
    *t = fake_temp;
    *p = fake_pressure;
    return ESP_OK;
}

int main(void)
{
    sensor_service_t s;
    int32_t t = 0;
    uint32_t p = 0;
    assert(sensor_service_init(NULL) == ESP_ERR_INVALID_ARG);
    assert(sensor_service_init(&s) == ESP_OK);
    assert(s.ready);
    assert(sensor_service_read(NULL, &t, &p) == ESP_ERR_INVALID_ARG);
    assert(sensor_service_read(&s, NULL, &p) == ESP_ERR_INVALID_ARG);
    assert(sensor_service_read(&s, &t, NULL) == ESP_ERR_INVALID_ARG);
    assert(sensor_service_read(&s, &t, &p) == ESP_OK);
    assert(t == 2345 && p == 101325);

    /*
     * Pass 3: Fault injection test.
     *
     * A failed sensor read must not leave stale
     * measurements in the output parameters.
     */
    s.inject_timeout = true;

    assert(sensor_service_read(&s, &t, &p) == ESP_ERR_TIMEOUT);

    /* Expected safe output values after failure */
    assert(t == 0);
    assert(p == 0);

    s.inject_timeout = false;

    read_result = ESP_ERR_TIMEOUT;
    assert(sensor_service_read(&s, &t, &p) == ESP_ERR_TIMEOUT);
    read_result = ESP_OK;

    fake_temp = 9000;
    assert(sensor_service_read(&s, &t, &p) == ESP_ERR_INVALID_RESPONSE);
    fake_temp = 2345;
    fake_pressure = 20000;
    assert(sensor_service_read(&s, &t, &p) == ESP_ERR_INVALID_RESPONSE);
    fake_pressure = 101325;

    init_result = ESP_FAIL;
    assert(sensor_service_init(&s) == ESP_FAIL);
    assert(!s.ready);
    assert(sensor_service_read(&s, &t, &p) == ESP_ERR_INVALID_STATE);
    puts("PASS: native sensor_service tests");
    return 0;
}
