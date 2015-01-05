/*
 * I2C.h
 *
 * Created: 28/12/2014 22:02:29
 *  Author: mfontane
 */ 


#ifndef I2C_H_
#define I2C_H_
#include <asf.h>

/* Number of times to try to send packet if failed. */
#define TIMEOUT 3

void configure_i2c_master(void);

uint8_t readRegister(uint8_t slaveAddr, uint8_t i2cRegister, uint8_t *data);
bool readBufferRegister(uint8_t slaveAddr, uint8_t i2cRegister, uint8_t *buffer, uint8_t bufferLen);

//void readRegisters(uint8_t slaveAddr, uint8_t i2cRegister, uint8_t * dest, int bytesToRead);
bool writeRegister(uint8_t slaveAddr, uint8_t i2cRegister, uint8_t dataToWrite);

#endif /* I2C_H_ */