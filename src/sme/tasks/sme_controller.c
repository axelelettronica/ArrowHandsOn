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
#include "..\model\sme_model_i2c.h"
#include "..\Devices\uart\sigFox\sme_sigfox_execute.h"
#include "..\Devices\uart\sigFox\sme_sfx_timer.h"
#include "../devices/uart/gps/sme_sl868v2_execute.h"
#include "../interrupt/interrupt.h"
#include "../Devices/I2C/nfc/nxpNfc.h"
#include "Devices/I2C/IOExpander/tca6416a.h"

static void control_task(void *params);

#define KEEP_TIMEOUT 276 // number of %minute in 23Hour
xQueueHandle controllerQueue;
static int keepTimeout;

typedef enum {
    SME_SFX_DEBUG_MSG,
    SME_SFX_I2C_1_MSG,
    SME_SFX_GPS_1_MSG
}sme_sfx_report_t;

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


bool sme_sfx_fill_report (sme_sfx_report_t type, char *msg, uint8_t *len, uint8_t msg_len) {
    
    if (!msg || !len) {
        return SME_ERR;
    }
    switch (type) {
        case SME_SFX_DEBUG_MSG:
        ((char*)msg)[0] = SME_SFX_DEBUG_MSG;
        *len = sprintf(&(((char*)msg)[1]),"Smart");
        (*len)++; // considering msg ID field
        break;
        case SME_SFX_I2C_1_MSG:
        ((char*)msg)[0] = SME_SFX_I2C_1_MSG;
        sme_i2c_get_read_str(&(((char*)msg)[1]), len, msg_len-1);
        (*len)++; // considering msg ID field
        break;
        case SME_SFX_GPS_1_MSG:
            ((char*)msg)[0] = SME_SFX_GPS_1_MSG;
            sme_sl868v2_get_latlong(&(((char*)msg)[1]), len, msg_len-1);
            break;
    }
    
    return SME_OK;
}

/*
* First Use case
detect NFC interrupt:
1) take data from sensor
2) take GPS position
3) Send all to SigFox
*/
volatile  uint8_t data;
static void performExecution( uint16_t intDetection) {
    
    #if NOT_SENSOR
    
    // point 1
    LSM9DS1getValues((char *)&data);

    //point 3
    sendToSigFoxValue(data);

    #else
    // check which is the interrupt that wake-up the task
    if ((intDetection & MASK_NFC_FD_INT) == MASK_NFC_FD_INT) {
        data++;
    }
    #endif

    
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



/*
* First Use case
detect NFC interrupt:
1) take data from sensor
2) take GPS position
3) Send all to SigFox
*/
static void button1Execution(void) {
    
    TCA6416a_gps_force_on();
    
    char *msg = NULL;
    char msg_len = 0;
    sigFoxT *sfModel = getSigFoxModel();

    sfModel->messageType = dataIntMessage;
    sfModel->message.dataMode.type = SIGFOX_DATA;

    // point 1
    //sfModel->message.dataMode.length = sprintf(sfModel->message.dataMode.payload,"Smart");
    sme_sfx_fill_report(SME_SFX_GPS_1_MSG, sfModel->message.dataMode.payload,
    &sfModel->message.dataMode.length, 12);
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
static void button2Execution(void) {
   /* sigFoxT *sfModel = getSigFoxModel();
    

    sfModel->messageType = dataIntMessage;
    sfModel->message.dataMode.type = SIGFOX_DATA;

    // point 1
    sfModel->message.dataMode.length = sprintf(sfModel->message.dataMode.payload,"Sent by SmartEverything");

    //point 3 SEND !!!!!!!!!!!
    sfModel->message.dataMode.sequenceNumber = getNewSequenceNumber();

    executeCDCSigFox(sfModel);*/
   button1Execution(); // just for Nuremberg

}


#if DEBUG_SIGFOX
char debugSFXMsg[100];
uint8_t msgCounter=0;
uint8_t   timeoutCounter=2;

static void debugSigFox(void){
    timeoutCounter++;
    if (timeoutCounter==3) {
        timeoutCounter =0;
        msgCounter++;
        print_sfx("Sending DEBUG MSG\n\r");
        button1Execution();
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
* Third Use case
detect NFC interrupt:
1) take data from sensor
2) take GPS position
3) Send all to SigFox
*/
static void nfcExecution(void) {
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
static void sme_gps_data_updateExecution(void) {
    uint8_t data;
    sigFoxT *sfModel = getSigFoxModel();

    sfModel->messageType = dataIntMessage;
    sfModel->message.dataMode.type = SIGFOX_DATA;

    // point 1
    sfModel->message.dataMode.length = sprintf(sfModel->message.dataMode.payload,"Sent by SmartEverything");
    //readCachedGPSPosition();

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


