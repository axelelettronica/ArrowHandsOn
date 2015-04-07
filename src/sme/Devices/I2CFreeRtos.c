/*
 * I2C.c
 *
 * Created: 28/12/2014 22:02:19
 *  Author: smkk
 */
#include "I2C/smeI2C.h"
#include <asf.h>
#include "FreeRTOS.h"
#include "semphr.h"

xSemaphoreHandle i2c_sem;


#define I2C_SEMAPHORE_DELAY  (1000 / portTICK_RATE_MS)


/**
 * \brief Function for configuring I2C master module
 *
 * This function will configure the I2C master module with
 * the SERCOM module to be used and pinmux settings
 */
void configure_i2c_master(uint32_t pinmux_sda, 
        uint32_t pinmux_scl,
        Sercom *const sercom)
{
    // create the i2c semaphore
    i2c_sem = xSemaphoreCreateMutex();

    // call the library
    sme_i2c_configure_master(pinmux_sda, pinmux_scl, sercom);
} 

uint8_t readRegister(uint8_t slaveAddr, uint8_t i2cRegister, uint8_t *data)
{
    uint8_t ret = 0;
    // first take the Semaphore
    BaseType_t ok = xSemaphoreTake(i2c_sem, I2C_SEMAPHORE_DELAY);

    if (ok) {
        ret = sme_i2c_read_register(slaveAddr, i2cRegister, data);
        // release the semaphore
        xSemaphoreGive(i2c_sem);
    }

    return ret;
}

uint8_t readRegister_16Bit(uint8_t slaveAddr, uint16_t i2cRegister, uint8_t *data)
{
    uint8_t ret = 0;

    // first take the Semaphore
    BaseType_t ok = xSemaphoreTake(i2c_sem, I2C_SEMAPHORE_DELAY);
    if (ok) {
        ret = sme_i2c_read_register_16Bit(slaveAddr, i2cRegister, data);
        // release the semaphore
        xSemaphoreGive(i2c_sem);

    }

    return ret;
}

bool readBufferRegister(uint8_t slaveAddr, uint8_t i2cRegister, uint8_t *buffer, uint8_t bufferLen){

    uint8_t ret = 0;

    // first take the Semaphore
    BaseType_t ok = xSemaphoreTake(i2c_sem, I2C_SEMAPHORE_DELAY);
    if (ok) {
        ret = sme_i2c_read_buffer_register(slaveAddr, i2cRegister, buffer, bufferLen);
        // release the semaphore
        xSemaphoreGive(i2c_sem);
    }


    return ret;
}

bool readBufferRegister_16Bit(uint8_t slaveAddr, uint16_t i2cRegister, uint8_t *buffer, uint8_t bufferLen) {

    uint8_t ret = 0;

    // first take the Semaphore
    BaseType_t ok = xSemaphoreTake(i2c_sem, I2C_SEMAPHORE_DELAY);
    if (ok) {

        ret = sme_i2c_read_buffer_register_16Bit(slaveAddr, i2cRegister, buffer, bufferLen);
        // release the semaphore
        xSemaphoreGive(i2c_sem);
    }


    return ret;
}

bool writeRegister_16Bit(uint8_t slaveAddr, uint16_t i2cRegister, uint8_t dataToWrite){

    uint8_t ret = 0;

    // first take the Semaphore
    BaseType_t ok = xSemaphoreTake(i2c_sem, I2C_SEMAPHORE_DELAY);
    if (ok) {

        ret = sme_i2c_write_register_16Bit(slaveAddr, i2cRegister, dataToWrite);
        // release the semaphore
        xSemaphoreGive(i2c_sem);
    }

    return ret;
}

bool writeRegister(uint8_t slaveAddr, uint8_t i2cRegister, uint8_t dataToWrite){

    uint8_t ret = 0;
    // first take the Semaphore
    BaseType_t ok = xSemaphoreTake(i2c_sem, I2C_SEMAPHORE_DELAY);
    if (ok) {

        ret = sme_i2c_write_register(slaveAddr, i2cRegister, dataToWrite);
        // release the semaphore
        xSemaphoreGive(i2c_sem);
    }

    return ret;
}


bool writeBufferRegister(uint8_t slaveAddr, const uint8_t* reg_data_write, uint16_t reg_data_len){

    uint8_t ret = 0;

    // first take the Semaphore
    BaseType_t ok = xSemaphoreTake(i2c_sem, I2C_SEMAPHORE_DELAY);
    if (ok) {
        ret = sme_i2c_write_buffer_register(slaveAddr, reg_data_write, reg_data_len);
        // release the semaphore
        xSemaphoreGive(i2c_sem);
    }


    return ret;
}
