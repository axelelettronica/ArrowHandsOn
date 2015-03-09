/*
* sme_decode_i2c_sfx.c
*
* Created: 3/7/2015 8:39:57 AM
*  Author: mfontane
*/
#include <string.h>
#include "sme_decode_i2c_sfx.h"
#include "Devices\I2C\nfc\nxpNfc.h"
#include "sme_cdc_io.h"
#include "Devices\I2C\nfc\model\NT3H1101_model.h"
#include "Devices\I2C\Pressure\LPS25H.h"
#include "Devices\I2C\Humidity\HTS221.h"
#include "sme_cmn.h"
#include "model\sme_model_i2c.h"
#include "Devices\I2C\Accelerometer\LSM9DS1.h"

#define NFC_PAGE_VL6180X_LUX  1
#define NFC_PAGE_VL6180X_PROX 2



int  vl6180x_get_ALS_PROX_str (char *msg, uint8_t msg_len, uint8_t *sensorValue){

    if (!msg || !sensorValue || msg_len < 5) {
        return 0;
    }
    
    int i=0;
    for (i=0; i<5; i++) {
        msg[i] = sensorValue[i];
    }
    
    return i; // return length of copied bytes
}



/*
 *  This function exports a pre formatted string with all read sensors
 */
int  sme_i2c_get_press_hum_read_str (char* buffer, char *msg, uint8_t *len, uint8_t msg_len)
{
    uint8_t offset = 0;
    uint16_t decodedData1, decodedData2;

    if (!msg || !len) {
        return SME_ERR;
    }

    LPS25HgetValues(buffer);
    LPS25HDecode(buffer, &decodedData1, &decodedData2);

    // Writing Temperature, pressure
    *len = sprintf(msg, "%02d%04d", (decodedData1)/10, decodedData2);

    HTS221getValues(buffer);
    HTS221Decode(buffer, &decodedData1, &decodedData2);
    offset += *len;
    // Writing Humidity
    *len += sprintf((msg+offset), "%2d", (decodedData2)/10);

    return SME_OK;
}

static /*
 *  This function exports a formatted string with Magnetic sensor info
 *
 *    <Msgid><X-Axis><Y-Axis><Z-Axis>
 *    - Msg Id  : 1 byte   (0x5)
 *    - X-Axis  : 2 bytes  in milli gauss
 *    - Y-Axis  : 2 bytes  in milli gauss
 *    - Z-Axis  : 2 bytes  in milli gauss
 */
int  LSM9DS1_get_AGM_str (char *msg, uint8_t *len, uint8_t msg_len, 
                          sensorTaskStr *sensor, float sensitivity)
{  
    float coord_f = 0;
    uint8_t i = 0, j = 0;

    if (!msg || !len || !sensor) {
        return SME_ERR;
    }

    coord_f = ((int16_t)(sensor->decodedData1)*LSM9DS1_get_g_lsb_sentivity());
    // copy x-axis 2-bytes in the right order
    for (i = 0; i < 2 ; ++i, ++j) {
        ((char *)msg)[j] = ((0xFF << (0x8*(1-i))) & ((uint16_t)coord_f)) >> (0x8*(1-i));
    }

    coord_f = ((int16_t)(sensor->decodedData2)*LSM9DS1_get_g_lsb_sentivity());
    // copy y-axis 2-bytes in the right order
    for (i = 0; i < 2 ; ++i, ++j) {
        ((char *)msg)[j] = ((0xFF << (0x8*(1-i))) & ((uint16_t)coord_f)) >> (0x8*(1-i));
    }

    coord_f = ((int16_t)(sensor->decodedData3)*LSM9DS1_get_g_lsb_sentivity());
    // copy y-axis 2-bytes in the right order
    for (i = 0; i < 2 ; ++i, ++j) {
        ((char *)msg)[j] = ((0xFF << (0x8*(1-i))) & ((uint16_t)coord_f)) >> (0x8*(1-i));
    }

    *len = j;
    return SME_OK;
}

int  LSM9DS1_get_A_str (char *msg, uint8_t *len,
uint8_t msg_len, void *sensor)
{
    return LSM9DS1_get_AGM_str(msg, len, msg_len,
    (sensorTaskStr *)sensor, LSM9DS1_get_a_lsb_sensitivity());
}


int LSM9DS1_get_G_str (char *msg, uint8_t *len,
uint8_t msg_len, void *sensor)
{
    return LSM9DS1_get_AGM_str(msg, len, msg_len,
    (sensorTaskStr *)sensor, LSM9DS1_get_g_lsb_sentivity());
}

/*
 *  This function exports a formatted string with Magnetic sensor info
 *
 *    <Msgid><X-Axis><Y-Axis><Z-Axis>
 *    - Msg Id  : 1 byte   (0x5)
 *    - X-Axis  : 2 bytes  in milli gauss
 *    - Y-Axis  : 2 bytes  in milli gauss
 *    - Z-Axis  : 2 bytes  in milli gauss
 */
int  LSM9DS1_get_M_str (char *msg, uint8_t *len, uint8_t msg_len, void *sensor)
{  
    return LSM9DS1_get_AGM_str(msg, len, msg_len, 
                               (sensorTaskStr *)sensor, LSM9DS1_get_m_lsb_sentivity());
}