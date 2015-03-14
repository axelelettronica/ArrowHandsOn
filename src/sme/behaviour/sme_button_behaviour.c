#include <stdint-gcc.h>
#include "sme_sfx_behaviour.h"
#include "sme_gps_behaviour.h"
#include "Devices\uart\gps\sme_sl868v2_execute.h"

/*
* sme_button_behaviour.c
*
* Created: 3/1/2015 9:34:38 PM
*  Author: mfontane
*/


void button1Execution(void) {

    /* Send single messages
    //sfxSendExecution(SME_SFX_I2C_1_MSG);
    //sfxSendExecution(SME_SFX_I2C_XL_MSG);
    //sfxSendExecution(SME_SFX_I2C_GYR_MSG);
    //sfxSendExecution(SME_SFX_I2C_MAG_MSG);
    */
    
    // collect new GPS location and send it with all sensor values
    gpsStartScan(sme_gps_button1_behaviour);

}

void button2Execution(void) {
    #if DEBUG_SIGFOX
    // Just send with SFX the gps cached/default values
    sme_gps_button2_behaviour();
    #else
    gpsStartScan(sme_gps_button2_behaviour);
    #endif
}