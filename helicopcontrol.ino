#include <stdlib.h>

// #include <i2c_t3.h>     // ***20190821 Modificado para evitar conflicoto con Wire.h
// #include <Wire.h>
#include <SPI.h>

#include "src/i2c_api/i2c_api.h"
#include "src/mpu9250_api/mpu9250_api.h"
#include "src/bmp280_api/bmp280_api.h"
#include "src/lora_server_api/lora_server_api.h"

#include "src/math_tool/math_tool.h"

#include <math.h>

uint32_t i = 0;

I2c i2c1(1);
MPU9250_api mpu9250( i2c1, 0x68);
BMP280_api bmp280( i2c1, 0x76);
Lora_server_api lora( SPI);

void setup() {
    Serial.begin(115200);               // Baud rate. The Teensy always uses USB speeds, so this number doesn't matter.
    
    SPI.setMOSI(7);
    SPI.setMISO(8);
    SPI.setSCK(14);

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
    
    lora.init();
    
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


    float x;
    uint32_t *y = (uint32_t *)&x;
    x = 0.0f;   Serial.printf("0x%08x = 0\n", *y);
    x = 1.0f;   Serial.printf("0x%08x = 1\n", *y);
    x = -1.0f;  Serial.printf("0x%08x = -1\n", *y);
    x = 0.5f;   Serial.printf("0x%08x = 0.5\n", *y);
    x = -0.5f;  Serial.printf("0x%08x = -0.5\n", *y);
    x = 2.0f;   Serial.printf("0x%08x = 2.0\n", *y);
    x = 4.0f;   Serial.printf("0x%08x = 4.0\n", *y);
    x = 8.0f;   Serial.printf("0x%08x = 8.0\n", *y);
    x = 16.0f;  Serial.printf("0x%08x = 16.0\n", *y);
    x = 1.543f; Serial.printf("0x%08x = 1.543\n", *y);
    x = 0.1f;   Serial.printf("0x%08x = %f\n", *y, x);
    x = 1.1f;   Serial.printf("0x%08x = %f\n", *y, x);
    
    x = 0.0;    Serial.printf("ln(%f) log = %lf   ln_64f = %lf   ln_32f = %f\n", x, log(x), ln_64f(x), ln_32f(x));    
    x = 1.0;    Serial.printf("ln(%f) log = %lf   ln_64f = %lf   ln_32f = %f\n", x, log(x), ln_64f(x), ln_32f(x));    
    x = 0.5;    Serial.printf("ln(%f) log = %lf   ln_64f = %lf   ln_32f = %f\n", x, log(x), ln_64f(x), ln_32f(x));    
    x = 1.5;    Serial.printf("ln(%f) log = %lf   ln_64f = %lf   ln_32f = %f\n", x, log(x), ln_64f(x), ln_32f(x));    
    x = 0.7422; Serial.printf("ln(%f) log = %lf   ln_64f = %lf   ln_32f = %f\n", x, log(x), ln_64f(x), ln_32f(x));    
    x = 42.56;  Serial.printf("ln(%f) log = %lf   ln_64f = %lf   ln_32f = %f\n", x, log(x), ln_64f(x), ln_32f(x));
    
    x = 0.0;    Serial.printf("exp(%f): exp = %lf   exp_64f = %lf   exp_32f = %f \n", x, exp(x), exp_64f(x), exp_32f(x));
    x = 1.0;    Serial.printf("exp(%f): exp = %lf   exp_64f = %lf   exp_32f = %f \n", x, exp(x), exp_64f(x), exp_32f(x));
    x = 0.5;    Serial.printf("exp(%f): exp = %lf   exp_64f = %lf   exp_32f = %f \n", x, exp(x), exp_64f(x), exp_32f(x));
    x = 1.5;    Serial.printf("exp(%f): exp = %lf   exp_64f = %lf   exp_32f = %f \n", x, exp(x), exp_64f(x), exp_32f(x));
    x = 0.7422; Serial.printf("exp(%f): exp = %lf   exp_64f = %lf   exp_32f = %f \n", x, exp(x), exp_64f(x), exp_32f(x));
    x = 42.56;  Serial.printf("exp(%f): exp = %lf   exp_64f = %lf   exp_32f = %f \n", x, exp(x), exp_64f(x), exp_32f(x));
    
    Serial.printf("pow(3.1, 2.0) = %lf \t%f \t%lf\n", pow_64f(3.1, 2.0), pow_32f(3.1, 2.0), pow(3.1, 2.0));
    Serial.printf("pow(-3.1, 2.0) = %lf \t%f \t%lf\n", pow_64f(-3.1, 2.0), pow_32f(-3.1, 2.0), pow(-3.1, 2.0));
    Serial.printf("pow(3.1, 3.0) = %lf \t%f \t%lf\n", pow_64f(3.1, 3.0), pow_32f(3.1, 3.0), pow(3.1, 3.0));
    Serial.printf("pow(-3.1, 3.0) = %lf \t%f \t%lf\n", pow_64f(-3.1, 3.0), pow_32f(-3.1, 3.0), pow(-3.1, 3.0));
    Serial.printf("pow(3.1, 2.7) = %lf \t%f \t%lf\n", pow_64f(3.1, 2.7), pow_32f(3.1, 2.7), pow(3.1, 2.7));
    Serial.printf("pow(-3.1, 2.7) = %lf \t%f \t%lf\n", pow_64f(-3.1, 2.7), pow_32f(-3.1, 2.7), pow(-3.1, 2.7));
    Serial.printf("pow(-3.1, -2.7) = %lf \t%f \t%lf\n", pow_64f(-3.1, -2.7), pow_32f(-3.1, -2.7), pow(-3.1, -2.7));
    Serial.printf("pow(3.1, 3.7) = %lf \t%f \t%lf\n", pow_64f(3.1, 3.7), pow_32f(3.1, 3.7), pow(3.1, 3.7));
    Serial.printf("pow(-3.1, 3.7) = %lf \t%f \t%lf\n", pow_64f(-3.1, 3.7), pow_32f(-3.1, 3.7), pow(-3.1, 3.7));
    
/*
    uint32_t t, t1, t2, t3;
    double r1, r2, r3;
    double alfa;
    float beta;
    
    r1 = 0;
    beta = -0.5;
    t = micros();
    for(i = 0; i < 10000; i++) {
        r1 += pow_32f( 3.7, beta);
        beta += 0.003;
    }
    t1 = micros() - t;
    
    r2 = 0;
    beta = -0.5;
    t = micros();
    for(i = 0; i < 10000; i++) {
        r2 += exp_32f( beta);
        beta += 0.003;
    }
    t2 = micros() - t;
    
    r3 = 0;
    beta = -0.5;
    t = micros();
    for(i = 0; i < 10000; i++) {
        r3 += ln_32f( beta);
        beta += 0.003;
        
//        Serial.printf("ln(%f): ln_32f = %f   ln_64f = %lf   log = %lf\n", beta, ln_32f(beta), ln_64f(beta), log(beta));
    }
    t3 = micros() - t;
    
    Serial.printf("%lf %lf %lf\n", r1, r2, r3);
    Serial.printf("tiempo 1 = %d\n", t1);
    Serial.printf("tiempo 2 = %d\n", t2);
    Serial.printf("tiempo 3 = %d\n", t3);
    
    */
}

