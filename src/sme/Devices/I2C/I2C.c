/*
* I2C.c
*
* Created: 28/12/2014 22:02:19
*  Author: mfontane
*/
#include "I2C.h"
#include <asf.h>

xSemaphoreHandle i2c_sem;


#define I2C_SEMAPHORE_DELAY  (1000 / portTICK_RATE_MS)

/** I2C related structure */
static struct i2c_master_module i2c_master_instance;
static struct i2c_master_packet master_packet;

/**
* \brief Function for configuring I2C master module
*
* This function will configure the I2C master module with
* the SERCOM module to be used and pinmux settings
*/
void configure_i2c_master(void)
{
    // create the i2c semaphore
    i2c_sem = xSemaphoreCreateMutex();

    /* Create and initialize config structure */
    struct i2c_master_config config_i2c;
    i2c_master_get_config_defaults(&config_i2c);

    /* Change pins */
    config_i2c.pinmux_pad0  = SME_I2C_SERCOM_PINMUX_PAD0;
    config_i2c.pinmux_pad1  = SME_I2C_SERCOM_PINMUX_PAD1;
    // Changed to leave Pins to GPS Serial connection
    //config_i2c.pinmux_pad0  = PINMUX_PA22C_SERCOM3_PAD0;
    //config_i2c.pinmux_pad1  = PINMUX_PA23C_SERCOM3_PAD1;

    /* Initialize and enable device with config */
    i2c_master_init(&i2c_master_instance, SME_I2C_MODULE, &config_i2c);

    i2c_master_enable(&i2c_master_instance);
}

uint8_t readRegister(uint8_t slaveAddr, uint8_t i2cRegister, uint8_t *data)
{
    // first take the Semaphore
    BaseType_t ok = xSemaphoreTake(i2c_sem, I2C_SEMAPHORE_DELAY);
    if (ok) {
        uint8_t request_token =i2cRegister;
        uint32_t timeout = 0;

        /** Send the request token */
        master_packet.address         = slaveAddr;
        master_packet.data_length     = 1;
        master_packet.data            = &request_token;
        master_packet.ten_bit_address = false;
        master_packet.high_speed      = false;
        master_packet.hs_master_code  = 0x0;
        while (i2c_master_write_packet_wait_no_stop(&i2c_master_instance, &master_packet) !=
        STATUS_OK) {
            /* Increment timeout counter and check if timed out. */
            if (timeout++ == TIMEOUT) {
                // release the semaphore
                xSemaphoreGive(i2c_sem);
                return 0;
            }
        }

        // if everything is ok continue with next step

        /** Get the register value */
        master_packet.data_length     = 1;
        master_packet.data            = data;
        while (i2c_master_read_packet_wait(&i2c_master_instance, &master_packet) !=
        STATUS_OK) {
            /* Increment timeout counter and check if timed out. */
            if (timeout++ == TIMEOUT) {
                // release the semaphore
                xSemaphoreGive(i2c_sem);
                return 0;
            }
        }
    }

    // release the semaphore
    xSemaphoreGive(i2c_sem);
    return 1;
}



bool readBufferRegister(uint8_t slaveAddr, uint8_t i2cRegister, uint8_t *buffer, uint8_t bufferLen){
    
    uint8_t request_token =i2cRegister;
    uint32_t timeout = 0;

    // first take the Semaphore
    BaseType_t ok = xSemaphoreTake(i2c_sem, I2C_SEMAPHORE_DELAY);
    if (ok) {
        /** Send the request token */
        master_packet.address         = slaveAddr;
        master_packet.data_length     = 1;
        master_packet.data            = &request_token;
        master_packet.ten_bit_address = false;
        master_packet.high_speed      = false;
        master_packet.hs_master_code  = 0x0;
        while (i2c_master_write_packet_wait(&i2c_master_instance, &master_packet) !=
        STATUS_OK) {
            /* Increment timeout counter and check if timed out. */
            if (timeout++ == TIMEOUT) {
                // release the semaphore
                xSemaphoreGive(i2c_sem);
                return 0;
            }
        }

        // if everything is ok continue with next step

        /** Get the register value */
        master_packet.data_length     = bufferLen;
        master_packet.data            = buffer;
        while (i2c_master_read_packet_wait(&i2c_master_instance, &master_packet) !=
        STATUS_OK) {
            /* Increment timeout counter and check if timed out. */
            if (timeout++ == TIMEOUT) {
                // release the semaphore
                xSemaphoreGive(i2c_sem);
                return 0;
            }
            
        }
    }

    // release the semaphore
    xSemaphoreGive(i2c_sem);
    return 1;
}

bool writeRegister(uint8_t slaveAddr, uint8_t i2cRegister, uint8_t dataToWrite){
    uint8_t request_token[2];
    uint32_t timeout = 0;

    // first take the Semaphore
    BaseType_t ok = xSemaphoreTake(i2c_sem, I2C_SEMAPHORE_DELAY);
    if (ok) {

        request_token[0] =i2cRegister;
        request_token[1] = dataToWrite;
        
        /** Send the request token */
        master_packet.address         = slaveAddr;
        master_packet.data_length     = 2;
        master_packet.data            = request_token;
        master_packet.ten_bit_address = false;
        master_packet.high_speed      = false;
        master_packet.hs_master_code  = 0x0;
        while (i2c_master_write_packet_wait_no_stop(&i2c_master_instance, &master_packet) !=
        STATUS_OK) {
            /* Increment timeout counter and check if timed out. */
            if (timeout++ == TIMEOUT) {
                // release the semaphore
                xSemaphoreGive(i2c_sem);
                return 0;
            }
        }
    }

    // release the semaphore
    xSemaphoreGive(i2c_sem);
    return 1;
}