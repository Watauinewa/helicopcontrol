#ifndef _BMP280_API
#define _BMP280_API

#include "../i2c_api/i2c_api.h"
#include "../bmp280/bmp280.h"

#include <stdint.h>

class BMP280_api {
    public:
        int8_t rslt;
        
        BMP280_api( I2c &i2c, const uint8_t address = 0x76);    // address = Direccion i2c del dispositivo. Puede ser 0x76 o 0x77 segun pin SDO  (GND = ‘0’, VDDIO = ‘1’)
        
        void init();
        void read();
        
        
    private:
        static I2c *i2c;   // Para leer y escribir por i2c ( Obligado a que sea static por como estan construidas las funciones "i2c_reg_write" y "i2c_reg_read" necesitadas por el driver bmp280.c)
        uint8_t address;   // Direccion i2c del dispositivo. Puede ser 0x76 o 0x77 segun pin SDO  (GND = ‘0’, VDDIO = ‘1’)
        
        static int8_t i2c_reg_write(uint8_t i2c_addr, uint8_t reg_addr, uint8_t *reg_data, uint16_t length);
        static int8_t i2c_reg_read(uint8_t i2c_addr, uint8_t reg_addr, uint8_t *reg_data, uint16_t length);
        
        struct bmp280_dev dev;
        struct bmp280_config conf;
        struct bmp280_uncomp_data ucomp_data;
        
    public:
        uint32_t pres32, pres64;
        double pres;
        
        int32_t temp_32;
        double temp_lf;
};

#endif
