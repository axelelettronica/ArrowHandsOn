/*
* sme_sigfox_execute.c
*
* Created: 12/01/2015 20:43:25
*  Author: mfontane
*/
#include <stdlib.h>
#include "sme_sigfox_execute.h"
#include "port.h"
#include "sme\Devices\Uart\sigFox\sme_sigfox_usart.h"
#include "samd21_xplained_pro.h"
#include "portmacro.h"
#include "../../../sme_FreeRTOS.h"
#include "sme/model/sme_model_sigfox.h"
#include "sme_cdc_io.h"
#include "sme_sfx_timer.h"

/* ATSxxx=yy<cr>, where ‘xxx’ is the register address and ‘yy’ the register value (up
to 255, it depends to the available values).*/
#define  MAX_CONF_CHAR 10 // depend if the value for the register in write is HEX
uint8_t message[sizeof(sigFoxDataMessage)];


static void sendSigFoxMsg(const uint8_t *msg, uint8_t len) {
    print_sfx("send Sequence Number=");
    char seq[4];
    sprintf(seq, "%d", msg[SFX_SEQUENCE_POS]);
    print_sfx(seq);
    print_sfx("\r\n");

    sigfoxSendMessage(msg, len);
}

static bool sendSigFoxConfiguration(sigFoxMessageTypeE msgType, const sigFoxConfT *configuration) {

    startSfxCommandTimer();

    int msgLen = sprintf((char *)message, CONF_REGISTER);

    // set the RX FSM that SFX is in configuration mode
    setSfxStatus(msgType);

    switch (msgType){
        case enterConfMode:
        sendSigFoxMsg((uint8_t *)ENTER_CONF_MODE,  sizeof(ENTER_CONF_MODE)-1);
        break;

        case confCdcMessage:

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
        break;

        default:
        print_err("sendSigFoxConfiguration with wrong messageType");
        return false;
        break;
    }
    

    return true;
};

static uint8_t insertCRC(const sigFoxDataMessage *packet, uint8_t msgLen) {
    uint16_t crc = calculateCRC(packet->length, packet->type,
    packet->sequenceNumber, packet->payload);

    char* tmp = &crc;
    message[msgLen++] = tmp[0];
    message[msgLen++] = tmp[1];
    return 2;
}

bool sendSigFoxDataMessage(const sigFoxT *msg) {
    uint8_t msgLen=0;
    sigFoxDataMessage *packet;
    sigFoxMessageTypeE msgType = msg->messageType;

    stopSfxCommandTimer();

    // set the RX FSM that SFX is in data mode
    setSfxStatus(msgType);


    switch (msgType){
        case dataCdcMessage:
        case dataIntMessage:
        packet = &msg->message.dataMode;
        message[msgLen++] = SFX_MSG_HEADER;
        message[msgLen++] = packet->length;
        message[msgLen++] = packet->type;
        message[msgLen++] = packet->sequenceNumber;
        for (int i=0; i<packet->length; i++) {
            message[msgLen++] = packet->payload[i];
        }
        msgLen += insertCRC(packet, msgLen);
        message[msgLen++] = SFX_MSG_TAILER;


        // store the message Id that will be sent
        if (SIGFOX_DATA == packet->type)
        sfxMessageIdx[DATA_SEQUENCE_NUMBER] = packet->sequenceNumber;
        else
        sfxMessageIdx[KEEP_SEQUENCE_NUMBER] = packet->sequenceNumber;

        //finally SEND !!!
        sendSigFoxMsg(message, msgLen);
        break;
        
        case enterDataMode:
        sendSigFoxMsg((uint8_t *)ENTER_DATA_MODE, sizeof(ENTER_DATA_MODE)-1);
        break;
    }

    return true;
};

bool executeCDCSigFox(const sigFoxT *msg) {

    bool ret = false;
    switch (msg->messageType){
        
        case enterConfMode:
        case confCdcMessage:
        case confIntMessage:
            ret = sendSigFoxConfiguration(msg->messageType, &msg->message.confMode);
        break;
        
        case enterDataMode:
            // this is the only way to activate the possibility to sent a message
            setSfxStatus(msg->messageType);
        case dataCdcMessage:
        case dataIntMessage:
        // send the message only when are in data mode 
        // (activated by the enterDataMode message)
        if (sfxIsInDataStatus()) {
            ret = sendSigFoxDataMessage(msg);
        }
        break;

        default:
        //error print Help
        break;
    }
    
    // release the SFX model that had an error
    if ( ret == false)
        releaseSigFoxModel();

    return ret;
}