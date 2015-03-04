/*
* controller.c
*
* Created: 02/01/2015 01:47:55
*  Author: speirano
*/

#include "sme_cmn.h"
#include "sme_controller.h"
#include "..\interrupt\interruptHandle.h"
#include "..\Devices\I2C\Accelerometer\LSM9DS1.h"
#include "common/sme_timer_define.h"
#include "behaviour/sme_gps_behaviour.h"
#include "behaviour/sme_interrupt_behaviour.h"
#include "behaviour/sme_button_behaviour.h"
#include "model/sme_model_sigfox.h"
#include "Devices/uart/sigFox/sme_sigfox_execute.h"
#include "interrupt/interrupt.h"

static void control_task(void *params);

#define KEEP_TIMEOUT 276 // number of %minute in 23Hour
xQueueHandle controllerQueue;
static int keepTimeout;

int sme_ctrl_init(void)
{

    int err = SME_OK;

    controllerQueue = xQueueCreate(64, sizeof(controllerQueueS));
    if( controllerQueue != 0 )
    {
        err= xTaskCreate(control_task,
        (const char *) "Control",
        configMINIMAL_STACK_SIZE*2,
        NULL,
        CONTROL_TASK_PRIORITY,
        NULL);
    }
    
    return err;
}

/**
*
* Send the KEEP message if SigFox is in data mode
*
*/
static void sendToSfxKeep(void){
    
    print_sfx("Sending KEEP\n\r");
    sigFoxT *sfModel = getSigFoxModel();

    sfModel->messageType = dataIntMessage;
    sfModel->message.dataMode.length=0;
    sfModel->message.dataMode.type = SIGFOX_KEEP;
    sfModel->message.dataMode.sequenceNumber = getNewSequenceNumber();

    executeCDCSigFox(sfModel);
    
}
static void sendToSfxExitConf(void){
    sigFoxT *sfModel = getSigFoxModel();

    sfModel->messageType = enterDataMode;

    executeCDCSigFox(sfModel);
}

static void enter_conf_mode(void) {
    port_pin_set_output_level(SME_LED_Y2_PIN, SME_LED_Y2_ACTIVE);
    sigFoxT *sfModel = getSigFoxModel();

    sfModel->messageType = enterConfMode;
    sfModel->message.dataMode.type = SIGFOX_DATA;

    // point 1
    sfModel->message.dataMode.length = sprintf(sfModel->message.dataMode.payload,"Smart");

    //point 3 SEND !!!!!!!!!!!
    sfModel->message.dataMode.sequenceNumber = getNewSequenceNumber();

    executeCDCSigFox(sfModel);
}

static void enter_in_data_mode(void){
    
    port_pin_set_output_level(SME_LED_Y1_PIN, SME_LED_Y1_ACTIVE);
    sigFoxT *sfModel = getSigFoxModel();

    sfModel->messageType = enterDataMode;
    sfModel->message.dataMode.type = SIGFOX_DATA;

    // point 1
    sfModel->message.dataMode.length = sprintf(sfModel->message.dataMode.payload,"Sent by SmartEverything");

    //point 3 SEND !!!!!!!!!!!
    sfModel->message.dataMode.sequenceNumber = getNewSequenceNumber();

    executeCDCSigFox(sfModel);
}





#if DEBUG_SIGFOX
char debugSFXMsg[100];
uint8_t msgCounter=0;
uint8_t   timeoutCounter=2;

static void debugSigFox(void){
    static uint8_t msg_toggle = 0;
    uint8_t msg_toggle_set = 2;
    timeoutCounter++;
    if (timeoutCounter==3) {
        timeoutCounter =0;
        msgCounter++;
        switch (msg_toggle%msg_toggle_set) {
           case 0:
               button1Execution();
               msg_toggle++;
           break;
           case 1:
               button2Execution();
               msg_toggle++;
           break;
           // Add more test messages here
           default:
           break;
        } 
        print_sfx("Sending DEBUG MSG: %d  (total sent%d)\n\r", 
                  msg_toggle%msg_toggle_set, msg_toggle);
    }
}
#endif

static void sfxTimeOut(void) {

    print_dbg("5 minutes timeout \r\n");

    #if DEBUG_SIGFOX
    debugSigFox();
    #else
    if (sfxIsInDataStatus()) {
        keepTimeout++;
        //every 23 Hours send the KEEP Message
        if (keepTimeout == KEEP_TIMEOUT) {
            print_sfx("Sending SFX KEEP\r\n");
            sendToSfxKeep();
            keepTimeout=0; // reset the counter for Keep
        }
    }
    else {
        print_sfx("no command sent, exit from command mode\r\n");
        sendToSfxExitConf();
    }
    #endif
}

/**
*
* This task is the board controller.
* This manage the SmartEverything controller as required
*
* \param params Parameters for the task. (Not used.)
*/

static void control_task(void *params)
{
    #ifdef VIRGIN_UNIT
    uint8_t initialized=0;
    int timeOut = TWO_SECOND;
    #else
    int timeOut = FIVE_MINUTE;
    #endif
    
    controllerQueueS current_message;
    //uint8_t value;
    
    for(;;) {
        if (xQueueReceive(controllerQueue, &current_message, timeOut)) {
            port_pin_set_output_level(SME_LED_Y2_PIN, SME_LED_Y2_INACTIVE);
            port_pin_set_output_level(SME_LED_Y1_PIN, SME_LED_Y1_INACTIVE);
            switch(current_message.intE) {
                case button1Int:
                button1Execution();
                break;

                case button2Int:
                button2Execution();
                break;

                // check on the I/O expander which is the interrupt
                case externalInt:
                performExecution(interruptDetection());
                break;

                case gpsData:
                sme_gps_data_updateExecution();
                break;

                default:
                print_dbg("Interrupt Not Handled\n");
            }
            clearInt(current_message.intE);
        }
        else {
            #ifdef VIRGIN_UNIT
            switch (initialized){
                case 0:
                enter_conf_mode();
                initialized++;
                break;
                
                case 1:
                enter_in_data_mode();
                initialized++;
                break;
                
                case 2:
                timeOut = CONTROL_TASK_DELAY;
                sendToSfxKeep();                
                initialized++;
                break;
                
                default:
                sfxTimeOut();
                break;
            }
            #else
            sfxTimeOut();
            #endif
        }
    }
}


