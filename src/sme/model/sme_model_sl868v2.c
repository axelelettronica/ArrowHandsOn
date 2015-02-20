
/*
* sme_model_sl868v2.c
*
* Created:22/1/2015 11:37:04 PM
*  Author: speirano
*/


#include "sme_model_sl868v2.h"
#include "sme_cmn.h"

static sl868v2T gpsTxMsg;
xSemaphoreHandle gps_tx_sem;
#define GPS_SEMAPHORE_DELAY  (1000 / portTICK_RATE_MS)


void releaseSl868v2Model(void){
    xSemaphoreGive(gps_tx_sem);
}
void initSl868v2Model(void){
    // create the sigFox semaphore
    gps_tx_sem = xSemaphoreCreateMutex();
}

sl868v2T* getSl868v2Model(void) {
    BaseType_t ok = xSemaphoreTake(gps_tx_sem, GPS_SEMAPHORE_DELAY);

    if (ok) {
        return &gpsTxMsg;
        } else {
        return NULL;
    }
}



void sme_parse_coord(uint8_t in[], uint8_t in_len, uint8_t out[], 
                     uint8_t out_len, sme_coord_t type)
{
    bool comma_found = false;
    uint8_t comma = 0;
    uint8_t i = 0, j = 0, k = 0;;
   
    

    switch (type) {
    case SME_LAT:
        comma = 3;
    break;
    case SME_LONG:
        comma = 5;
    break;
    }
    while (i++) {
        if (i >= in_len) {
            return;
        }
        if (in[i] == ',') {
            j++;
        }
        if (j == comma) {
            comma_found = true;
            break;
        }
    }
    if (comma_found) {
        memset(out, 0, out_len);
        while (i != ',') {
            out[k++] = in[i++]; 
        }
    }
}