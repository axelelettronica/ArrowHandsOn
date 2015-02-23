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
#include "..\..\IO\sme_rgb_led.h"
#include "sme_sfx_timer.h"
#include "interrupt\interrupt.h"

typedef enum {
    headerRec,
    lenRec,
    typeRec,
    sequenceRec,
    payloadRec,
    crcRec,
    tailerRec,
    nullState // used when gotcha an error
} sfxRxFSME;


static sigFoxRxMessage answer;
static sfxRxFSME       recFsm=headerRec;
static uint8_t         crcCounter;


static void clearLed(bool error) {
    if (error) {
        sme_led_red_brightness(HALF_LIGTH);
        } else {
        sme_led_red_off();
    }
    
    sme_led_blue_off();
    sme_led_green_off();
    nurembergSent=false;
}

static sfxRxFSME crcCheck(void) {
    uint16_t crc = calculateCRC(answer.length, answer.type,
    answer.sequenceNumber, answer.payload);
    uint16_t *receivedCrc=answer.crc;

    if (*receivedCrc == crc) {
        return tailerRec;
    }
    else {
        clearLed(true);
        print_dbg("wrong crc = %X calculated = %x \r\n", *receivedCrc, crc);
        return nullState;
    }
}

static sfxRxFSME checkSequenceConsistence(uint8_t sequence) {
    
    for(int i=0; i<MAX_MESSAGE_OUT; i++ ) {
        if (sfxMessageIdx[i] == sequence){
            answer.sequenceNumber = sequence;
            answer.payloadPtr = 0;

            return payloadRec;
        }
    }
    
    clearLed(true);
    print_sfx ("find wrong Sequence = %X", sequence);
    print_sfx (" stored = %X, %X\n\r", sfxMessageIdx[0], sfxMessageIdx[1]);

    
    return nullState;
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
            recFsm = checkSequenceConsistence(msg[i]);
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
            // remove the charged timeout
            stopSfxCommandTimer();
                        
            recFsm = headerRec;
            if (SFX_MSG_TAILER == msg[i]){
                print_sfx("msg %0X completed received\n\r", answer.sequenceNumber);
                 clearLed(false);
                return SME_SFX_OK;                              
            } else
            return SME_SFX_KO;

            case nullState:
            // enter here in case of CRC error or swequence error
            // remove all the last incoming data
            // remove the charged timeout
            stopSfxCommandTimer();
            break;
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
            // remove the charged timeout
            stopSfxCommandTimer();

            // end found check if the answer is ok
            if (SGF_CONF_ERROR != answer.payload[0]) {
               clearLed(false);
                return SME_SFX_OK;
                } else {
                return SME_SFX_KO;
            }
        }
    }
    return SME_OK;
}

void resetRxFsm(void) {
    recFsm = headerRec;
}

uint8_t sfxHandleRx(uint8_t *msg, uint8_t msgMaxLen) {
    
    switch(sfxStatus){
        // first byte reset the payload ptr
        case factoryResert:
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