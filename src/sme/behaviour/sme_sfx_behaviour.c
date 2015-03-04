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
 *  Author: mfontane
 */ 


static bool sme_sfx_fill_report (sme_sfx_report_t type, char *msg, uint8_t *len, uint8_t msg_len) {
    
    if (!msg || !len) {
        return SME_ERR;
    }

    ((char*)msg)[0] = type; // check HEX after 9

    switch (type) {
        case SME_SFX_DEBUG_MSG:
             *len = sprintf(&(((char*)msg)[1]),"Smart");
             break;
        case SME_SFX_I2C_1_MSG:
             sme_i2c_get_read_str(SME_I2C_PRESS_HUM_TEMP_STR,
                                  &(((char*)msg)[1]), len, msg_len-1);
             break;
        case SME_SFX_I2C_XL_MSG:
             sme_i2c_get_read_str(SME_I2C_XL_STR,
                                  &(((char*)msg)[1]), len, msg_len-1);
             break;
       case SME_SFX_I2C_GYR_MSG:
             sme_i2c_get_read_str(SME_I2C_GYRO_STR,
                                  &(((char*)msg)[1]), len, msg_len-1);
             break;
        case SME_SFX_I2C_MAG_MSG:
             sme_i2c_get_read_str(SME_I2C_MAGNET_STR,
                                  &(((char*)msg)[1]), len, msg_len-1);
             break;
        case SME_SFX_GPS_1_MSG:
             sme_sl868v2_get_latlong(&(((char*)msg)[1]), len, msg_len-1);
             break;
        default:
             print_err("Do Nothing %d\n", type);
    }
    (*len)++; // considering msg ID field
    return SME_OK;
}

void sfxSendExecution(sme_sfx_report_t msg_id) {
    
    //TCA6416a_gps_force_on();
    
    char *msg = NULL;
    char msg_len = 0;
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