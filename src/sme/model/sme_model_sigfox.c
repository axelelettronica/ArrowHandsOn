/*
* s_sigfox.c
*
* Created: 1/22/2015 11:37:04 PM
*  Author: smkk
*/


#include "sme_model_sigfox.h"
#include <string.h>
#include "common/sme_timer_define.h"


uint8_t sequenceNumber;
static sigFoxT usartMsg;
xSemaphoreHandle sf_sem; 

#define SF_SEMAPHORE_DELAY  ONE_SECOND

// after a reset SigFx starts in data mode
sigFoxMessageTypeE sfxStatus=enterDataMode;


uint8_t getNewSequenceNumber(void) {
    sequenceNumber++;
    
    if (sequenceNumber ==0)
    sequenceNumber =1;

    return sequenceNumber;
}


void initSigFoxModel(void){
    sequenceNumber=1;
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

uint16_t calculateCRC(uint8_t length, uint8_t type, 
                      uint8_t sequence, 
                      uint8_t *payload){
    uint16_t crc = length;
    crc += type;
    crc += sequence;
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
