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
    print_rslt(" bmp280_init status", rslt);
    
    // Always read the current settings before writing, especially when
    //all the configuration is not modified
    rslt = bmp280_get_config(&conf, &dev);
    print_rslt(" bmp280_get_config status", rslt);

    // configuring the temperature oversampling, filter coefficient and output data rate 
    // Overwrite the desired settings 
    conf.filter = BMP280_FILTER_COEFF_2;

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

void BMP280_api::read() {
    rslt = bmp280_get_uncomp_data(&ucomp_data, &dev);
    
    // Getting the 32 bit compensated temperature 
    rslt = bmp280_get_comp_temp_32bit(&temp_32, ucomp_data.uncomp_temp, &dev);

    // Getting the compensated temperature as floating point value 
    rslt = bmp280_get_comp_temp_double(&temp_lf, ucomp_data.uncomp_temp, &dev);

    // Getting the compensated pressure using 32 bit precision 
    rslt = bmp280_get_comp_pres_32bit(&pres32, ucomp_data.uncomp_press, &dev);

    // Getting the compensated pressure using 64 bit precision 
    rslt = bmp280_get_comp_pres_64bit(&pres64, ucomp_data.uncomp_press, &dev);

    // Getting the compensated pressure as floating point value 
    rslt = bmp280_get_comp_pres_double(&pres, ucomp_data.uncomp_press, &dev);
    
    Serial.printf(
        "UT: %ld, T32: %ld, T: %f \r\n", 
        ucomp_data.uncomp_temp, 
        temp_32, 
        temp_lf
    );
    
    Serial.printf(
        "UP: %ld, P32: %ld, P64: %ld, P64N: %ld, P: %f\r\n",
        ucomp_data.uncomp_press,
        pres32,
        pres64,
        pres64 / 256,
        pres
    );
}

