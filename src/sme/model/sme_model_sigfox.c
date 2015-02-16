/*
* s_sigfox.c
*
* Created: 1/22/2015 11:37:04 PM
*  Author: mfontane
*/


#include "sme_model_sigfox.h"
#include <string.h>


uint8_t sequenceNumber=1;
static sigFoxT usartMsg;
xSemaphoreHandle sf_sem;
#define SF_SEMAPHORE_DELAY  (1000 / portTICK_RATE_MS) // it could be to wait 4 Sec

// after a reset SigFx starts in data mode
sigFoxMessageTypeE sfxStatus=enterDataMode;

void initSigFoxModel(void){
    // create the sigFox semaphore
    sf_sem = xSemaphoreCreateMutex();
}

inline void setSfxStatus(sigFoxMessageTypeE state) {
    sfxStatus = state;
}


bool sfxIsInDataStatus(void) {
    return  ((sfxStatus==enterDataMode) ||
            (sfxStatus==dataIntMessage) || 
            (sfxStatus==dataCdcMessage) );
}

inline sigFoxMessageTypeE getSfxStatus(void) {
    return sfxStatus;
}

uint16_t calculateCRC(uint8_t length, uint8_t type, uint8_t sequenceNumber, uint8_t *payload){
    uint16_t crc = length;
    crc += type;
    crc += sequenceNumber;
    for(int i=0; i<length; i++){
        crc+=payload[i];
    }

    return crc;
}

sigFoxT* getSigFoxModel(void) {
    BaseType_t ok = xSemaphoreTake(sf_sem, SF_SEMAPHORE_DELAY);
    if (ok) {
        memset(&usartMsg.message.dataMode,0,sizeof(sigFoxDataMessage));
        return &usartMsg;
        } else {
        return NULL;
    }
}


void releaseSigFoxModel(void){
    xSemaphoreGive(sf_sem);
}