int v = 1;
int status = 0;

void loop() {
    if(v) Serial.printf("si\n");
    v = 0;
    
    status = mpu9250.readFifo();
    if(status > 0) {
        mpu9250.getdata();
        
        if(i % 1000 == 0) {
            bmp280.read();
            
            bmp280.calc_temperature_32();
            bmp280.calc_pressure_32f();
            bmp280.calc_altitude_32f();
            
//             bmp280.calc_pressure_64f();
//             bmp280.calc_altitude_64f();

//             Serial.printf( "UT: %d, \tUP: %d\n", bmp280.uncomp_data.uncomp_temp, bmp280.uncomp_data.uncomp_press);
//             Serial.printf("%f \t%f \t%f \t", mpu9250.ax, mpu9250.ay, mpu9250.az);
//             Serial.printf("%f \t%f \t", bmp280.temp_64f, bmp280.press_64f);
//             Serial.printf("%d \t%f \t%lf \t", bmp280.temp_32, bmp280.temp_32f, bmp280.temp_64f);
            Serial.printf("H = %f \t", bmp280.h_32f);
//             Serial.printf("H_64f = %f \t", bmp280.h_64f);

//             Serial.printf(
//                 "---- UP: %ld, P32: %ld, P64: %ld, P64N: %ld, P: %f %f \t",
//                 bmp280.uncomp_data.uncomp_press,
//                 bmp280.calc_pressure_32bit_u32(),
//                 bmp280.calc_pressure_u32(),
//                 bmp280.calc_pressure_u32() / 256,
//                 bmp280.calc_pressure_32f(),
//                 bmp280.calc_pressure_64f()
//             );
            
            Serial.printf("\n");
        }
        i++;
    }
}
