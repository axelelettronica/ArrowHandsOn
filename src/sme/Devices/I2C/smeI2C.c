/*
 * I2C.c
 *
 * Created: 28/12/2014 22:02:19
 *  Author: smkk
 */
#include "smeI2C.h"
#include <asf.h>


/** I2C related structure */
static struct i2c_master_module i2c_master_instance;
static struct i2c_master_packet master_packet;
volatile int status=9;

/**
 * \brief Function for configuring I2C master module
 *
 * This function will configure the I2C master module with
 * the SERCOM module to be used and pinmux settings
 */
void sme_i2c_configure_master(uint32_t pinmux_sda, 
                          uint32_t pinmux_scl,
                          Sercom *const sercom)
{
    /* Create and initialize config structure */
    struct i2c_master_config config_i2c;
    i2c_master_get_config_defaults(&config_i2c);

    /* Change pins */
    config_i2c.pinmux_pad0  = pinmux_sda;
    config_i2c.pinmux_pad1  = pinmux_scl;


    /* Initialize and enable device with config */
    status =  i2c_master_init(&i2c_master_instance, sercom, &config_i2c);

    i2c_master_enable(&i2c_master_instance);
}

uint8_t sme_i2c_read_register(uint8_t slaveAddr, uint8_t i2cRegister, uint8_t *data)
{

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
                return 0;
            }
        }

    return 1;
}

uint8_t sme_i2c_read_register_16Bit(uint8_t slaveAddr, uint16_t i2cRegister, uint8_t *data)
{

        uint8_t request_token[2];
        request_token[0] =i2cRegister>>8 & 0xFF;
        request_token[1] =i2cRegister & 0xFF;
        uint32_t timeout = 0;

        /** Send the request token */
        master_packet.address         = slaveAddr;
        master_packet.data_length     = 2;
        master_packet.data            = &request_token;
        master_packet.ten_bit_address = false;
        master_packet.high_speed      = false;
        master_packet.hs_master_code  = 0x0;
        while (i2c_master_write_packet_wait_no_stop(&i2c_master_instance, &master_packet) !=
                STATUS_OK) {
            /* Increment timeout counter and check if timed out. */
            if (timeout++ == TIMEOUT) {
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
                return 0;
            }
        }

    return 1;
}

bool sme_i2c_read_buffer_register(uint8_t slaveAddr, uint8_t i2cRegister, uint8_t *buffer, uint8_t bufferLen){

    uint8_t request_token =i2cRegister;
    uint32_t timeout = 0;


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
                return 0;
            }

        }

    return 1;
}

bool sme_i2c_read_buffer_register_16Bit(uint8_t slaveAddr, uint16_t i2cRegister, uint8_t *buffer, uint8_t bufferLen) {

    uint8_t request_token[2];
    request_token[0] =i2cRegister>>8 & 0xFF;
    request_token[1] =i2cRegister & 0xFF;
    uint32_t timeout = 0;


        /** Send the request token */
        master_packet.address         = slaveAddr;
        master_packet.data_length     = 2;
        master_packet.data            = &request_token;
        master_packet.ten_bit_address = false;
        master_packet.high_speed      = false;
        master_packet.hs_master_code  = 0x0;
        while (i2c_master_write_packet_wait(&i2c_master_instance, &master_packet) !=
                STATUS_OK) {
            /* Increment timeout counter and check if timed out. */
            if (timeout++ == TIMEOUT) {
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
                return 0;
            }

        }

}

bool sme_i2c_write_register_16Bit(uint8_t slaveAddr, uint16_t i2cRegister, uint8_t dataToWrite){
    uint8_t request_token[3];
    uint32_t timeout = 0;


        request_token[0] =i2cRegister>>8 & 0xFF;
        request_token[1] =i2cRegister & 0xFF;
        request_token[2] = dataToWrite;

        /** Send the request token */
        master_packet.address         = slaveAddr;
        master_packet.data_length     = 3;
        master_packet.data            = request_token;
        master_packet.ten_bit_address = false;
        master_packet.high_speed      = false;
        master_packet.hs_master_code  = 0x0;
        int err;
        do {
            err = i2c_master_write_packet_wait_no_stop(&i2c_master_instance, &master_packet);
            /* Increment timeout counter and check if timed out. */
            if (timeout++ == TIMEOUT) {
                return 0;
            }
        } while( err != STATUS_OK);

    return 1;
}

bool sme_i2c_write_register(uint8_t slaveAddr, uint8_t i2cRegister, uint8_t dataToWrite){
    uint8_t request_token[2];
    uint32_t timeout = 0;



        request_token[0] =i2cRegister;
        request_token[1] = dataToWrite;

        /** Send the request token */
        master_packet.address         = slaveAddr;
        master_packet.data_length     = 2;
        master_packet.data            = request_token;
        master_packet.ten_bit_address = false;
        master_packet.high_speed      = false;
        master_packet.hs_master_code  = 0x0;
        int err;
        do {
            err = i2c_master_write_packet_wait_no_stop(&i2c_master_instance, &master_packet);
            /* Increment timeout counter and check if timed out. */
            if (timeout++ == TIMEOUT) {
                return 0;
            }
        } while( err != STATUS_OK);

    return 1;
}


bool sme_i2c_write_buffer_register(uint8_t slaveAddr, const uint8_t* reg_data_write, uint16_t reg_data_len){

    uint32_t timeout = 0;



        /** Send the request token */
        master_packet.address         = slaveAddr;
        master_packet.data_length     = reg_data_len;
        master_packet.data            = reg_data_write;
        master_packet.ten_bit_address = false;
        master_packet.high_speed      = false;
        master_packet.hs_master_code  = 0x0;
        int err;
        do {
            err = i2c_master_write_packet_wait_no_stop(&i2c_master_instance, &master_packet);
            /* Increment timeout counter and check if timed out. */
            if (timeout++ == TIMEOUT) {
                return 0;
            }
        } while( err != STATUS_OK);

    return 1;
}
