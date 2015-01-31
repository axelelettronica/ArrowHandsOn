
/*
* sme_model_sl868v2.c
*
* Created:22/1/2015 11:37:04 PM
*  Author: speirano
*/


#include "sme_model_sl868v2.h"


static sl868v2T gpsUsartMsg;
xSemaphoreHandle gps_cdc_sem;
#define GPS_SEMAPHORE_DELAY  (1000 / portTICK_RATE_MS)


void releaseSl868v2Model(void){
    xSemaphoreGive(gps_cdc_sem);
}
void initSl868v2Model(void){
    // create the sigFox semaphore
    gps_cdc_sem = xSemaphoreCreateMutex();
}

sl868v2T* getSl868v2Model(void) {
    BaseType_t ok = xSemaphoreTake(gps_cdc_sem, GPS_SEMAPHORE_DELAY);

    if (ok) {
        return &gpsUsartMsg;
        } else {
        return NULL;
    }
}