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
#include "../devices/uart/gps/sme_sl868v2_execute.h"
#include "../interrupt/interrupt.h"
#include "../Devices/I2C/nfc/nxpNfc.h"

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

#if DEBUG_SIGFOX
char debugSFXMsg[100];
uint8_t msgCounter=0;
uint8_t   timeoutCounter=6;

static void debugSigFox(void){
    timeoutCounter++;
    if (timeoutCounter==7) {
        timeoutCounter =0;
        msgCounter++;
        print_sfx("Sending DEBUG MSG\n\r");
        sigFoxT *sfModel = getSigFoxModel();
        sprintf(sfModel->message.dataMode.payload, "SmartEveryThing 0x%x", msgCounter);
        sfModel->messageType = dataIntMessage;
        sfModel->message.dataMode.length=strlen(sfModel->message.dataMode.payload);
        sfModel->message.dataMode.type = SIGFOX_DATA;
        sfModel->message.dataMode.sequenceNumber = getNewSequenceNumber();

        executeCDCSigFox(sfModel);
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
    
    // point 1
    LSM9DS1getValues((char *)&data);

    //point 3
    sendToSigFoxValue(data);

    #else
    // check which is the interrupt that wake-up the task
    if ((intDetection & NFC_FD_INT) == NFC_FD_INT) {
    }
    #endif

    
}


/*
* First Use case
detect NFC interrupt:
1) take data from sensor
2) take GPS position
3) Send all to SigFox
*/
static void button2Execution(void) {
    sigFoxT *sfModel = getSigFoxModel();

    sfModel->messageType = dataIntMessage;
    sfModel->message.dataMode.type = SIGFOX_DATA;

    // point 1
    int offset;
    for (int page=0; page<255/NFC_PAGE_SIZE; page++){
        if (readUserData(page)) {
            
            sfModel->message.dataMode.length+=NFC_PAGE_SIZE;

            offset = ((page)*NFC_PAGE_SIZE);
            memcpy(&sfModel->message.dataMode.payload[offset],getLastNfcPage(),NFC_PAGE_SIZE);
        }
    }

    //point 2 (could be a FSM because has to be wait the GSM wake-up)
    // getPosition();

    //point 3 SEND !!!!!!!!!!!
    sfModel->message.dataMode.sequenceNumber = getNewSequenceNumber();

    executeCDCSigFox(sfModel);
}


/*
* First Use case
detect NFC interrupt:
1) take data from sensor
2) take GPS position
3) Send all to SigFox
*/
static void button1Execution(void) {
    sigFoxT *sfModel = getSigFoxModel();

    sfModel->messageType = dataIntMessage;
    sfModel->message.dataMode.type = SIGFOX_DATA;

    // point 1
    sfModel->message.dataMode.length = sprintf(sfModel->message.dataMode.payload,"Sent by SmartEverything");

    //point 2 (could be a FSM because has to be wait the GSM wake-up)
    gpsStartScan();

    //point 3 SEND !!!!!!!!!!!
    sfModel->message.dataMode.sequenceNumber = getNewSequenceNumber();

    executeCDCSigFox(sfModel); 
}

/*
* First Use case
detect NFC interrupt:
1) take data from sensor
2) take GPS position
3) Send all to SigFox
*/
static void sme_gps_data_updateExecution(void) {
    uint8_t data;
    sigFoxT *sfModel = getSigFoxModel();

    sfModel->messageType = dataIntMessage;
    sfModel->message.dataMode.type = SIGFOX_DATA;

    // point 1
    sfModel->message.dataMode.length = sprintf(sfModel->message.dataMode.payload,"Sent by SmartEverything");
    // readCachedGPSPosition();

    //point 3 SEND !!!!!!!!!!!
    sfModel->message.dataMode.sequenceNumber = getNewSequenceNumber();

    executeCDCSigFox(sfModel); 
}

/**
*
* This task is the board controller.
* This manage the SmartEverything controller as required
*
* \param params Parameters for the task. (Not used.)
*/
static bool blink = false;
static void control_task(void *params)
{
    //bool valueRead=false;
    controllerQueueS current_message;
    //uint8_t value;
    

    for(;;) {
        if (xQueueReceive(controllerQueue, &current_message, CONTROL_TASK_DELAY)) {
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
            blink != blink;
            port_pin_set_output_level(SME_LED_Y2_PIN, blink);
            sfxTimeOut();
        }
    }
}

