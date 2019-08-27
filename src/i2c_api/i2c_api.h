#ifndef _I2C_API
#define _I2C_API

#include <Wire.h>

// Utilizado para comunicación con dispositivos conectados por i2c
class I2c {
    public:
        I2c( uint8_t num_dispositivo);      // Puede ser 0 o 1 pues Teensy 3.2 tiene 2 puertos i2c
        void init( uint8_t num_dispositivo);     // Inicializa interfaz i2c
        
        uint8_t readByte( uint8_t address, uint8_t reg);
        int8_t readBytes( uint8_t address, uint8_t reg, uint8_t len, uint8_t *data);
        void writeBytes( uint8_t address, uint8_t reg, uint8_t len, uint8_t *data);
        void writeByte( uint8_t address, uint8_t reg, uint8_t data) { writeBytes(address, reg, 1, &data); }
        
        uint8_t requestFrom( const uint8_t &address, const uint8_t &len) { return wire->requestFrom( address, len); }
        int read() { return wire->read(); }
        
    private:
        uint32_t clock;                 // Reloj de velocidad de comunicacion
        uint8_t pin_scl;                // Indica pin usado como scl
        uint8_t pin_sda;                // Indica pin usado como sda
    
    public:
        TwoWire *wire = NULL;           // Interfaz i2c (desde Wire.h)
};

#endif
