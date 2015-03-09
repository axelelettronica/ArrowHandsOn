/*
* sme_gps_timer.c
*
* Created: 1/31/2015 1:11:39 PM
*  Author: speirano
*/

#include <stdbool.h>
#include "../../../sme_FreeRTOS.h"
#include "sme_cdc_io.h"
#include "timers.h"
#include "sme_gps_timer.h"
#include "sme/model/sme_model_sl868v2.h"
#include "sme_sl868v2_execute.h"
#include "../../IO/sme_rgb_led.h"
#include "port.h"
#include "sme_pin_definition.h"
#include "delay.h"

static TimerHandle_t gpsCommandTimeOut;
static bool timedOut;

inline bool isGpsCommandTimerExpired(void) {
    return timedOut;
}

static void gpsTimerCallback( TimerHandle_t pxTimer )
{
    uint8_t i;
    /* Optionally do something if the pxTimer parameter is NULL. */
    configASSERT( pxTimer );
    print_err("TimeOut, GPS Position scanning not acknowledged\r\n");
    gpsCompletedScan();


    timedOut = true;
}

void initGpsTimer(void){
    gpsCommandTimeOut = xTimerCreate
    (  /* Just a text name, not used by the RTOS kernel. */
    "GpsCmdTimeOut",
    /* The timer period in ticks. */
    GPS_SCAN_TIMEOUT,
    /* The timers will NOT auto-reload themselves when they expire. */
    pdFALSE,
    /* Assign each timer a unique id. */
    ( void * )GPS_TIMEOUT_ID,
    /* callback when it expires. */
    gpsTimerCallback
    );
}


void stopGpsCommandTimer(void){
    sme_led_blue_off();
    xTimerStop( gpsCommandTimeOut, 0 );
}


void startGpsCommandTimer(void) {
    // start or rest the timer
    BaseType_t start =  xTimerStart(gpsCommandTimeOut, 0 );
    if(start != pdPASS )
    {
        print_err("startgpsCommandTimer - Timer NO started");
    }
    else {
        sme_led_blue_brightness(SIXTEEN_LIGTH);
        print_gps("%s","Timer gpsCommandTimeOut started\n");
        timedOut = false;
    }
    
}