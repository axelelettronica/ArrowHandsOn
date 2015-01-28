
/*
* s_sigfox.c
*
* Created: 1/22/2015 11:37:04 PM
*  Author: mfontane
*/


#include "sme_model_sigfox.h"
#include <string.h>


uint8_t sequenceNumber;
static sigFoxT usartMsg;
xSemaphoreHandle sf_sem;
#define SF_SEMAPHORE_DELAY  (1000 / portTICK_RATE_MS)

void initSigFoxModel(void){
    // create the sigFox semaphore
    sf_sem = xSemaphoreCreateMutex();
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