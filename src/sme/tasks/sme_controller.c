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
        configMINIMAL_STACK_SIZE,
        NULL,
        CONTROL_TASK_PRIORITY,
        NULL);
    }
    
    return err;
}


static void sendToSigFox(uint8_t sensorData){
    //sigFoxT *sfModel= 
    getSigFoxModel();
    
}

/*
* First Use case
detect NFC interrupt:
1) take data from sensor
2) take GPS position
3) Send all to SigFox
*/
static void performExecution( uint16_t detection) {
    uint8_t data;
    #if NOT_SENSOR
    if (1) {
        // point 1
        LSM9DS1getValues((char *)&data);

        //point 2 (could be a FSM because has to be wait the GSM wake-up)
        // getPosition();

        //point 3
        sendToSigFox(data);

        #else
        if ((detection & NFC_FD) == NFC_FD) {
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
                    print_dbg("interruptDetected %d\n", current_message.intE);
                    performExecution(interruptDetection());
                    break;
                    default:
                        print_dbg("Not Handled\n");
                }
            }
        }
    }

