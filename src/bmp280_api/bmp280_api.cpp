#include "bmp280_api.h"

// static I2c* global_i2c;

I2c* BMP280_api::i2c;

/*!
 *  @brief Function for writing the sensor's registers through I2C bus.
 *
 *  @param[in] i2c_addr : sensor I2C address.
 *  @param[in] reg_addr : Register address.
 *  @param[in] reg_data : Pointer to the data buffer whose value is to be written.
 *  @param[in] length   : No of bytes to write.
 *
 *  @return Status of execution
 *  @retval 0 -> Success
 *  @retval >0 -> Failure Info
 *
 */
int8_t BMP280_api::i2c_reg_write(
    uint8_t i2c_addr, 
    uint8_t reg_addr, 
    uint8_t *reg_data, 
    uint16_t length
) {
    i2c->writeBytes( i2c_addr, reg_addr, length, reg_data);
    return 0;
}

/*!
 *  @brief Function for reading the sensor's registers through I2C bus.
 *
 *  @param[in] i2c_addr : Sensor I2C address.
 *  @param[in] reg_addr : Register address.
 *  @param[out] reg_data    : Pointer to the data buffer to store the read data.
 *  @param[in] length   : No of bytes to read.
 *
 *  @return Status of execution
 *  @retval 0 -> Success
 *  @retval >0 -> Failure Info
 *
 */
int8_t BMP280_api::i2c_reg_read(
    uint8_t i2c_addr, 
    uint8_t reg_addr, 
    uint8_t *reg_data, 
    uint16_t length
) {
    i2c->readBytes(i2c_addr, reg_addr, length, reg_data);
    return 0;
}

void print_rslt(const char api_name[], int8_t rslt) {
    if (rslt != BMP280_OK) {
        Serial.printf("%s\t", api_name);
        if (rslt == BMP280_E_NULL_PTR) {
            Serial.printf("Error [%d] : Null pointer error\r\n", rslt);
        } else if (rslt == BMP280_E_COMM_FAIL) {
            Serial.printf("Error [%d] : Bus communication failed\r\n", rslt);
        } else if (rslt == BMP280_E_IMPLAUS_TEMP) {
            Serial.printf("Error [%d] : Invalid Temperature\r\n", rslt);
        } else if (rslt == BMP280_E_DEV_NOT_FOUND) {
            Serial.printf("Error [%d] : Device not found\r\n", rslt);
        } else {
            /* For more error codes refer "*_defs.h" */
            Serial.printf("Error [%d] : Unknown error code\r\n", rslt);
        }
    }
}

BMP280_api::BMP280_api(I2c& i2c, const uint8_t address) {
    this->i2c = &i2c;
    this->address = address;
}

void BMP280_api::init() {
    struct bmp280_config conf;
     
    // Assign device I2C address based on the status of SDO pin (GND for PRIMARY(0x76) & VDD for SECONDARY(0x77)) 
    dev.dev_id = address;
    
    // Select the interface mode as I2C 
    dev.intf = BMP280_I2C_INTF;

    dev.delay_ms = delay;
    dev.read = i2c_reg_read;
    dev.write = i2c_reg_write;
    
    rslt = bmp280_init(&dev);
    
    /*****/
    // El parametro de calibración dig_t3 debería de estar entre -3000 y -1000
    // ...no lo está
    Serial.printf("t1 = %d \nt2 = %d \nt3 = %d \np1 = %d \np2 = %d \np3 = %d \np4 = %d \np5 = %d \np6 = %d \np7 = %d \np8 = %d \np9 = %d\n", 
        dev.calib_param.dig_t1,
        dev.calib_param.dig_t2,
        dev.calib_param.dig_t3,
        dev.calib_param.dig_p1,
        dev.calib_param.dig_p2,
        dev.calib_param.dig_p3,
        dev.calib_param.dig_p4,
        dev.calib_param.dig_p5,
        dev.calib_param.dig_p6,
        dev.calib_param.dig_p7,
        dev.calib_param.dig_p8,
        dev.calib_param.dig_p9
    );
//     dev.calib_param.dig_t3 = -1000;
    /****/
    
    print_rslt(" bmp280_init status", rslt);
    
    rslt = bmp280_selftest(&dev);
    print_rslt(" bmp280_perform_self_test status", rslt);
    if (rslt == BMP280_OK) {
        Serial.printf("\n Self test success \n");
    } else {
        Serial.printf("\n Self test failed \n");
    }
    
    // Always read the current settings before writing, especially when
    //all the configuration is not modified
    rslt = bmp280_get_config(&conf, &dev);
    print_rslt(" bmp280_get_config status", rslt);

    // configuring the temperature oversampling, filter coefficient and output data rate 
    // Overwrite the desired settings 
    conf.filter = BMP280_FILTER_COEFF_16;
    
    // Temperature oversampling set
    conf.os_temp = BMP280_OS_4X;

    // Pressure oversampling set at 16x 
    conf.os_pres = BMP280_OS_16X;

    // Setting the output data rate 
    conf.odr = BMP280_ODR_0_5_MS;
    
    rslt = bmp280_set_config(&conf, &dev);
    print_rslt(" bmp280_set_config status", rslt);

    // Always set the power mode after setting the configuration 
    rslt = bmp280_set_power_mode(BMP280_NORMAL_MODE, &dev);
    print_rslt(" bmp280_set_power_mode status", rslt);
    
}

