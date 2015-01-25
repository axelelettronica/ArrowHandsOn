/*
* sme_i2c_parse.c
*
* Created: 1/22/2015 10:34:33 PM
*  Author: mfontane
*/

#include "sme_i2c_parse.h"
#include "sme_cdc_cmd_parse.h"
#include "sme_cdc_uart.h"


int parseI2CMsg(cdc_queue_msg_t *data) {
    /* I.E. i2c <hex-addressd> [r/w] <hex-register> <hex-data> */
    messageU  *msg = &(data->i2c_msg.command);
    int err = SME_OK;
    
    err |= sme_hex_str_to_uint8(sme_cli_msg.token[1],
    &(msg->fields.sensorId));
    
    if (err != SME_OK) {
        return err;
    }
    // read operation
    if (sme_cli_msg.token[2][0] == 'r') {
        data->i2c_msg.code = sensorReadRegister;
        data->i2c_msg.code = sensorWriteRegister;
        err |= sme_hex_str_to_data(sme_cli_msg.token[4], msg->fields.data,
        &(msg->fields.datalen));
        } else {
        // print help
        //return SME_EINVAL;
        
        // Just this i2c for now
        data->i2c_msg.code = justForDebugToBeRemoved;
        sme_cdc_i2c(&data->i2c_msg);
        return SME_OK;
    }

    return sme_hex_str_to_uint8(sme_cli_msg.token[3], &(msg->fields.i2cRegister));
}