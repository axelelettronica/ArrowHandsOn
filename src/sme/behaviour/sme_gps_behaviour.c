#include "sme_sfx_behaviour.h"
/*
 * sme_gps_behaviour.c
 *
 * Created: 3/1/2015 9:59:41 PM
 *  Author: mfontane
 */

 void sme_gps_data_updateExecution(void) {
 //TCA6416a_gps_force_on();
 sfxSendExecution(SME_SFX_GPS_1_MSG);
 }
