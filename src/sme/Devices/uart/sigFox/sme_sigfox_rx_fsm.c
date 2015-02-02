/*
* sme_sigfox_rx_fsm.c
*
* Created: 1/25/2015 8:42:56 PM
*  Author: mfontane
*/
#include "sme_sigfox_rx_fsm.h"
#include "sme\model\sme_model_sigfox.h"
#include "sme_cmn.h"
#include "sme_sigfox_usart.h"

typedef enum {
    headerRec,
    lenRec,
    typeRec,
    sequenceRec,
    payloadRec,
    crcRec,
    tailerRec
} sfxRxFSME;


static sigFoxRxMessage answer;
static sfxRxFSME       recFsm;
static uint8_t         crcCounter;


static sfxRxFSME crcCheck(void) {
    uint16_t crc = calculateCRC(answer.length, answer.type,
    answer.sequenceNumber, answer.payload);
    uint16_t *receivedCrc=answer.crc;

    if (*receivedCrc == crc) {
        return tailerRec;
    }
    else {
        print_dbg("wrong crc = %X calculated = %x \r\n", *receivedCrc, crc);
        return headerRec;
    }
}

static uint8_t checkSequenceConsistence(uint8_t sequence) {
    char seq[4];
    for(int i=0; i<MAX_MESSAGE_OUT; i++ ) {
        if (sfxMessageIdx[i] == sequence){
            answer.sequenceNumber = sequence;
            answer.payloadPtr = 0;
            recFsm = payloadRec;

            return SME_SFX_OK;
        }
    }

    print_sfx ("find wrong Sequence = %X", sequence);
    print_sfx (" stored = %X, %X\n\r", sfxMessageIdx[0], sfxMessageIdx[1]);

    return SME_SFX_KO;
}

static uint8_t handleData(uint8_t *msg, uint8_t msgMaxLen) {
    for (int i=0; i<msgMaxLen; i++) {
        print_sfx("%X ", msg[i]);
        switch (recFsm) {
            case headerRec:
            if (SFX_MSG_HEADER != msg[i])
            return SME_SFX_KO;
            else
            recFsm = lenRec;
            break;

            case lenRec:
            answer.length = msg[i];
            recFsm = typeRec;
            break;

            case typeRec:            
            answer.type  = msg[i];
            recFsm = sequenceRec;
            break;

            case sequenceRec:
            checkSequenceConsistence(msg[i]);
            break;

            case payloadRec:
            answer.payload[answer.payloadPtr++]= msg[i];
            if (answer.payloadPtr == answer.length) {
                
                crcCounter =0;
                recFsm = crcRec;
            }
            break;

            case crcRec:
            answer.crc[crcCounter++] = msg[i];
            if (crcCounter == 2)
            recFsm = crcCheck();
            break;

            case tailerRec:
            
            recFsm = headerRec;
            if (SFX_MSG_TAILER == msg[i])
            return SME_SFX_OK;
            else
            return SME_SFX_KO;
        }
    }
    return SME_OK;
}

static uint8_t handleConf(uint8_t *msg, uint8_t msgMaxLen) {
    for (int i=0; i < msgMaxLen; i++) {
        // store all the received bye till the end of message
        if (msg[i] != SIGFOX_END_MESSAGE) {
            answer.payload[answer.payloadPtr++]= msg[i];
            print_dbg(&msg[i]);
            } else {
            // end found check if the answer is ok
            if (SGF_CONF_ERROR != answer.payload[0]) {
                return SME_SFX_OK;
                } else {
                return SME_SFX_KO;
            }
        }
    }
    return SME_OK;
}

uint8_t sfxHandleRx(uint8_t *msg, uint8_t msgMaxLen) {
    
    switch(sfxStatus){
        // first byte reset the payload ptr
        case enterConfMode:
        case confCdcMessage:
        case enterDataMode:
        // move the FSM to idle when finis is OK or ERROR
        if (SME_OK != handleConf(msg, msgMaxLen)) {
            answer.payloadPtr=0;
        }
        return SME_OK;
        
        
        case dataCdcMessage:
        case dataIntMessage:
        if (SME_OK != handleData(msg, msgMaxLen)) {
            answer.payloadPtr=0;
        }
        return SME_OK;

        default:
        return SME_EINVAL;
    }
    return SME_EINVAL;
}