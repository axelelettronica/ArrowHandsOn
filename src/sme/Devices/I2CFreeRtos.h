/*
 * I2C.h
 *
 * Created: 28/12/2014 22:02:29
 *  Author: smkk
 */ 


#ifndef I2C_FREERTOS_H_
#define I2C_FREERTOS_H_
#include "../sme_FreeRTOS.h"

extern xSemaphoreHandle i2c_sem;
/**
 * \brief Function for configuring I2C master module
 *
 * This function will configure the I2C master module with
 * the SERCOM module to be used and pinmux settings
 */
void configure_i2c_master(uint32_t pinmux_sda, 
                          uint32_t pinmux_scl,
                          Sercom *const sercom);
                          
uint8_t readRegister(uint8_t slaveAddr, uint8_t i2cRegister, uint8_t *data);
uint8_t readRegister_16Bit(uint8_t slaveAddr, uint16_t i2cRegister, uint8_t *data);
bool readBufferRegister(uint8_t slaveAddr, uint8_t i2cRegister, uint8_t *buffer, uint8_t bufferLen);
bool readBufferRegister_16Bit(uint8_t slaveAddr, uint16_t i2cRegister, uint8_t *buffer, uint8_t bufferLen);

//void readRegisters(uint8_t slaveAddr, uint8_t i2cRegister, uint8_t * dest, int bytesToRead);
bool writeRegister(uint8_t slaveAddr, uint8_t i2cRegister, uint8_t dataToWrite);
bool writeRegister_16Bit(uint8_t slaveAddr, uint16_t i2cRegister, uint8_t dataToWrite);
bool writeBufferRegister(uint8_t slaveAddr, const uint8_t* reg_data_write, uint16_t reg_data_len);

#endif /* I2C_FREERTOS_H_ */
