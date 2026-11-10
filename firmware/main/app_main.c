#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sensor_service.h"
static sensor_service_t sensor;
static void emit_read(bool temperature) {
    int32_t t=0;uint32_t p=0;
    esp_err_t e=sensor_service_read(&sensor,&t,&p);
    if(e!=ESP_OK) {
        printf("STATUS=ERROR;ERROR=%s\n",e==ESP_ERR_TIMEOUT?"TIMEOUT":
            e==ESP_ERR_INVALID_STATE?"NOT_READY":
            e==ESP_ERR_INVALID_RESPONSE?"INVALID_SAMPLE":"BUS_ERROR");
        return;
    }
    if(temperature) printf("TEMP_C=%" PRId32 ".%02" PRId32 ";STATUS=OK\n",
        t/100,(t<0?-t:t)%100);
    else printf("PRESSURE_HPA=%" PRIu32 ".%02" PRIu32 ";STATUS=OK\n",
        p/100,p%100);
}
static void command(const char *s) {
    if(!strcmp(s,"PING")) puts("PONG");
    else if(!strcmp(s,"GET_INFO")) puts("MODEL=ESP32-SENSOR;FW=1.0.0");
    else if(!strcmp(s,"GET_STATUS")) printf("SENSOR=%s;FAULT=%s\n",
        sensor.ready?"READY":"NOT_READY",sensor.inject_timeout?"TIMEOUT":"NONE");
    else if(!strcmp(s,"READ_TEMP")) emit_read(true);
    else if(!strcmp(s,"READ_PRESSURE")) emit_read(false);
    else if(!strcmp(s,"INJECT_FAULT=TIMEOUT")) {sensor.inject_timeout=true;puts("OK");}
    else if(!strcmp(s,"CLEAR_FAULT")) {sensor.inject_timeout=false;puts("OK");}
    else puts("STATUS=ERROR;ERROR=UNKNOWN_COMMAND");
    fflush(stdout);
}
void app_main(void) {
    esp_err_t e=sensor_service_init(&sensor);
    printf("BOOT;SENSOR=%s\n",e==ESP_OK?"READY":"NOT_READY");
    vTaskDelay(pdMS_TO_TICKS(100));
    char buf[96];size_t n=0;
    while(1) {
        int c=getchar();
        if(c==EOF) {vTaskDelay(pdMS_TO_TICKS(10));continue;}
        if(c=='\r'||c=='\n') {if(n){buf[n]=0;command(buf);n=0;}}
        else if(n<sizeof(buf)-1) buf[n++]=(char)c;
        else {n=0;puts("STATUS=ERROR;ERROR=COMMAND_TOO_LONG");fflush(stdout);}
    }
}
