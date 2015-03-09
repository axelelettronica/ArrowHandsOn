
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


#define SME_CTRL_COORD_LEN             9
#define SME_CTRL_MINUTES_START         2
#define SME_CTRL_DECIMALS_START        4
static uint8_t ctrl_lat[SME_CTRL_COORD_LEN]  = {'0','4','1','3','5','3','7','4','4'};
static uint8_t lat_direction = 1; // 1= north
static uint8_t ctrl_long[SME_CTRL_COORD_LEN] = {'0','0','2','1','2','8','1','9','3'};
static uint8_t long_direction = 1; // 1= East
static uint16_t ctrl_alt = 36;      
static uint8_t quality = 0;      // Fixed for now
static uint8_t n_satellites = 3; // Fixed for now

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
    uint8_t *dir = NULL;
    uint8_t out_len = 0;

    switch (type) {
    case SME_LAT:
        out = ctrl_lat;
        out_len = SME_CTRL_COORD_LEN;
        dir = &lat_direction;
        comma = 3;
    break;
    case SME_LONG:
        out = ctrl_long;
        out_len = SME_CTRL_COORD_LEN;
        dir = &long_direction;
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
        if (SME_LAT== type) {
            // Adjust first digit. Lat is only 8 digit while long is 9
            out[k++] = '0';
        }
        while (in[i] != ',') {
            if (in[i] != '.') { // decimals are always 6 digit, remove '.'
                out[k++] = in[i++]; 
            } else {
                i++;
            }
        }
        i++;
        // To be added and UT
        *dir = ((in[i] == 'N') || (in[i] == 'E')) ? 1 : 0;
    }
}

uint8_t *sl868v2_parse_param_offset(uint8_t in[], uint8_t in_len, uint8_t comma_num)
{
    bool comma_found = false;
    uint8_t i = 0, j = 0;

    do {    
        if (i >= in_len) {
            return NULL;
        }
        if (in[i] == ',') {
            j++;
        }
        if (j == comma_num) {
            comma_found = true;
        }
        i++;
    } while (!comma_found);

    if (comma_found) {
        return &in[i];
    }
    return NULL;
}

void sl868v2_parse_ssa(uint8_t in[], uint8_t in_len)
{
    uint8_t comma_alt = 9; // n. ',' before alt data
    uint8_t comma_nsat = 7;
    uint8_t comma_quality = 6;
    uint8_t *ptr;
 
    ptr = sl868v2_parse_param_offset(in, in_len, comma_quality);

    if (ptr && (*ptr != ',')) {
        quality = *ptr-'0';  // ascii
    }

    ptr = sl868v2_parse_param_offset(in, in_len, comma_nsat);
    n_satellites = 0;
    while (ptr && (*ptr != ',')) {
        n_satellites = (n_satellites *10) + (*ptr -'0'); // uint16_t
        ptr++;
    }

    ptr = sl868v2_parse_param_offset(in, in_len, comma_alt);
    ctrl_alt = 0;
    while (ptr && (*ptr != '.') && (*ptr != ',')) {
        ctrl_alt = (ctrl_alt *10) + (*ptr -'0'); // uint16_t
        ptr++;
    }
}



/*
 *  This function exports a pre formatted string with all read sensors
 *
 *    <Msgid><lat_data><long_data><altit><compact_data>
 *    - Msg Id       : 1 byte   (0x2)
 *    - Latitude data : 4 bytes  (0/1 (first nibble) + <Latitude> 28 bits)
 *    - Longitude data: 4 bytes  (0/1 (first nibble) + <Longitude> 28 bits)
 *    - Altitude      : 2 bytes  
 *    - quality_data (bitmap): 1 byte  [Quality | n. satellites] :  [4 bits | 4 bits]
 */
int  sme_sl868v2_get_latlong (char *msg, uint8_t *len, uint8_t msg_len)
{
    uint8_t i = 0, j = 0;;
    uint32_t long_lat = 0, long_longit=0;
    uint8_t  tmp_str[SME_CTRL_COORD_LEN] = {};
    uint16_t alt = (uint16_t)ctrl_alt;

    if (!msg || !len) {
        return SME_ERR;
    }

    for (j=0; j <SME_CTRL_COORD_LEN; ++j) {
        long_lat = (long_lat *10) + (ctrl_lat[j]-'0');
    }
    if (lat_direction) {
        long_lat |=  0x10000000;
    }
    for (j=0; j <SME_CTRL_COORD_LEN; ++j) {
        long_longit = (long_longit *10) + (ctrl_long[j]-'0');
    }
    if (long_direction) {
        long_longit |=  0x10000000;
    }

    j = 0;
    // Latitude 1 long
    for (i = 0; i < 4 ; ++i, ++j) {
        ((char *)msg)[j] = ((0xFF << (0x8*(3-i))) & long_lat) >> (0x8*(3-i));
    }
    // Longitude 1 long
    for (i = 0; i < 4 ; ++i, ++j) {
        ((char *)msg)[j] = ((0xFF << (0x8*(3-i))) & long_longit) >> (0x8*(3-i));
    }
    // Altitude  2-bytes
    for (i = 0; i < 2 ; ++i, ++j) {
        ((char *)msg)[j] = ((0xFF << (0x8*(1-i))) & alt) >> (0x8*(1-i));
    }
    ((char *)msg)[j] = ((0xF & quality)<<4) | (0xF & n_satellites);

    *len = 11;

    return SME_OK;
}