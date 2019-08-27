#include "mpu9250_api.h"


MPU9250_api::MPU9250_api( I2c &i2c, const uint8_t address): 
    MPU9250(*(i2c.wire), address) 
{
    this->i2c = &i2c;
    
    ax_offset = 0;
    ay_offset = 0;
    az_offset = 0;
    ax_gain = 1.0f;
    ay_gain = 1.0f;
    az_gain = 1.0f;
    
    gx_offset = 0;
    gy_offset = 0;
    gz_offset = 0;
    gx_gain = 1.0f;
    gy_gain = 1.0f;
    gz_gain = 1.0f;
    
    gx_tempcomp = 0;
    gy_tempcomp = 0;
    gz_tempcomp = 0;
    temperatureref = 0;
    temperaturecomp = 0;
    
    hx_offset = 0;
    hy_offset = 0;
    hz_offset = 0;
    hx_gain = 1.0f;
    hy_gain = 1.0f;
    hz_gain = 1.0f;
}

// configures and enables the FIFO buffer 
int MPU9250_api::enableFifo(bool accel, bool gyro, bool mag, bool temp) {
    // use low speed SPI for register setting
    _useSPIHS = false;
    if(writeRegister(USER_CTRL, (0x40 | I2C_MST_EN)) < 0) {
        return -1;
    }
    if( writeRegister( FIFO_EN, (accel * FIFO_ACCEL) | (gyro * FIFO_GYRO) | (mag * FIFO_MAG) | (temp * FIFO_TEMP)) < 0) {
        return -2;
    }
    _enFifoAccel = accel;
    _enFifoGyro = gyro;
    _enFifoMag = mag;
    _enFifoTemp = temp;
    _fifoFrameSize = accel * 6 + gyro * 6 + mag * 7 + temp * 2;
    
    return 1;
}

void MPU9250_api::init() {
    status = this->begin();
    Serial.printf("status 1 = %d\n", status);
    
    status = this->setDlpfBandwidth(MPU9250::DLPF_BANDWIDTH_41HZ);
    Serial.printf("status 2 = %d\n", status);
    
    status = this->setSrd(1);
    Serial.printf("status 3 = %d\n", status);
    
    status = this->setAccelRange(MPU9250::ACCEL_RANGE_4G);
    Serial.printf("status 4 = %d\n", status);
    
    status = this->setGyroRange(MPU9250::GYRO_RANGE_2000DPS);
    Serial.printf("status 5 = %d\n", status);
    
    status = this->disableDataReadyInterrupt();
    Serial.printf("status 6 = %d\n", status);
    
    status = this->enableFifo(true, true, true, true);
    Serial.printf("status 7 = %d\n", status);
    
}

int MPU9250_api::readFifo() {
    uint16_t count = fifoCount();
    
    if( count == 1024) {
        Serial.printf("Error -- datos sobreescritos en IMU. FIFO = %d\n", count);
        cleanfifo();
        return 0;
    }
    
    if( count < _fifoFrameSize) return 0;
    
    //request register
    i2c->wire->beginTransmission( _address);
    i2c->wire->write( MPU9250::FIFO_READ);
    i2c->wire->endTransmission( false);
    
    //read data
    count = i2c->requestFrom( _address, _fifoFrameSize);
    
    // Acelerómetro
    buffer[1] = i2c->read();
    buffer[0] = i2c->read();
    buffer[3] = i2c->read();
    buffer[2] = i2c->read();
    buffer[5] = i2c->read();
    buffer[4] = i2c->read();

    // Datos termometro
    buffer[7] = i2c->read();
    buffer[6] = i2c->read();

    // Datos giroscopio
    buffer[9] = i2c->read();
    buffer[8] = i2c->read();
    buffer[11] = i2c->read();
    buffer[10] = i2c->read();
    buffer[13] = i2c->read();
    buffer[12] = i2c->read();
    
    // Datos magnetrometro
    buffer[14] = i2c->read();
    buffer[15] = i2c->read();
    buffer[16] = i2c->read();
    buffer[17] = i2c->read();
    buffer[18] = i2c->read();
    buffer[19] = i2c->read();
    
    return count;
}

