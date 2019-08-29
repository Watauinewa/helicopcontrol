#include <stdlib.h>

// #include <i2c_t3.h>     // ***20190821 Modificado para evitar conflicoto con Wire.h
// #include <Wire.h>

#include "src/i2c_api/i2c_api.h"
#include "src/mpu9250_api/mpu9250_api.h"
#include "src/bmp280_api/bmp280_api.h"

uint32_t i = 0;

I2c i2c1(1);
MPU9250_api mpu9250( i2c1, 0x68);
BMP280_api bmp280( i2c1, 0x76);

void setup() {
    
    Serial.begin(115200);               // Baud rate. The Teensy always uses USB speeds, so this number doesn't matter.

    pinMode( 11, INPUT);                // Interrupcin compas
    pinMode( 12, INPUT);                // Interrupcin IMU
    pinMode( LED_BUILTIN, OUTPUT);      // LED sobre la placa del teensy (pin 13 en teensy 3.2)

    digitalWrite( LED_BUILTIN, HIGH);    // Encender LED on-board

    delay( 1000);

    Serial.printf("Iniciando teensy 3.2\n");

//     wire_cmd = 0;
//     wire_cmd_pending = 0;
//     wire_addr = 0;

    // leer registro WHOIAM del mpu9250
    uint8_t v = i2c1.readByte( 0x68, 0x75);    
    Serial.printf("valor leido %d\n", v);
    
    mpu9250.init();
    bmp280.init();
    
    // Wire - Setup for Slave mode, address 0x44, pins 18/19, external pullup, 400kHz
//     Wire.begin(I2C_SLAVE, 0x44, I2C_PINS_18_19, I2C_PULLUP_EXT, (uint32_t)400000);

    // Wire1 - Setup for Master mode, pins 29/30, external pullup, 400kHz
//     Wire1.begin( I2C_MASTER, 0x00, I2C_PINS_29_30, I2C_PULLUP_EXT, (uint32_t)400000);
//     Wire1.begin(I2C_MASTER, 0x00, I2C_PINS_29_30, I2C_PULLUP_INT, (uint32_t)400000);

    // Register Slave events
//     Wire.onReceive(receiveEvent);
//     Wire.onRequest(requestEvent);

    // Wire.resetBus(): this is used to try and reset the bus in cases of a hung Slave device (typically 
    // a Slave which is stuck outputting a low on SDA due to a lost clock). It will 
    // generate up to 9 clocks pulses on SCL in an attempt to get the Slave to release 
    // the SDA line. Once SDA is released it will restore I2C functionality. 
    // return: none
//     Wire1.resetBus();
//     delayMicroseconds( 50);

//     bmp280_api_init();
//     hmc5883l_init();
//     mpu6050_init();
// 
//     mpu6050_calibrate();
// 
// 
//     mpu6050_asetoffset( 128.6, -210.9, -1000.0);
//     mpu6050_asetgain( 1.0f, 1.0f, 1.0f);
//     mpu6050_gsetoffset( 645.87190, 12.1909, 0.5812);
//     mpu6050_gsetgain( 1.0f, 1.0f, 1.0f);		// gyro_scale = ( Sensibility). Sensibility puede ser 8,75e-3f a 245dps, 17.5e-3f a 500dps o 70.0e-3f a 2000dps
//     mpu6050_gsettempcomp(  0.0, 0.0, 0.0);
// 
//     hmc5883l_setoffset( 86.813938, -27.119864, -64.232921);
//     hmc5883l_setgain( 
//         0.900194, 0.006566, 0.011218,
//         0.006566, 0.916163, 0.006808,
//         0.011218, 0.006808, 1.127647);

//     mpu6050_cleanfifo();
}

int v = 1;
int status = 0;

void loop() {
    if(v) Serial.printf("si\n");
    v = 0;
    
    status = mpu9250.readFifo();
    if(status > 0) {
        float ax, ay, az, gx, gy, gz, hx, hy, hz;
        mpu9250.getdata(ax, ay, az, gx, gy, gz, hx, hy, hz);
        
        if(i % 1000 == 0) {
            Serial.printf("%f \t%f \t%f\n", ax, ay, az);
        }
        
        i++;
    }
}
