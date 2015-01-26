/*
* sme_sigfox_rx_fsm.c
*
* Created: 1/25/2015 8:42:56 PM
*  Author: mfontane
*/
#include "sme_sigfox_rx_fsm.h"
#include "sme\model\sme_model_sigfox.h"
#include "sme_cmn.h"


sigFoxMessageTypeE sgfFsm=0;
sigFoxRxMessage answer;

inline void set_sgf_fsm(sigFoxMessageTypeE state) {
    sgfFsm = state;
}

static uint8_t handleConf(uint8_t *msg, uint8_t msgMaxLen) {
    for (int i=0; i < msgMaxLen; i++) {
        // store all the received bye till the end of message
        if (msg[i] != SIGFOX_END_MESSAGE) {
            answer.payload[answer.payloadPtr++] = msg[i];
            } else {
            // end found check if the answer is ok
            if (SGF_CONF_OK == answer.payload[0]) {
                return SME_SFX_OK;
                } else {
                return SME_SFX_KO;
            }
        }
    }
    return SME_OK;
}


uint8_t sfxHandleRx(uint8_t *msg, uint8_t msgMaxLen) {
    
    switch(sgfFsm){
        // first byte reset the payload ptr
        case enterConfMode:
        answer.payloadPtr=0;
        case confCdcMessage:
        return handleConf(msg, msgMaxLen);

        case enterDataMode:
        case dataCdcMessage:
        return SME_OK;

        default:
        return SME_EINVAL;
    }
    return SME_EINVAL;
}