// Retorna la cantidad de valores en el FIFO
uint16_t MPU9250_api::fifoCount() {
//     i2c->readBytes( _address, MPU9250::FIFO_COUNT, 2, buffer);
    
    //request register
    i2c->wire->beginTransmission( _address);
    i2c->wire->write( MPU9250::FIFO_COUNT);
    i2c->wire->endTransmission( false);
    
    //read data
    i2c->requestFrom( _address, 2);
    
    buffer[1] = i2c->read();
    buffer[0] = i2c->read();
    uint16_t count = buffer16u[0];
    
    return count;
}

void MPU9250_api::cleanfifo() {
    i2c->writeByte( _address, USER_CTRL, 0b00000000);		// Disable FIFO
    i2c->writeByte( _address, USER_CTRL, 0b00000100);		// Reset FIFO
    i2c->writeByte( _address, USER_CTRL, 0b01000000);		// Enable FIFO
}

void MPU9250_api::getdata( 
    float &ax, float &ay, float &az,
    float &gx, float &gy, float &gz
) {
    
//     _axcounts = (((int16_t)_buffer[0]) << 8) | _buffer[1];
//     _aycounts = (((int16_t)_buffer[2]) << 8) | _buffer[3];
//     _azcounts = (((int16_t)_buffer[4]) << 8) | _buffer[5];
//     _tcounts = (((int16_t)_buffer[6]) << 8) | _buffer[7];
//     _gxcounts = (((int16_t)_buffer[8]) << 8) | _buffer[9];
//     _gycounts = (((int16_t)_buffer[10]) << 8) | _buffer[11];
//     _gzcounts = (((int16_t)_buffer[12]) << 8) | _buffer[13];
//     _hxcounts = (((int16_t)_buffer[15]) << 8) | _buffer[14];
//     _hycounts = (((int16_t)_buffer[17]) << 8) | _buffer[16];
//     _hzcounts = (((int16_t)_buffer[19]) << 8) | _buffer[18];
//     
    // transform and convert to float values
//     _ax = (((float)(tX[0]*_axcounts + tX[1]*_aycounts + tX[2]*_azcounts) * _accelScale) - _axb)*_axs;
//     _ay = (((float)(tY[0]*_axcounts + tY[1]*_aycounts + tY[2]*_azcounts) * _accelScale) - _ayb)*_ays;
//     _az = (((float)(tZ[0]*_axcounts + tZ[1]*_aycounts + tZ[2]*_azcounts) * _accelScale) - _azb)*_azs;
//     _gx = ((float)(tX[0]*_gxcounts + tX[1]*_gycounts + tX[2]*_gzcounts) * _gyroScale) - _gxb;
//     _gy = ((float)(tY[0]*_gxcounts + tY[1]*_gycounts + tY[2]*_gzcounts) * _gyroScale) - _gyb;
//     _gz = ((float)(tZ[0]*_gxcounts + tZ[1]*_gycounts + tZ[2]*_gzcounts) * _gyroScale) - _gzb;
//     _hx = (((float)(_hxcounts) * _magScaleX) - _hxb)*_hxs;
//     _hy = (((float)(_hycounts) * _magScaleY) - _hyb)*_hys;
//     _hz = (((float)(_hzcounts) * _magScaleZ) - _hzb)*_hzs;
//     _t = ((((float) _tcounts) - _tempOffset)/_tempScale) + _tempOffset;
    
    ax = ((float)buffer16[0] - ax_offset) * ax_gain;
    ay = ((float)buffer16[1] - ay_offset) * ay_gain;
    az = ((float)buffer16[2] - az_offset) * az_gain;
    gx = ((float)buffer16[4] - gx_offset) * gx_gain;
    gy = ((float)buffer16[5] - gy_offset) * gx_gain;
    gz = ((float)buffer16[6] - gz_offset) * gz_gain;
}