void BMP280_api::read_test() {
    uint32_t press_32bits_u32;      // Para convertir la pression a entero con el algoritmo de 32bits
    
    rslt = bmp280_get_uncomp_data(&uncomp_data, &dev);
    
    // Getting the 32 bit compensated temperature 
    rslt = bmp280_get_comp_temp_32bit(&temp_32, uncomp_data.uncomp_temp, &dev);

    // Getting the compensated temperature as floating point value 
    rslt = bmp280_get_comp_temp_double(&temp_lf, uncomp_data.uncomp_temp, &dev);

    // Getting the compensated pressure using 32 bit precision 
    rslt = bmp280_get_comp_pres_32bit(&press_32bits_u32, uncomp_data.uncomp_press, &dev);

    // Getting the compensated pressure using 64 bit precision 
    rslt = bmp280_get_comp_pres_64bit(&press_u32, uncomp_data.uncomp_press, &dev);

    // Getting the compensated pressure as floating point value 
    rslt = bmp280_get_comp_pres_double(&press_lf, uncomp_data.uncomp_press, &dev);
    
    Serial.printf(
        "UT: %ld, T32: %ld, T: %f \r\n", 
        uncomp_data.uncomp_temp, 
        temp_32, 
        temp_lf
    );
    
    Serial.printf(
        "UP: %ld, P32: %ld, P64: %ld, P64N: %ld, P: %f\r\n",
        uncomp_data.uncomp_press,
        press_32bits_u32,
        press_u32,
        press_u32 / 256,
        press_lf
    );
}

void BMP280_api::read() {
    i2c->readBytes( address, BMP280_PRES_MSB_ADDR, 6, buffer);
    
    uncomp_data.uncomp_press = (int32_t) (
        (((uint32_t) (buffer[0])) << 12) | 
        (((uint32_t) (buffer[1])) << 4) | 
        ((uint32_t) buffer[2] >> 4)
    );
    
    uncomp_data.uncomp_temp = (int32_t) (
        (((int32_t) (buffer[3])) << 12) | 
        (((int32_t) (buffer[4])) << 4) | 
        (((int32_t) (buffer[5])) >> 4)
    );
    
    Serial.printf( "UT: %d, \tUP: %d\n", uncomp_data.uncomp_temp, uncomp_data.uncomp_press);

//     int8_t rslt = st_check_boundaries(
//         (int32_t)uncomp_data.uncomp_temp, 
//         (int32_t)uncomp_data.uncomp_press
//     );
}

int32_t BMP280_api::get_temperature_32() {
    int32_t var1, var2;
    int32_t p1 = (uncomp_data.uncomp_temp >> 4) - ((int32_t) dev.calib_param.dig_t1);
    
    var1 = (((uncomp_data.uncomp_temp >> 3) - ((int32_t) dev.calib_param.dig_t1 << 1)) * ((int32_t) dev.calib_param.dig_t2)) >> 11;
    var2 = (((p1 * p1) >> 12) * ((int32_t) dev.calib_param.dig_t3)) >> 14;
        
    dev.calib_param.t_fine = var1 + var2;
    temp_32 = (dev.calib_param.t_fine * 5 + 128) >> 8;
    
    return temp_32;
}


double BMP280_api::get_temperature_lf() {
    double var1, var2;
    double p1;
    
    var1 = 
        (((double) uncomp_data.uncomp_temp) / 16384.0 - 
        ((double) dev.calib_param.dig_t1) / 1024.0) *
        ((double) dev.calib_param.dig_t2);
            
    p1 = (((double) uncomp_data.uncomp_temp) / 131072.0 - ((double) dev.calib_param.dig_t1) / 8192.0);
    var2 = (p1 * p1) * ((double) dev.calib_param.dig_t3);
    p1 = var1 + var2;
        
    dev.calib_param.t_fine = (int32_t) p1;
    temp_lf = (p1 / 5120.0);
    
    return temp_lf;
}

