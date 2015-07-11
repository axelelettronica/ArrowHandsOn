/** \file
 *  \brief Functions of Hardware Dependent Part of ATSHA204 Physical Layer
 *         Using I<SUP>2</SUP>C For Communication
 *  \author Atmel Crypto Products
 *  \date  January 11, 2013
 * \copyright Copyright (c) 2013 Atmel Corporation. All rights reserved.
 *
 * \atsha204_library_license_start
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel integrated circuit.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \atsha204_library_license_stop
 */



#include "i2c_phys.h"     // definitions and declarations for the hardware dependent I2C module


/** \brief This function initializes and enables the I<SUP>2</SUP>C peripheral.
 * */
void i2c_enable(void)
{
	
}


/** \brief This function disables the I<SUP>2</SUP>C peripheral. */
void i2c_disable(void)
{
	
}


/** \brief This function creates a Start condition (SDA low, then SCL low).
 * \return status of the operation
 * */
uint8_t i2c_send_start(void)
{
	

	return I2C_FUNCTION_RETCODE_SUCCESS;
}


/** \brief This function creates a Stop condition (SCL high, then SDA high).
 * \return status of the operation
 * */
uint8_t i2c_send_stop(void)
{
	

	return I2C_FUNCTION_RETCODE_SUCCESS;
}


/** \brief This function sends bytes to an I<SUP>2</SUP>C device.
 * \param[in] count number of bytes to send
 * \param[in] data pointer to tx buffer
 * \return status of the operation
 */
uint8_t i2c_send_bytes(uint8_t count, uint8_t *data)
{
	return I2C_FUNCTION_RETCODE_SUCCESS;
}


/** \brief This function receives one byte from an I<SUP>2</SUP>C device.
 *
 * \param[out] data pointer to received byte
 * \return status of the operation
 */
uint8_t i2c_receive_byte(uint8_t *data)
{
	
	return I2C_FUNCTION_RETCODE_SUCCESS;
}


/** \brief This function receives bytes from an I<SUP>2</SUP>C device
 *         and sends a Stop.
 *
 * \param[in] count number of bytes to receive
 * \param[out] data pointer to rx buffer
 * \return status of the operation
 */
uint8_t i2c_receive_bytes(uint8_t count, uint8_t *data)
{
	
	return i2c_send_stop();
}


    /** \brief This function creates a Start condition and sends the
    * I<SUP>2</SUP>C address.
    * \param[in] read #I2C_READ for reading, #I2C_WRITE for writing
    * \return status of the I<SUP>2</SUP>C operation
    */
    uint8_t i2c_send_slave_address(uint8_t device_address, uint8_t read)
    {
        uint8_t sla = device_address | read;
        uint8_t ret_code = i2c_send_start();
        if (ret_code != I2C_FUNCTION_RETCODE_SUCCESS)
        return ret_code;

        ret_code = i2c_send_bytes(1, &sla);

        if (ret_code != I2C_FUNCTION_RETCODE_SUCCESS)
        (void) i2c_send_stop();

        return ret_code;
    }
