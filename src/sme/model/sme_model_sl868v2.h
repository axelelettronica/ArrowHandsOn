//#include <stdint-gcc.h>
/*
 * sme_model_sl868v2.h
 *
 * Created: 22/1/2015 11:18:32 PM
 *  Author: speirano
 */ 


#ifndef SME_MODEL_SL868V2_H_
#define SME_MODEL_SL868V2_H_



/*
#define SIGFOX_REGISTER_READ 'R'
#define SIGFOX_REGISTER_WRITE 'W'

#define SIGFOX_DATA     0x01
#define SIGFOX_KEEP     0x02
#define SIGFOX_BIT      0x03
#define SIGFOX_CONFIRM  0x10
#define SIGFOX_HEADER	0xA5
#define SIGFOX_TAILER	0x5A

typedef enum {
    enterConfMode,
    enterDataMode,
    confMessage,
    dataMessage
} sigFoxMessageTypeE;
*/

/* see
http://www.adaptivemodules.com/assets/telit/Telit-LE51-868-S-Software-User-Guide-r1.pdf
*
* In order to get a register value, the user shall send:
* ATSxxx?< cr>, where ‘xxx’ is the register address.
* The answer will be: Sxxx=yy<cr>, where ‘xxx’ is the register address and ‘yy’ the
* register value (up to 255, it depends to the available values).*
* In order to set a new register value, the user shall send:
* ATSxxx=yy<cr>, where ‘xxx’ is the register address and ‘yy’ the register value (up
* to 255, it depends to the available values).
*

#define SIG_FOX_MAX_REGISTER_LEN 0x3
#define SIG_FOX_MAX_REG_VALUE_LEN 0x2
typedef struct {
    uint8_t access; // R/W
    uint8_t registerAddr[SIG_FOX_MAX_REGISTER_LEN];
    uint8_t data[SIG_FOX_MAX_REG_VALUE_LEN];
}sigFoxConfT;

#define SIG_FOX_MAX_PAYLOAD 0xff
typedef struct {
    uint8_t header; //maybe not needed if always 0xA5
    uint8_t length;
    uint8_t type;
    uint8_t sequenceNumber; //0x1 to 0xff
    uint8_t payload[SIG_FOX_MAX_PAYLOAD];
    uint8_t crc;
    uint8_t tailer;
}sigFoxDataMessage;
*/


#include "sme/sme_FreeRTOS.h"

//extern xSemaphoreHandle gps_rx_sem;


#define STD_NMEA_MAX_DATA_LEN 76
#define MTK_NMEA_MAX_DATA_LEN 242

typedef struct {

    uint8_t talker[2];
    uint8_t sentenceId[3];
    uint8_t data[STD_NMEA_MAX_DATA_LEN];
    uint8_t dataLenght;

} sl868v2StdMsgT;

typedef struct {

    uint8_t talker[4];
    uint8_t msgId[3];
    uint8_t data[MTK_NMEA_MAX_DATA_LEN];
    uint8_t dataLenght;

} sl868v2MTKMsgT;

typedef union {
    sl868v2StdMsgT std;  
    sl868v2MTKMsgT mtk;
} sl868v2MsgT;

typedef enum {
    STD_NMEA,
    MTK_NMEA
} sl868v2MsgE;

typedef struct {
    sl868v2MsgE     messageType;
    sl868v2MsgT     nmea_msg;
} sl868v2T;


void releaseSl868v2Model(void);
sl868v2T* getSl868v2Model(void);
void initSl868v2Model(void);

/* TX Messagese */
#define  SL868V2_SET_STDBY_CMD    "$PMTK161,0*28\r\n"    // Set standby
#define  SL868V2_HOT_RST_CMD      "$PMTK101*32\r\n"      // HOT RESTART
#define  SL868V2_WARM_RST_CMD     "$PMTK102*31\r\n"      // WARM Restart
#define  SL868V2_COLD_RST_CMD     "$PMTK103*30\r\n"      // Cold restart
#define  SL868V2_FCOLD_RST_CMD    "$PMTK104*37\r\n"      // Full cold restart
#define  SL868V2_CLR_FLASH_CMD    "$PMTK120*31\r\n"      // clear flash
#define  SL868V2_CLEAR_ORBIT_CMD  "$PMTK127*36\r\n"      // clear predicted orbit data
#define  SL868V2_SET_FIXINT_CMD   "$PMTK220,1000*1F\r\n" // set position fix interval to 1000 

#define SL868V2_CONF_CMD "PMTK"

typedef enum {
    SME_LAT,
    SME_LONG,
    SME_ALT,
    SME_COORD_UNDEF
} sme_coord_t;

void sme_parse_coord(uint8_t in[], uint8_t in_len, sme_coord_t type);
int  sme_sl868v2_get_latlong (char *msg, uint8_t *len, uint8_t msg_len);

#endif /* SME_MODEL_SL868V2_H_ */