/*
 * smeI2C.h
 *
 * Created: 4/7/2015 11:52:59 AM
 *  Author: smkk
 */ 


#ifndef SMEI2C_H_
#define SMEI2C_H_

#include <stdint-gcc.h>
#include <stdbool.h>
#include <sercom.h>


/* Number of times to try to send packet if failed. */
#define TIMEOUT 10

void sme_i2c_configure_master(uint32_t pinmux_sda, /** PAD0 (SDA) pinmux. */
        uint32_t pinmux_scl, /** PAD1 (SCL) pinmux. */
        Sercom *const sercom);

uint8_t sme_i2c_read_register(uint8_t slaveAddr, uint8_t i2cRegister, uint8_t *data);
uint8_t sme_i2c_read_register_16Bit(uint8_t slaveAddr, uint16_t i2cRegister, uint8_t *data);
bool    sme_i2c_read_buffer_register(uint8_t slaveAddr, uint8_t i2cRegister, uint8_t *buffer, uint8_t bufferLen);
bool    sme_i2c_read_buffer_register_16Bit(uint8_t slaveAddr, uint16_t i2cRegister, uint8_t *buffer, uint8_t bufferLen);
bool    sme_i2c_read_buffer(uint8_t slaveAddr, uint8_t *buffer, uint8_t bufferLen);

//void readRegisters(uint8_t slaveAddr, uint8_t i2cRegister, uint8_t * dest, int bytesToRead);
bool    sme_i2c_write_register(uint8_t slaveAddr, uint8_t i2cRegister, uint8_t dataToWrite);
bool    sme_i2c_write_register_16Bit(uint8_t slaveAddr, uint16_t i2cRegister, uint8_t dataToWrite);
bool    sme_i2c_write_buffer_register(uint8_t slaveAddr, const uint8_t* reg_data_write, uint16_t reg_data_len, bool stop);

bool    sme_i2c_wakeup(uint8_t slaveAddr, uint8_t i2cRegister, uint8_t dataToWrite);

#endif /* SMEI2C_H_ */
