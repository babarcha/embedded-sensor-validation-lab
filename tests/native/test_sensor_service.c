#include <assert.h>
#include <stdio.h>
#include "sensor_service.h"

static esp_err_t init_result = ESP_OK;
static esp_err_t read_result = ESP_OK;
static int32_t fake_temp = 2345;
static uint32_t fake_pressure = 101325;
static bool inject_partial_write = false;

esp_err_t bmp280_init(bmp280_t *dev)
{
    (void)dev;
    return init_result;
}

esp_err_t bmp280_read(bmp280_t *dev, int32_t *t, uint32_t *p)
{
    (void)dev;

    if (inject_partial_write)
    {
        *t = 9999;
        return ESP_ERR_TIMEOUT;
    }

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

    /* Test 1: Invalid initialization argument */
    assert(sensor_service_init(NULL) == ESP_ERR_INVALID_ARG);

    /* Test 2: Successful sensor initialization */
    assert(sensor_service_init(&s) == ESP_OK);
    assert(s.ready);

    /* Test 3: Invalid read arguments */
    assert(sensor_service_read(NULL, &t, &p) == ESP_ERR_INVALID_ARG);
    assert(sensor_service_read(&s, NULL, &p) == ESP_ERR_INVALID_ARG);
    assert(sensor_service_read(&s, &t, NULL) == ESP_ERR_INVALID_ARG);

    /* Test 4: Successful sensor measurement */
    assert(sensor_service_read(&s, &t, &p) == ESP_OK);
    assert(t == 2345);
    assert(p == 101325);

    /* Test 5: Injected timeout must invalidate outputs */
    s.inject_timeout = true;

    assert(sensor_service_read(&s, &t, &p) == ESP_ERR_TIMEOUT);
    assert(t == 0);
    assert(p == 0);

    s.inject_timeout = false;

    /* Test 6: BMP280 driver timeout */
    t = 2345;
    p = 101325;
    read_result = ESP_ERR_TIMEOUT;

    assert(sensor_service_read(&s, &t, &p) == ESP_ERR_TIMEOUT);
    assert(t == 0);
    assert(p == 0);

    read_result = ESP_OK;

    /* Test 7: Partial sensor read failure */
    t = 2345;
    p = 101325;
    inject_partial_write = true;

    assert(sensor_service_read(&s, &t, &p) == ESP_ERR_TIMEOUT);
    assert(t == 0);
    assert(p == 0);

    inject_partial_write = false;

    /* Test 8: Recovery after driver failure */
    fake_temp = 2210;
    fake_pressure = 100850;

    assert(sensor_service_read(&s, &t, &p) == ESP_OK);
    assert(t == 2210);
    assert(p == 100850);

    /* Restore original mock measurements */
    fake_temp = 2345;
    fake_pressure = 101325;

    /* Test 9: Temperature outside valid range */
    fake_temp = 9000;

    assert(sensor_service_read(&s, &t, &p) == ESP_ERR_INVALID_RESPONSE);
    assert(t == 0);
    assert(p == 0);

    fake_temp = 2345;

    /* Test 10: Pressure outside valid range */
    fake_pressure = 20000;

    assert(sensor_service_read(&s, &t, &p) == ESP_ERR_INVALID_RESPONSE);
    assert(t == 0);
    assert(p == 0);

    fake_pressure = 101325;

    /* Test 11: Sensor initialization failure */
    init_result = ESP_FAIL;

    assert(sensor_service_init(&s) == ESP_FAIL);
    assert(!s.ready);

    /* Test 12: Read from uninitialized sensor */
    t = 2345;
    p = 101325;

    assert(sensor_service_read(&s, &t, &p) == ESP_ERR_INVALID_STATE);
    assert(t == 0);
    assert(p == 0);

    puts("PASS: native sensor_service tests");

    return 0;
}