uint32_t BMP280_api::get_pressure_32bit_u32() {
    int32_t var1, var2;
    int32_t p1;
    uint32_t p2;
    
    var1 = (((int32_t) dev.calib_param.t_fine) >> 1) - (int32_t) 64000;
    p1 = (var1 >> 2);
    var2 = ((p1 * p1) >> 11) * ((int32_t) dev.calib_param.dig_p6);
    var2 = var2 + ((var1 * ((int32_t) dev.calib_param.dig_p5)) << 1);
    var2 = (var2 >> 2) + (((int32_t) dev.calib_param.dig_p4) << 16);
    p1 = (var1 >> 2);
    var1 =
        (((dev.calib_param.dig_p3 * ((p1 * p1) >> 13)) >> 3) +
        ((((int32_t) dev.calib_param.dig_p2) * var1) >> 1)) >> 18;
    var1 = ((((32768 + var1)) * ((int32_t) dev.calib_param.dig_p1)) >> 15);
    
    press_u32 = (((uint32_t) (((int32_t)1048576) - uncomp_data.uncomp_press) - (var2 >> 12))) * 3125;

    /* Avoid exception caused by division with zero */
    if (var1 != 0) {
        /* Check for overflows against UINT32_MAX/2; if pres is left-shifted by 1 */
        if (press_u32 < 0x80000000) {
            press_u32 = (press_u32 << 1) / ((uint32_t) var1);
        } else {
            press_u32 = (press_u32 / (uint32_t) var1) << 1;
        }
        
        p2 = (press_u32 >> 3);
        var1 = (((int32_t) dev.calib_param.dig_p9) * ((int32_t) ((p2 * p2) >> 13))) >> 12;
        var2 = (((int32_t) (press_u32 >> 2)) * ((int32_t) dev.calib_param.dig_p8)) >> 13;
            
        press_u32 = (uint32_t) ((int32_t) press_u32 + ((var1 + var2 + dev.calib_param.dig_p7) >> 4));
//         rslt = BMP280_OK;
    } else {
        press_u32 = 0;
//         rslt = BMP280_E_32BIT_COMP_PRESS;
    }
    
    return press_u32;
}


uint32_t BMP280_api::get_pressure_u32() {
    int64_t var1, var2, p;
    
    var1 = ((int64_t) (dev.calib_param.t_fine)) - 128000;
    var2 = var1 * var1 * (int64_t) dev.calib_param.dig_p6;
    var2 = var2 + ((var1 * (int64_t) dev.calib_param.dig_p5) << 17);
    var2 = var2 + (((int64_t) dev.calib_param.dig_p4) << 35);
    var1 = ((var1 * var1 * (int64_t) dev.calib_param.dig_p3) >> 8) +
        ((var1 * (int64_t) dev.calib_param.dig_p2) << 12);
    var1 = ((INT64_C(0x800000000000) + var1) * 
        ((int64_t) dev.calib_param.dig_p1)) >> 33;
    
    if (var1 != 0) {
        p = 1048576 - uncomp_data.uncomp_press;
        p = (((((p << 31)) - var2) * 3125) / var1);
        var1 = (((int64_t) dev.calib_param.dig_p9) * (p >> 13) * (p >> 13)) >> 25;
        var2 = (((int64_t) dev.calib_param.dig_p8) * p) >> 19;
        p = ((p + var1 + var2) >> 8) + (((int64_t)dev.calib_param.dig_p7) << 4);
        press_u32 = (uint32_t)p;
//         rslt = BMP280_OK;
    } else {
        press_u32 = 0;
//         rslt = BMP280_E_64BIT_COMP_PRESS;
    }
    
    return press_u32;
}

double BMP280_api::get_pressure_lf() {
    double var1, var2;
    int32_t var3;
    
    var1 = ((double) dev.calib_param.t_fine / 2.0) - 64000.0;
    var2 = var1 * var1 * ((double) dev.calib_param.dig_p6) / 32768.0;
    var2 = var2 + var1 * ((double) dev.calib_param.dig_p5) * 2.0;
    var2 = (var2 / 4.0) + (((double) dev.calib_param.dig_p4) * 65536.0);
    var1 = (((double) dev.calib_param.dig_p3) * var1 * 
        var1 / 524288.0 + ((double) dev.calib_param.dig_p2) * var1) / 524288.0;
    var1 = (1.0 + var1 / 32768.0) * ((double) dev.calib_param.dig_p1);
    
    var3 = (uint32_t)(1048576.0 - (double) uncomp_data.uncomp_press);
    
    if (var1 < 0 || var1 > 0) {
        var3 = (uint32_t)((var3 - (var2 / 4096.0)) * 6250.0 / var1);
        var1 = ((double) dev.calib_param.dig_p9) * var3 * var3 / 2147483648.0;
        var2 = var3 * ((double) dev.calib_param.dig_p8) / 32768.0;
        press_lf = (var3 + (var1 + var2 + ((double) dev.calib_param.dig_p7)) / 16.0);
    } else {
        press_lf = 0;
//         rslt = BMP280_E_DOUBLE_COMP_PRESS;
    }

    return press_lf;
}

double BMP280_api::get_alture_lf() {
    return 0;
}



