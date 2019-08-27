#include "i2c_api.h"

I2c::I2c(uint8_t num_dispositivo) {
    init( num_dispositivo);
}

void I2c::init(uint8_t num_dispositivo) {
    // El teensy 3.2 tiene dos puertos I2C. Accesible por los objetos "Wire" y "Wire1" de la libreria Wire.h
    // "Wire" para pines 16/17 o 18/19
    // "Wire1" para pines 29/30
    // Existe la libreria <i2c_t3.h> ( https://github.com/nox771/i2c_t3), pero no puede usarse junto con Wire.h (ver https://forum.pjrc.com/threads/21680-New-I2C-library-for-Teensy3/page26)
    // La libreria <i2c_t3.h> viene incluida en teensyduino ( Chequear si es la ultima version con respecto a la de github)

    switch(num_dispositivo) {
        case 0:
            clock = 400000;
            pin_scl = 16;
            pin_sda = 17;
            wire = &Wire;
            break;
        case 1:
            clock = 400000;
            pin_scl = 29;           // Pin 29 como SCL
            pin_sda = 30;           // Pin 30 como SDA
            wire = &Wire1;
            break;
        default:
            wire = NULL;
            return;
    }    
    
    wire->begin();
    wire->setClock( clock);
    wire->setSCL( pin_scl);
    wire->setSDA( pin_sda);
}

uint8_t I2c::readByte( uint8_t address, uint8_t reg) {
    wire->beginTransmission( address);
    wire->write( reg);
    wire->endTransmission( false);
    
    int count = wire->requestFrom( address, (uint8_t)1);
    uint8_t v = 0x00;
    if( count == 1) {
        v = wire->read();
    }
    
    return v;
}

int8_t I2c::readBytes(uint8_t address, uint8_t reg, uint8_t len, uint8_t *data) {
	uint8_t i = 0;
	int8_t count = 0;
    
    //request register
    wire->beginTransmission( address);
    wire->write( reg);
    wire->endTransmission( false);
    
    //read data
    count = wire->requestFrom( address, len);
    
    for( i = 0; i < count; i++) {
        data[i] = wire->read();
    }
    
	return count;
}

void I2c::writeBytes(uint8_t address, uint8_t reg, uint8_t len, uint8_t *data) {
    if(len > 0) {
        //write data
        wire->beginTransmission( address);
        wire->write( reg);
        
        for (uint8_t i = 0; i < len; i++) {
            wire->write((uint8_t) data[i]);
        }
        wire->endTransmission( true);
    }
}



