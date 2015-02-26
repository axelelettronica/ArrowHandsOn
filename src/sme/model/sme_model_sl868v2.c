
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


#define SME_CTRL_COORD_LEN             11
static uint8_t ctrl_lat[SME_CTRL_COORD_LEN]  = {'4', '5', '4', '0', '7', '0','1'};
static uint8_t ctrl_long[SME_CTRL_COORD_LEN] = {'0','0','9','2','3','5','8','9','2'};
    
    

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



void sme_parse_coord(uint8_t in[], uint8_t in_len, sme_coord_t type)
{
    bool comma_found = false;
    uint8_t comma = 0;
    uint8_t i = 0, j = 0, k = 0;
    uint8_t *out;
    uint8_t out_len = 0;
    

    switch (type) {
    case SME_LAT:
        out = ctrl_lat;
        out_len = SME_CTRL_COORD_LEN;
        comma = 3;
    break;
    case SME_LONG:
        out = ctrl_long;
        out_len = SME_CTRL_COORD_LEN;
        comma = 5;
    break;
    default: 
        print_err("No Lat/Long provided");
    }

    
    do {
                
        if (i >= in_len) {
            return;
        }
        if (in[i] == ',') {
            j++;
        }
        if (j == comma) {
            comma_found = true;
        }
        i++;
    } while (!comma_found);
    
    if (comma_found) {
        memset(out, 0, out_len);
        while (in[i] != ',') {
            out[k++] = in[i++]; 
        }
    }
}



/*
 *  This function exports a pre formatted string with all read sensors
 */
int  sme_sl868v2_get_latlong (char *msg, uint8_t *len, uint8_t msg_len)
{
    uint8_t offset = 0;
    long converted  = 0;
    uint8_t digit;
    uint8_t i = 0;
    uint8_t n_decimals= 0;
    bool    decimals = false;
    
    if (!msg || !len) {
        return SME_ERR;
    }


             digit = atol(&ctrl_lat[i]); 
           
    
    
    memcpy(msg, &digit, sizeof(long));
    
   
                digit = atol(&ctrl_long[i]);
              
    memcpy(msg+sizeof(long), &digit, sizeof(long));
    return SME_OK;
}