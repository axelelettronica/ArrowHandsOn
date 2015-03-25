#include "sme_sfx_behaviour.h"
#include "model\sme_model_sigfox.h"
#include "Devices\uart\sigFox\sme_sigfox_execute.h"
#include "model\sme_model_sl868v2.h"
#include "model\sme_model_i2c.h"
#include "sme_cdc_io.h"
#include "sme_cmn.h"
/*
* sme_sfx_behaviour.c
*
* Created: 3/1/2015 9:54:37 PM
*  Author: smkk
*/

volatile  uint8_t len;
static bool sme_create_all_sensor_msg(char *msg, uint8_t *initLen, uint8_t msg_len) {
    uint8_t sensorMsgLen;
    len = *initLen;
    uint8_t tmpLen;
    
    for (char i = SME_SFX_I2C_1_MSG; i<SME_SFX_END_MSG; i++) {
        
        msg[len++] = i; // first add the msdId
        
        switch (i) {
            
            case SME_SFX_I2C_1_MSG:            
                sme_i2c_get_read_str(SME_I2C_PRESS_HUM_TEMP_STR, &(((char*)msg)[len]), &sensorMsgLen, msg_len-len);                
            break;
            
            case SME_SFX_I2C_XL_MSG:
                sme_i2c_get_read_str(SME_I2C_PROX_STR, &(((char*)msg)[len]), &sensorMsgLen, msg_len-len);                
                tmpLen = sensorMsgLen;
                sme_i2c_get_read_str(SME_I2C_XL_STR, &(((char*)msg)[len+tmpLen]), &sensorMsgLen, msg_len-len);
                sensorMsgLen += tmpLen; // add the 2 msg Len
            break;
            
            case SME_SFX_I2C_GYR_MSG:
                sme_i2c_get_read_str(SME_I2C_GYRO_STR, &(((char*)msg)[len]), &sensorMsgLen, msg_len-len);
            break;
            
            case SME_SFX_I2C_MAG_MSG:
                sme_i2c_get_read_str(SME_I2C_MAGNET_STR, &(((char*)msg)[len]), &sensorMsgLen, msg_len-len);
            break;
            
            case SME_SFX_GPS_1_MSG:
                sme_sl868v2_get_latlong(&(((char*)msg)[len]), &sensorMsgLen, msg_len-len);
            break;
            
            default:
            print_err("Do Nothing %d\n", i);
        }
        
        // fill the empty SFX buffer with zero
        for (; sensorMsgLen<SIG_FOX_DATA_PAYLOAD-1; sensorMsgLen++) 
            msg[sensorMsgLen+len]=0x00; // plus one because the msgId
            
        len += sensorMsgLen; //update the entire len
    }
    *initLen += len;
}
static bool sme_sfx_fill_report (sme_sfx_report_t type, char *msg, uint8_t *len, uint8_t msg_len) {
    
    if (!msg || !len) {
        return SME_ERR;
    }

    ((char*)msg)[0] = type; 

    switch (type) {
        case SME_SFX_DEBUG_MSG:
            sme_create_all_sensor_msg((char*)msg, len, SIG_FOX_MAX_PAYLOAD);
        break;
        
        case SME_SFX_I2C_1_MSG:
            sme_i2c_get_read_str(SME_I2C_PRESS_HUM_TEMP_STR, &(((char*)msg)[1]), len, msg_len-1);
        break;
        
        case SME_SFX_I2C_XL_MSG:
            sme_i2c_get_read_str(SME_I2C_PROX_STR, &(((char*)msg)[1]), len, msg_len-1);
            sme_i2c_get_read_str(SME_I2C_XL_STR, &(((char*)msg)[1]), len, msg_len-1);
        break;
        
        case SME_SFX_I2C_GYR_MSG:
            sme_i2c_get_read_str(SME_I2C_GYRO_STR, &(((char*)msg)[1]), len, msg_len-1);
        break;
        
        case SME_SFX_I2C_MAG_MSG:
            sme_i2c_get_read_str(SME_I2C_MAGNET_STR, &(((char*)msg)[1]), len, msg_len-1);
        break;
        
        case SME_SFX_GPS_1_MSG:
            sme_sl868v2_get_latlong(&(((char*)msg)[1]), len, msg_len-1);
        break;
        
        default:
            print_err("Do Nothing %d\n", type);
    }
    
    if (type != SME_SFX_DEBUG_MSG)
        (*len)++; // considering msg ID field, but not for the debug message which is more complex
        
    return SME_OK;
}

void sfxSendExecution(sme_sfx_report_t msg_id) {
    
    //TCA6416a_gps_force_on();
    
    sigFoxT *sfModel = getSigFoxModel();

    sfModel->messageType = dataIntMessage;
    sfModel->message.dataMode.type = SIGFOX_DATA;

    // point 1
    //sfModel->message.dataMode.length = sprintf(sfModel->message.dataMode.payload,"Smart");
    sme_sfx_fill_report(msg_id, sfModel->message.dataMode.payload,
    &sfModel->message.dataMode.length, 12);
    //point 3 SEND !!!!!!!!!!!
    sfModel->message.dataMode.sequenceNumber = getNewSequenceNumber();

    executeCDCSigFox(sfModel);

}
