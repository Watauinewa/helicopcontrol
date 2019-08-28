#ifndef MPU9250_API_H_
#define MPU9250_API_H_

#include "../i2c_api/i2c_api.h"
#include "../mpu9250/MPU9250.h"

#include <stdint.h>

class MPU9250_api: public MPU9250 {
    public:
        int status;
        
        MPU9250_api( I2c& i2c, const uint8_t address = 0x68);   // address = Direccion i2c del dispositivo. Puede ser 0x68 o 0x69 segun pin AD0
        int enableFifo(bool accel, bool gyro, bool mag, bool temp);
        
        void init();
        void calibrate();
        void calibrate_gyro();
        void calibrate_accel();
        void calibrate_mag();
        
        int readFifo();
        
        uint16_t fifoCount();
        void cleanfifo();
        int16_t* getrawdata();
        void getdata( 
            float &ax, float &ay, float &az,
            float &gx, float &gy, float &gz,
            float &hx, float &hy, float &hz
        );
        
    private:
        I2c *i2c;
        
        uint8_t buffer[64];
        int16_t *buffer16 = (int16_t*)buffer;
        uint16_t *buffer16u = (uint16_t*)buffer;
        
        bool _enFifoAccel, _enFifoGyro, _enFifoMag, _enFifoTemp;
        uint16_t _fifoFrameSize;
    
        float ax_offset, ay_offset, az_offset;
        float ax_gain, ay_gain, az_gain;
        
        float gx_offset, gy_offset, gz_offset;
        float gx_gain, gy_gain, gz_gain;
        
        float gx_tempcomp, gy_tempcomp, gz_tempcomp;
        float temperatureref;
        float temperaturecomp;
        
        float hx_offset, hy_offset, hz_offset;
        float hx_gain, hy_gain, hz_gain;
};

#endif
