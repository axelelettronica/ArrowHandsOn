/*
 * sme_gps_behaviour.c
 *
 * Created: 3/1/2015 9:59:41 PM
 *  Author: smkk
 */

#include "sme_sfx_behaviour.h"
#include "tasks\sme_controller.h"
#include "sme_FreeRTOS.h"
#include "Devices\IO\sme_rgb_led.h"
#include "Devices\uart\gps\sme_sl868v2_execute.h"
#include "Devices\uart\gps\sme_gps_timer.h"
#include "model\sme_model_sl868v2.h"


 void sme_gps_stop_behaviour(void) 
 {
    gpsStopScan();
}

 void sme_gps_button1_behaviour(void) 
 {
    controllerQueueS gpsEvt;
    gpsEvt.intE = allSensorSend;
    xQueueSend(controllerQueue, (void *) &gpsEvt, NULL);
 }


void sme_gps_button2_behaviour(void) {
    
    controllerQueueS gpsEvt;

    gpsEvt.intE = gpsData;
    xQueueSend(controllerQueue, (void *) &gpsEvt, NULL);


    sme_led_blue_off();

    // set GPS in standby
    sendSl868v2Msg(SL868V2_SET_STDBY_CMD,
    sizeof(SL868V2_SET_STDBY_CMD));
}

/* 
 * Return cached location and Start a new scan
 */
void sme_gps_only_behaviour(void) 
{
    /* Return cached location */    
    controllerQueueS gpsEvt;
    gpsEvt.intE = gpsData;
    xQueueSend(controllerQueue, (void *) &gpsEvt, NULL);

    /* Start a new scan to collect current GPS location */
    gpsStartScan(sme_gps_stop_behaviour);

}
