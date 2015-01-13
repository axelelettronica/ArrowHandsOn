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

typedef enum {
	confMessage,
	dataMessage
} sigFoxMessageTypeE;


/* see
http://www.adaptivemodules.com/assets/telit/Telit-LE51-868-S-Software-User-Guide-r1.pdf
*/
#define SIG_FOX_MAX_PAYLOAD 0xff
typedef union {
	struct {
		uint8_t access; // R/W
		uint8_t registerAddr;
		uint8_t data;
	}sigFoxConfT;
	
	struct {
		uint8_t Header; //maybe not needed if always 0xA5
		uint8_t length;
		uint8_t type;
		uint8_t sequenceNumber; //0x1 to 0xff
		uint8_t payload[SIG_FOX_MAX_PAYLOAD];
		uint8_t crc;
		uint8_t tailer;
	}sigFoxConfMessage;
}sigFoxMessageU;

typedef struct {
	sigFoxMessageTypeE messageType;
	sigFoxMessageU     message;
}sigFoxT;


bool executeSigFox(sigFoxT *msg);
#endif /* SME_SIGFOX_EXECUTE_H_ */