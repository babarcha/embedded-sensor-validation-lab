#include "bmp280.h"
#include "driver/i2c.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <string.h>
#define PORT I2C_NUM_0
#define TIMEOUT pdMS_TO_TICKS(100)
static esp_err_t rd(uint8_t addr, uint8_t reg, uint8_t *out, size_t n) {
    return i2c_master_write_read_device(PORT, addr, &reg, 1, out, n, TIMEOUT);
}
static esp_err_t wr(uint8_t addr, uint8_t reg, uint8_t val) {
    uint8_t b[2]={reg,val};
    return i2c_master_write_to_device(PORT,addr,b,2,TIMEOUT);
}
static uint16_t u16(const uint8_t *p) { return (uint16_t)p[0] | ((uint16_t)p[1]<<8); }
esp_err_t bmp280_init(bmp280_t *d) {
    if (!d) return ESP_ERR_INVALID_ARG;
    memset(d,0,sizeof(*d));
    i2c_config_t cfg={.mode=I2C_MODE_MASTER,.sda_io_num=21,.scl_io_num=22,
        .sda_pullup_en=GPIO_PULLUP_ENABLE,.scl_pullup_en=GPIO_PULLUP_ENABLE,
        .master.clk_speed=100000};
    esp_err_t e=i2c_param_config(PORT,&cfg); if(e!=ESP_OK) return e;
    e=i2c_driver_install(PORT,cfg.mode,0,0,0); if(e!=ESP_OK && e!=ESP_ERR_INVALID_STATE) return e;
    uint8_t id=0;
    for(uint8_t a=0x76;a<=0x77;a++) {
        if(rd(a,0xD0,&id,1)==ESP_OK && id==0x58) { d->addr=a; break; }
    }
    if(!d->addr) return ESP_ERR_NOT_FOUND;
    uint8_t c[24]; e=rd(d->addr,0x88,c,sizeof(c)); if(e!=ESP_OK) return e;
    d->t1=u16(c);d->t2=(int16_t)u16(c+2);d->t3=(int16_t)u16(c+4);
    d->p1=u16(c+6);d->p2=(int16_t)u16(c+8);d->p3=(int16_t)u16(c+10);
    d->p4=(int16_t)u16(c+12);d->p5=(int16_t)u16(c+14);d->p6=(int16_t)u16(c+16);
    d->p7=(int16_t)u16(c+18);d->p8=(int16_t)u16(c+20);d->p9=(int16_t)u16(c+22);
    if(!d->t1 || !d->p1) return ESP_ERR_INVALID_RESPONSE;
    // Temperature and pressure oversampling x1, normal mode.
    e=wr(d->addr,0xF4,0x27); if(e!=ESP_OK) return e;
    // Standby 1000 ms, filter off.
    return wr(d->addr,0xF5,0xA0);
}
esp_err_t bmp280_read(bmp280_t *d,int32_t *t,uint32_t *p) {
    if(!d||!t||!p||!d->addr) return ESP_ERR_INVALID_ARG;
    uint8_t b[6];esp_err_t e=rd(d->addr,0xF7,b,6);if(e!=ESP_OK)return e;
    int32_t ap=((int32_t)b[0]<<12)|((int32_t)b[1]<<4)|(b[2]>>4);
    int32_t at=((int32_t)b[3]<<12)|((int32_t)b[4]<<4)|(b[5]>>4);
    if(ap==0x80000||at==0x80000) return ESP_ERR_INVALID_RESPONSE;
    int32_t v1=((((at>>3)-((int32_t)d->t1<<1)))*((int32_t)d->t2))>>11;
    int32_t v2=(((((at>>4)-(int32_t)d->t1)*((at>>4)-(int32_t)d->t1))>>12)*((int32_t)d->t3))>>14;
    d->t_fine=v1+v2;
    *t=(d->t_fine*5+128)>>8;
    // Bosch BMP280 64-bit integer pressure compensation algorithm.
    int64_t x=(int64_t)d->t_fine-128000;
    int64_t y=x*x*(int64_t)d->p6;
    y=y+((x*(int64_t)d->p5)<<17);
    y=y+(((int64_t)d->p4)<<35);
    x=((x*x*(int64_t)d->p3)>>8)+((x*(int64_t)d->p2)<<12);
    x=(((((int64_t)1)<<47)+x)*((int64_t)d->p1))>>33;
    if(x==0) return ESP_ERR_INVALID_RESPONSE;
    int64_t q=1048576-ap;
    q=(((q<<31)-y)*3125)/x;
    x=(((int64_t)d->p9)*(q>>13)*(q>>13))>>25;
    y=(((int64_t)d->p8)*q)>>19;
    q=((q+x+y)>>8)+(((int64_t)d->p7)<<4);
    if(q<0 || (q>>8)>UINT32_MAX) return ESP_ERR_INVALID_RESPONSE;
    *p=(uint32_t)(q>>8);
    return ESP_OK;
}
