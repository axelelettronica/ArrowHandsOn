/*
* sme_sigfox_execute.h
*
* Created: 12/01/2015 20:43:40
*  Author: mfontane
*
* This the file that execute  the command on sigfox component according with
* the information are present on the sigFoxT structure
*/


#ifndef SME_SIGFOX_EXECUTE_H_
#define SME_SIGFOX_EXECUTE_H_
#include <stdint-gcc.h>
#include <stdbool.h>


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

typedef union {
    sigFoxConfT       confMode;
    sigFoxDataMessage dataMode;
}sigFoxMessageU;

typedef struct {
    sigFoxMessageTypeE messageType;
    sigFoxMessageU     message;
}sigFoxT;


extern uint8_t sequenceNumber;

inline uint8_t getNewSequenceNumber(void) {
    return sequenceNumber++;
}

bool executeSigFox(const sigFoxT *msg);
#endif /* SME_SIGFOX_EXECUTE_H_ */