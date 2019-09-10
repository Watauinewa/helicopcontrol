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
//     status = this->begin();
//     Serial.printf("status 1 = %d\n", status);
    
    // select clock source to gyro
    status = writeRegister(PWR_MGMNT_1, CLOCK_SEL_PLL);
    Serial.printf("status 1.1 = %d\n", status);
    
    // enable I2C master mode
    status = writeRegister(USER_CTRL, I2C_MST_EN);
    Serial.printf("status 1.2 = %d\n", status);
    
    // set the I2C bus speed to 400 kHz
    status = writeRegister(I2C_MST_CTRL, I2C_MST_CLK);
    Serial.printf("status 1.3 = %d\n", status);
    
    // set AK8963 to Power Down
    writeAK8963Register(AK8963_CNTL1, AK8963_PWR_DOWN);
    
    // reset the MPU9250
    writeRegister(PWR_MGMNT_1, PWR_RESET);
    
    // wait for MPU-9250 to come back up
    delay(1);
    
    // reset the AK8963
    writeAK8963Register(AK8963_CNTL2, AK8963_RESET);
    
    // select clock source to gyro
    status = writeRegister(PWR_MGMNT_1, CLOCK_SEL_PLL);
    Serial.printf("status 1.4 = %d\n", status);
    
    // check the WHO AM I byte, expected value is 0x71 (decimal 113) or 0x73 (decimal 115)
    if((whoAmI() != 113) && (whoAmI() != 115)) {
        Serial.printf("registro whoami no coincide\n");
        return;
    }
    
    // enable accelerometer and gyro
    status = writeRegister(PWR_MGMNT_2, SEN_ENABLE);
    Serial.printf("status 1.5 = %d\n", status);
    
    
    status = this->setAccelRange(MPU9250::ACCEL_RANGE_4G);
    Serial.printf("status 2 = %d\n", status);
    
    status = this->setGyroRange(MPU9250::GYRO_RANGE_2000DPS);
    Serial.printf("status 3 = %d\n", status);
    
    status = this->setDlpfBandwidth(MPU9250::DLPF_BANDWIDTH_41HZ);
    Serial.printf("status 4 = %d\n", status);
    
    // setting the sample rate divider to 0 as default
    status = writeRegister(SMPDIV, 0x00);
    _srd = 0;
//     status = this->setSrd(0); // Se cae al hacer AK8963_CNTL1 = AK8963_CNT_MEAS2 si se ehecuta esta funcion aquí
    Serial.printf("status 5 = %d\n", status);

    // enable I2C master mode
    status = writeRegister(USER_CTRL, I2C_MST_EN);
    Serial.printf("status 5.1 = %d\n", status);
    
    // set the I2C bus speed to 400 kHz
    status =  writeRegister(I2C_MST_CTRL, I2C_MST_CLK);
    Serial.printf("status 5.2 = %d\n", status);
    
    // check AK8963 WHO AM I register, expected value is 0x48 (decimal 72)
    if( whoAmIAK8963() != 72 ) {
        Serial.printf("registro whiamI magnotrometro no coincide\n");
        return;
    }
    
    // get the magnetometer calibration
    // set AK8963 to Power Down
    status = writeAK8963Register(AK8963_CNTL1, AK8963_PWR_DOWN);
    Serial.printf("status 5.3 = %d\n", status);
    delay(100); // long wait between AK8963 mode changes
    
    // set AK8963 to FUSE ROM access
    status = writeAK8963Register(AK8963_CNTL1,AK8963_FUSE_ROM);
    Serial.printf("status 5.4 = %d\n", status);
    delay(100); // long wait between AK8963 mode changes
    
    // read the AK8963 ASA registers and compute magnetometer scale factors
    readAK8963Registers(AK8963_ASA,3,_buffer);
    _magScaleX = ((((float)_buffer[0]) - 128.0f)/(256.0f) + 1.0f) * 4912.0f / 32760.0f; // micro Tesla
    _magScaleY = ((((float)_buffer[1]) - 128.0f)/(256.0f) + 1.0f) * 4912.0f / 32760.0f; // micro Tesla
    _magScaleZ = ((((float)_buffer[2]) - 128.0f)/(256.0f) + 1.0f) * 4912.0f / 32760.0f; // micro Tesla
    
    // set AK8963 to Power Down
    status = writeAK8963Register(AK8963_CNTL1,AK8963_PWR_DOWN);
    Serial.printf("status 5.5 = %d\n", status);
    delay(100); // long wait between AK8963 mode changes
    
    // set AK8963 to 16 bit resolution, 100 Hz update rate
    status = writeAK8963Register(AK8963_CNTL1, AK8963_CNT_MEAS2);
    Serial.printf("status 5.6 = %d\n", status);
    delay(100); // long wait between AK8963 mode changes
    
    // select clock source to gyro
    status = writeRegister(PWR_MGMNT_1, CLOCK_SEL_PLL);
    Serial.printf("status 5.7 = %d\n", status);
    
    // instruct the MPU9250 to get 7 bytes of data from the AK8963 at the sample rate
    readAK8963Registers(AK8963_HXL, 7, _buffer);
    
    status = this->disableDataReadyInterrupt();
    Serial.printf("status 6 = %d\n", status);
    
    status = this->enableFifo(true, true, true, true);
    Serial.printf("status 7 = %d\n", status);
}

