/*
* sme_sigfox_execute.c
*
* Created: 12/01/2015 20:43:25
*  Author: mfontane
*/
#include "sme_sigfox_execute.h"
#include "port.h"
#include "sme\Devices\Uart\sigFox\sme_sigfox_usart.h"
#include "samd21_xplained_pro.h"
#include "portmacro.h"
#include "../../../sme_FreeRTOS.h"

#define ENTER_CONF_MODE     "+++"
#define ENTER_DATA_MODE     "ATX"
#define CONF_REGISTER       "ATS"
#define SIGFOX_END_READ     '?'
#define SIGFOX_EQUAL_CHAR   '='
#define SIGFOX_END_MESSAGE  0xd

/* ATSxxx=yy<cr>, where ‘xxx’ is the register address and ‘yy’ the register value (up
to 255, it depends to the available values).*/
#define  MAX_CONF_CHAR 10 // depend if the value for the register in write is HEX
uint8_t message[sizeof(sigFoxDataMessage)];
uint8_t sequenceNumber;


static void sendSigFoxMsg(uint8_t *msg, uint8_t len) {
    port_pin_toggle_output_level(LED_0_PIN); // just for debug
    sigfoxSendMessage(msg, len);
}

static bool  sendSigFoxConfiguration(const sigFoxConfT *configuration) {
    int msgLen = sprintf(message, CONF_REGISTER);
    for (int i=0; i<SIG_FOX_MAX_REGISTER_LEN; i++) {
        message[msgLen++] = configuration->registerAddr[i];
    }

    if (SIGFOX_REGISTER_READ==configuration->access) {
        message[msgLen++] = SIGFOX_END_READ;
    } else {
        message[msgLen++] = SIGFOX_EQUAL_CHAR;
        for (int i=0; i<SIG_FOX_MAX_REG_VALUE_LEN; i++) {
            message[msgLen++] = configuration->data[i];
        }
    }
    
    // check if still into the maxchar
    if (msgLen >= MAX_CONF_CHAR)
    return true;

    message[msgLen++]= SIGFOX_END_MESSAGE;
    sendSigFoxMsg(message, msgLen);

    return true;
};

static bool  sendSigFoxDataMessage(const sigFoxDataMessage *packet) {
    int msgLen=0;
		
    message[msgLen++] = packet->header;
    message[msgLen++] = packet->length;
    message[msgLen++] = packet->type;
    message[msgLen++] = packet->sequenceNumber;
    for (int i=0; i<packet->length; i++) {
        message[msgLen++] = packet->payload[i];
    }
    message[msgLen++] = packet->crc;
    message[msgLen++] = packet->tailer;

    //finally SEND !!!
    sendSigFoxMsg(message, msgLen);
    return true;
};

bool executeSigFox(const sigFoxT *msg) {

    bool ret = true;
    switch (msg->messageType){
        
        case enterConfMode:
        sendSigFoxMsg(ENTER_CONF_MODE,  sizeof(ENTER_CONF_MODE)-1);
        break;
        
        case confMessage:
        ret = sendSigFoxConfiguration(&msg->message.confMode);
        break;
        
        case dataMessage:
        ret = sendSigFoxDataMessage(&msg->message.dataMode);
        break;
        
        case enterDataMode:
        sendSigFoxMsg(ENTER_DATA_MODE, sizeof(ENTER_DATA_MODE)-1);
        break;
        
        default:
        //error print Help
        break;
    }
    
    return ret;
}