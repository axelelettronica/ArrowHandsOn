/*
* sme_i2c_parse.c
*
* Created: 1/22/2015 10:34:33 PM
*  Author: mfontane
*/

#include "sme_i2c_parse.h"
#include "sme_cdc_cmd_parse.h"
#include "sme_cdc_uart.h"
#include "..\model\sme_model_i2c.h"
#include "..\Devices\I2C\nfc\nxpNfc.h"

char CDC_HELP_I2C[]   ="Help: i2c [nfc/hex-addressd] [r/w] <hex-register> <hex-data>\r\n";
static i2cQueueS   i2c_msg; 	// spi_msg


static int  getAddress(uint8_t *token, uint8_t *value) {
    int err = SME_OK;

    switch (token[0]){
        case 'n':
        case 'N':
            *value = NXPNFC_ADDRESS;
        break;

        default:
            err |= sme_hex_str_to_uint8(token,  value);
        break;
    }

    return err;
}


int parseI2CMsg(void) {
    i2cMessageS  *msg = &(i2c_msg.command);
    int err = SME_OK;
    
    err |= getAddress(sme_cli_msg.token[1],  &(msg->fields.sensorId));
    
    if (err == SME_OK) {
        

        // Read the regfister of the devices
        sme_hex_str_to_uint8(sme_cli_msg.token[3], &(msg->fields.i2cRegister));
        

        // read operation
        if (sme_cli_msg.token[2][0] == 'r') {
            i2c_msg.code = sensorReadRegister;
            } else {
            i2c_msg.code = sensorWriteRegister;
            msg->fields.datalen = strlen(sme_cli_msg.token[4]);
            err |= sme_hex_str_to_data(sme_cli_msg.token[4], msg->fields.data, msg->fields.datalen);
        }

        if (err == SME_OK){
            sme_cdc_i2c(&i2c_msg);
        }
    }
    
    if (err != SME_OK) {
        // print help
        print_out(CDC_HELP_I2C);
        return SME_EINVAL;
    }
    
    return SME_OK;
}