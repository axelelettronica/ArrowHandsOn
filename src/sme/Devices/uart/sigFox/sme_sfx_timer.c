/*
* sme_sfx_timer.c
*
* Created: 1/31/2015 1:11:39 PM
*  Author: mfontane
*/

#include <stdbool.h>
#include "../../../sme_FreeRTOS.h"
#include "sme_cdc_io.h"
#include "timers.h"
#include "sme_sfx_timer.h"
#include "sme/model/sme_model_sigfox.h"
#include "sme_sigfox_execute.h"
#include "../../IO/sme_rgb_led.h"
#include "sme_pin_definition.h"
#include "port.h"

static TimerHandle_t sfxCommandTimeOut;
static bool timedOut;

inline bool isSfxCommandTimerExpired(void) {
    return timedOut;
}

static void sfxTimerCallback( TimerHandle_t pxTimer )
{
    /* Optionally do something if the pxTimer parameter is NULL. */
    configASSERT( pxTimer );
    print_sfx("TimeOut\r\n");
    sme_led_red_off();
    sme_led_blue_off();
     port_pin_set_output_level(SME_LED_Y1_PIN, SME_LED_Y1_INACTIVE);
}

void initSfxTimer(void){
    sfxCommandTimeOut = xTimerCreate
    (  /* Just a text name, not used by the RTOS kernel. */
    "SigFoxCmdTimeOut",
    /* The timer period in ticks. */
    SFX_COMMAND_TIMEOUT,
    /* The timers will NOT auto-reload themselves when they expire. */
    pdFALSE,
    /* Assign each timer a unique id. */
    ( void * ) SFX_COMMAND_TIMEOUT_ID,
    /* callback when it expires. */
    sfxTimerCallback
    );
}


void stopSfxCommandTimer(void){
    xTimerStop( sfxCommandTimeOut, 0 );
}


void startSfxCommandTimer(void) {
    // start or reset the timer
    BaseType_t start =  xTimerStart(sfxCommandTimeOut, 0 );
    if(start != pdPASS )
    {
        print_err("startSfxCommandTimer - Timer NO started");
    }
    else {
        print_sfx("Timer sfxCommandTimeOut started\r\n");
        timedOut = false;
    }
}