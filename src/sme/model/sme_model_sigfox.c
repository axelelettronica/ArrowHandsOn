
/*
* s_sigfox.c
*
* Created: 1/22/2015 11:37:04 PM
*  Author: mfontane
*/


#include "sme_model_sigfox.h"


static sigFoxT usartMsg;
xSemaphoreHandle sf_sem;
#define SF_SEMAPHORE_DELAY  (1000 / portTICK_RATE_MS)

void initSigFoxModel(void){
    // create the sigFox semaphore
    sf_sem = xSemaphoreCreateMutex();
}

sigFoxT* getSigFoxModel(void) {
    BaseType_t ok = xSemaphoreTake(sf_sem, SF_SEMAPHORE_DELAY);
    if (ok) {
        return &usartMsg;
        } else {
        return NULL;
    }
}