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
#include "../../../model/sme_model_sigfox.h"

extern uint8_t sequenceNumber;

inline uint8_t getNewSequenceNumber(void) {
    return sequenceNumber++;
}

bool executeSigFox(const sigFoxT *msg);
#endif /* SME_SIGFOX_EXECUTE_H_ */