#ifndef _BMP280_API
#define _BMP280_API

#include "../i2c_api/i2c_api.h"
#include "../bmp280/bmp280.h"

#include "../math_tool/math_tool.h"

#include <stdint.h>

class BMP280_api {
    public:
        int8_t rslt;
        
        BMP280_api( I2c &i2c, const uint8_t address = 0x76);    // address = Direccion i2c del dispositivo. Puede ser 0x76 o 0x77 segun pin SDO  (GND = ‘0’, VDDIO = ‘1’)
        
        void init();
        void read_test();   // Prueba de lectura
        void read();
        
    private:
        static I2c *i2c;   // Para leer y escribir por i2c ( Obligado a que sea static por como estan construidas las funciones "i2c_reg_write" y "i2c_reg_read" necesitadas por el driver bmp280.c)
        uint8_t address;   // Direccion i2c del dispositivo. Puede ser 0x76 o 0x77 segun pin SDO  (GND = ‘0’, VDDIO = ‘1’)
        
        uint8_t buffer[6];
        int16_t *buffer16 = (int16_t*)buffer;
        uint16_t *buffer16u = (uint16_t*)buffer;
        
        static int8_t i2c_reg_write(uint8_t i2c_addr, uint8_t reg_addr, uint8_t *reg_data, uint16_t length);
        static int8_t i2c_reg_read(uint8_t i2c_addr, uint8_t reg_addr, uint8_t *reg_data, uint16_t length);
        
        struct bmp280_dev dev;
        
    public:
        struct bmp280_uncomp_data uncomp_data;
        uint32_t press_u32;
        double press_lf;
        
        int32_t temp_32;
        double temp_lf;
        
        // Obtener la temperatura siempre antes de la presion para que "t_fine" sea 
        // calculado (usado en el calculo de la presion)
        int32_t get_temperature_32();
        double get_temperature_lf();
        uint32_t get_pressure_32bit_u32();  // Version usando enteres de 32bits (es menos exacto que usando enteros de 64bits)
        uint32_t get_pressure_u32();        // Version usando enteros de 64bits (se debe dividir por 256)
        double get_pressure_lf();
        
        double get_alture_lf();
};

#endif
