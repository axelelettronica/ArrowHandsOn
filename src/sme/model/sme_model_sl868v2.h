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

extern xSemaphoreHandle gps_sem;


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
#endif /* SME_MODEL_SL868V2_H_ */