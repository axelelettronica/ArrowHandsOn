#include <stdint-gcc.h>
#include "sme_sfx_behaviour.h"
#include "sme_gps_behaviour.h"
#include "Devices\uart\gps\sme_sl868v2_execute.h"

/*
* sme_button_behaviour.c
*
* Created: 3/1/2015 9:34:38 PM
*  Author: smkk
*/


void button1Execution(void) {

 sfxSendExecution(SME_SFX_I2C_1_MSG);

}

void button2Execution(void) {

    // Just send with SFX the gps cached values
    sme_gps_only_behaviour();

}
