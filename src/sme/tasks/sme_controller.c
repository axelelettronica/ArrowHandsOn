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
#include "..\model\sme_model_sigfox.h"
#include "..\Devices\uart\sigFox\sme_sigfox_execute.h"
#include "..\Devices\uart\sigFox\sme_sfx_timer.h"

static void control_task(void *params);

xQueueHandle controllerQueue;


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


static void sendToSigFoxValue(uint8_t sensorData){
    sigFoxT *sfModel = getSigFoxModel();

    sfModel->messageType = dataIntMessage;
    sfModel->message.dataMode.length++;
    sfModel->message.dataMode.type = SIGFOX_DATA;
    sfModel->message.dataMode.payload[0]=sensorData;
    sfModel->message.dataMode.sequenceNumber = getNewSequenceNumber();

    executeCDCSigFox(sfModel);
}


/**
*
* Send the KEEP message if SigFox is in data mode
*
*/
static void sendToSfxKeep(void){
    
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

static void sfxTimeOut(void){
    if (sfxIsInDataStatus()) {
        print_sfx("Sending SFX KEEP\r\n");
        sendToSfxKeep();
    }
    else {
        if (isSfxCommandTimerExpired()) {
            print_sfx("no command sent, exit from command mode\r\n");
            sendToSfxExitConf();
        }
    }

}
/*
* First Use case
detect NFC interrupt:
1) take data from sensor
2) take GPS position
3) Send all to SigFox
*/
static void performExecution( uint16_t intDetection) {
    uint8_t data;
    #if NOT_SENSOR
    if (1) {
        // point 1
        LSM9DS1getValues((char *)&data);

        //point 2 (could be a FSM because has to be wait the GSM wake-up)
        // getPosition();

        //point 3
        sendToSigFoxValue(data);

        #else
        // check which is the interrupt that wake-up the task
        if ((intDetection & NFC_FD_INT) == NFC_FD_INT) {
            #endif

        }
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
        //bool valueRead=false;
        controllerQueueS current_message;
        //uint8_t value;
        

        for(;;) {
            if (xQueueReceive(controllerQueue, &current_message, CONTROL_TASK_DELAY)) {
                switch(current_message.intE) {
                    case interruptDetected:
                    //print_dbg("interruptDetected %d\n", current_message.intE);
                    performExecution(interruptDetection());
                    break;
                    default:
                    print_dbg("Not Handled\n");
                }
            }
            else {
                sfxTimeOut();
            }
        }
    }

