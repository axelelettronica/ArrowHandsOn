#include <stdint-gcc.h>
/*
* sme_model_sigfox.h
*
* Created: 1/22/2015 11:18:32 PM
*  Author: mfontane
*/


#ifndef SME_MODEL_SIGFOX_H_
#define SME_MODEL_SIGFOX_H_

#define SIGFOX_REGISTER_READ 'R'
#define SIGFOX_REGISTER_WRITE 'W'

#define ENTER_CONF_MODE     "+++"
#define ENTER_DATA_MODE     "ATX\r"
#define CONF_REGISTER       "ATS"
#define SIGFOX_END_READ     '?'
#define SIGFOX_EQUAL_CHAR   '='
#define SIGFOX_END_MESSAGE  0xd

#define SFX_MSG_HEADER      0xA5
#define SFX_MSG_TAILER      0x5A
#define SGF_CONF_OK         "O" // OK
#define SGF_CONF_ERROR      "E" // ERROR

#define SIGFOX_DATA     0x01
#define SIGFOX_KEEP     0x02
#define SIGFOX_BIT      0x03
#define SIGFOX_CONFIRM  0x10


typedef enum {
    enterConfMode,
    enterDataMode,

    confCdcMessage,
    dataCdcMessage,

    confIntMessage,
    dataIntMessage
} sigFoxMessageTypeE;


extern sigFoxMessageTypeE sfxStatus;

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
*/

typedef enum
{
    NO_ERROR = 0,
    SERIAL_LENGTH_ERROR,
    SERIAL_CRC_ERROR,
    SERIAL_TAILER_MISS_ERROR,
    SERIAL_TIMEOUT_ERROR,
    SERIAL_GENERIC_ERROR,
    SFX_INIT_ERROR,
    SFX_SEND_ERROR,
    SFX_CLOSE_ERROR} SFX_ACK;

#define SIG_FOX_MAX_REGISTER_LEN 0x3
#define SIG_FOX_MAX_REG_VALUE_LEN 0x2
typedef struct {
    uint8_t access; // R/W
    uint8_t registerAddr[SIG_FOX_MAX_REGISTER_LEN];
    uint8_t data[SIG_FOX_MAX_REG_VALUE_LEN];
}sigFoxConfT;

#define SIG_FOX_MAX_PAYLOAD 0xff

typedef struct {
    uint8_t length;
    uint8_t type;
    uint8_t sequenceNumber; //0x1 to 0xff
    uint8_t payload[SIG_FOX_MAX_PAYLOAD];
}sigFoxDataMessage;

typedef union {
    sigFoxConfT       confMode;
    sigFoxDataMessage dataMode;
}sigFoxMessageU;

typedef struct {
    sigFoxMessageTypeE messageType;
    sigFoxMessageU     message;
}sigFoxT;


#define SFX_ANSWER_LEN 6 // considering the max payload that could be ERROR in Conf mode
typedef struct {
    uint8_t payloadPtr; //maybe not needed if always 0xA5
    uint8_t length;
    uint8_t type;
    uint8_t sequenceNumber; //0x1 to 0xff
    uint8_t payload[SFX_ANSWER_LEN];
    uint8_t crc[2];
    uint8_t tailer;
}sigFoxRxMessage;

#include "sme/sme_FreeRTOS.h"

extern uint8_t sequenceNumber;

inline uint8_t getNewSequenceNumber(void) {
    sequenceNumber++;

    if (sequenceNumber ==0)
        sequenceNumber =1;

    return sequenceNumber;
}


sigFoxT* getSigFoxModel(void);
void releaseSigFoxModel(void);
void initSigFoxModel(void);
uint16_t calculateCRC(uint8_t length, uint8_t type, uint8_t sequenceNumber, uint8_t *payload);


void setSfxStatus(sigFoxMessageTypeE state);
sigFoxMessageTypeE getSfxStatus(void);
bool sfxIsInDataStatus(void);

#endif /* SME_MODEL_SIGFOX_H_ */