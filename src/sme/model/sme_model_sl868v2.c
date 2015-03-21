
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
#define SME_CTRL_LAT_MINUTES_START     2
#define SME_CTRL_LONG_MINUTES_START    3

typedef struct {
   uint16_t  lat_deg;
   uint32_t lat_decimals;  // always 6 digit, measurement = 1/100000
   uint8_t  lat_direction; // 1= north/+,  0 = South/-
   uint16_t  longit_deg;
   uint32_t longit_decimals;  //always 6 digit, measurement = 1/100000
   uint8_t  longit_direction; // 1= East/+, 0 = West/-
   uint16_t altitude;
   uint8_t  quality;
   uint8_t  n_satellites;
}sl868v2CachedDataT;

static sl868v2CachedDataT gpsRxData = {};

bool sme_gps_position_fixed()
{
    return (gpsRxData.quality && (gpsRxData.n_satellites > 4));
}


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



void sme_parse_rmc(uint8_t in[], uint8_t in_len, sme_coord_t type)
{
    bool comma_found = false;
    uint8_t comma = 0;
    uint8_t i = 0, j = 0, k = 0;
    uint8_t *out;
    uint8_t *dir = NULL;
    uint8_t out_len = 0;
    uint8_t *degree;
    uint16_t *decimals_p;
    uint64_t tmp_decimals = 0;
    uint8_t first_minute_idx = 0;
    uint16_t lower_val = 0, tmp_decimals_leftover = 0;

    switch (type) {
    case SME_LAT:
        degree = &gpsRxData.lat_deg;
        decimals_p = &gpsRxData.lat_decimals;
        out_len = SME_CTRL_COORD_LEN;
        dir = &gpsRxData.lat_direction;
        first_minute_idx = SME_CTRL_LAT_MINUTES_START;
        comma = 3;
    break;
    case SME_LONG:
        degree = &gpsRxData.longit_deg;
        out_len = SME_CTRL_COORD_LEN;
        dir = &gpsRxData.longit_direction;
        first_minute_idx = SME_CTRL_LONG_MINUTES_START;
        decimals_p = &gpsRxData.longit_decimals;
        comma = 5;
    break;
    default: 
        print_err("No Lat/Long provided\n");
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
        // storing degree
        *degree = 0;
        while (i < first_minute_idx)  {
            degree = ((*degree)*10) + (in[i++] - '0');
        }
        // storing decimals
        while (in[i] != '.') {
            if (in[i] == ',') {
                print_err("lat Wrong data\n");
                return;
            }
            tmp_decimals |= (tmp_decimals *10) + (in[i++] - '0');
        }
        // at . convert degree into decimals
        tmp_decimals = (tmp_decimals*10000*10/6);
        tmp_decimals_leftover = (tmp_decimals*10000*10%6);
        i++;
        while (in[i] != ',') {
            lower_val |= (lower_val *10) + (in[i++] - '0');
        }
        tmp_decimals += tmp_decimals_leftover + lower_val;
        *decimals_p = tmp_decimals;

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



void sme_parse_coord(uint8_t in[], uint8_t in_len, sme_coord_t type)
{
    uint8_t comma = 0, i = 0;
    uint16_t *deg_p = NULL;
    uint32_t *decimals_p = NULL;
    uint8_t *direction_p = NULL;
    uint8_t *ptr = NULL;
    uint8_t first_minute_idx = 0;
    uint64_t minutes_tmp = 0;
    uint64_t decimals_tmp = 0;
    uint64_t rest_tmp = 0;

    switch (type) {
        case SME_LAT:
            deg_p = &gpsRxData.lat_deg;
            decimals_p = &gpsRxData.lat_decimals;
            direction_p = &gpsRxData.lat_direction;
            comma = 3;
            // lat has deg in 2 chars format: ddmm.mmmmm
            first_minute_idx = SME_CTRL_LAT_MINUTES_START;
            
        break;
        case SME_LONG:
            deg_p = &gpsRxData.longit_deg;
            decimals_p = &gpsRxData.longit_decimals;
            direction_p = &gpsRxData.longit_direction;
            // lat has deg in 3 chars format: dddmm.mmmmm
            first_minute_idx = SME_CTRL_LONG_MINUTES_START;
            comma = 5;
        break;
        default:
            print_err("No Lat/Long provided\n");
    }

    ptr = sl868v2_parse_param_offset(in, in_len, comma);

    if (!ptr || (*ptr == ',')) {
        return; // data not valid
    }
      
    // degrees
    *deg_p = 0; 
    for (i = 0; i < first_minute_idx; ++i) {
        *deg_p = ((*deg_p)*10) + (*ptr -'0'); // uint16_t
        ptr++;
    }

    minutes_tmp = 0;
    // minutes
    while (ptr && (*ptr != '.')) {
        minutes_tmp = ((minutes_tmp)*10) + (*ptr -'0'); // uint16_t
        ptr++;
    }
    // convert minutes in decimals *10000
    decimals_tmp = ((minutes_tmp * 10000)*10) /6;

    ptr++;

    // minutes decimals
    *decimals_p = 0;
    while (ptr && (*ptr != ',')) {
        *decimals_p = ((*decimals_p)*10) + (*ptr -'0'); // uint16_t
        ptr++;
    }
    // convert minute decimals from minutes in decimals 
    *decimals_p = (*decimals_p)*10/6;

    // get the rest of the 
    
    /* complete conversion 
     * from:  DDmm.dddd (D = degree, m = minutes, d = decimals of minutes, all are in 1/60)
     * to  :  DDdddddd     [D are in 1/60, d are in decimals of Degree]
     */
     // calculate the whole 'dddddd'
     *decimals_p = *decimals_p + decimals_tmp;

     ptr++;
     // To be added and UT
     *direction_p = ((*ptr == 'N') || (*ptr == 'E')) ? 1 : 0;
}

void sl868v2_parse_ssa(uint8_t in[], uint8_t in_len)
{
    uint8_t comma_alt = 9; // n. ',' before alt data
    uint8_t comma_nsat = 7;
    uint8_t comma_quality = 6;
    uint8_t *ptr;
 
    ptr = sl868v2_parse_param_offset(in, in_len, comma_quality);

    if (ptr && (*ptr != ',')) {
        gpsRxData.quality = *ptr-'0';  // ascii
    }

    ptr = sl868v2_parse_param_offset(in, in_len, comma_nsat);
    gpsRxData.n_satellites = 0;
    while (ptr && (*ptr != ',')) {
        gpsRxData.n_satellites = (gpsRxData.n_satellites *10) + (*ptr -'0'); // uint16_t
        ptr++;
    }

    ptr = sl868v2_parse_param_offset(in, in_len, comma_alt);
    gpsRxData.altitude = 0;
    while (ptr && (*ptr != '.') && (*ptr != ',')) {
        gpsRxData.altitude = (gpsRxData.altitude *10) + (*ptr -'0'); // uint16_t
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
    uint16_t alt = (uint16_t)gpsRxData.altitude;

    if (!msg || !len) {
        return SME_ERR;
    }

    long_lat = (gpsRxData.lat_deg)*1000000+gpsRxData.lat_decimals;
    if (gpsRxData.lat_direction) {
        long_lat |=  0x10000000;
    }

    long_longit = (gpsRxData.longit_deg)*1000000+gpsRxData.longit_decimals;
    if (gpsRxData.longit_direction) {
        long_longit |=  0x10000000;
    }

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
    ((char *)msg)[j] = ((0xF & gpsRxData.quality)<<4) | (0xF & gpsRxData.n_satellites);

    *len = 11;

    return SME_OK;
}