uint16_t MPU9250_api::readFifo() {
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

int16_t* MPU9250_api::getrawdata() {
    return buffer16;
}

void MPU9250_api::getdata() {
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
    
    hx = ((float)buffer16[7] - hx_offset) * hx_gain;
    hy = ((float)buffer16[8] - hy_offset) * hy_gain;
    hz = ((float)buffer16[9] - hz_offset) * hz_gain;
}

void MPU9250_api::getdata( 
    float &ax, float &ay, float &az,
    float &gx, float &gy, float &gz,
    float &hx, float &hy, float &hz
) {
    this->getdata();
    
    ax = this->ax;
    ay = this->ay;
    az = this->az;
    
    gx = this->gx;
    gy = this->gy;
    gz = this->gz;
    
    hx = this->hx;
    hy = this->hy;
    hz = this->hz;
}

void MPU9250_api::calibrate_gyro() {
    GyroRange gyroRange = _gyroRange;
    DlpfBandwidth bandwidth = _bandwidth;
    uint8_t srd = _srd;
    
    uint32_t i;
    
    // set the range, bandwidth, and srd
    if( setGyroRange(GYRO_RANGE_250DPS) < 0) {
        Serial.printf("error - calibrate_gyro\n");
        return;
    }
    
    if (setDlpfBandwidth(DLPF_BANDWIDTH_20HZ) < 0) {
        Serial.printf("error - calibrate_gyro\n");
        return;
    }
    
    if (setSrd(19) < 0) {
        Serial.printf("error - calibrate_gyro\n");
        return;
    }
    
    cleanfifo();
    
    // Calentamiento
    for( i = 0; i < 50; i++) {
        readFifo();
        delayMicroseconds(500);
    }

    // take samples and find bias
    double gxbD = 0;
    double gybD = 0;
    double gzbD = 0;
    
    uint32_t numSamples = 100;
    for( i = 0; i < numSamples; ) {
        if( readFifo() > 0) {
            readSensor();
            gxbD += getGyroX_rads() / ((double)numSamples);
            gybD += getGyroY_rads() / ((double)numSamples);
            gzbD += getGyroZ_rads() / ((double)numSamples);
            
            i++;
        }
        delayMicroseconds(20000);
    }
    
    gx_offset = (float)gxbD;
    gy_offset = (float)gybD;
    gz_offset = (float)gzbD;

    // set the range, bandwidth, and srd back to what they were
    if (setGyroRange( gyroRange) < 0) {
        Serial.printf("error - calibrate_gyro\n");
        return;
    }
    
    if (setDlpfBandwidth( bandwidth) < 0) {
        Serial.printf("error - calibrate_gyro\n");
        return;
    }
    
    if (setSrd( srd) < 0) {
        Serial.printf("error - calibrate_gyro\n");
        return;
    }
}

void MPU9250_api::calibrate_accel() {
}

void MPU9250_api::calibrate_mag() {
}

