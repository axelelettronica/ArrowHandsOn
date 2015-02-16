/*
* sme_sigfox_execute.h
*
* Created: 12/01/2015 20:43:40
*  Author: mfontane
*
* This the file that execute  the command on sigfox component according with
* the information are present on the sigFoxT structure
*/


#ifndef SME_SL868V2_EXECUTE_H_
#define SME_SL868V2_EXECUTE_H_
//#include <stdint-gcc.h>
#include <stdbool.h>
#include "../../../model/sme_model_sl868v2.h"

//extern uint8_t sequenceNumber;

//inline uint8_t getNewSequenceNumber(void);

//inline uint8_t getNewSequenceNumber(void) {
//    return sequenceNumber++;
//}

int  executeSl868v2(const sl868v2T *data);
int  sendSl868v2Msg(const uint8_t *msg, uint8_t len);

//  GPS exported API to drive GPS management
void gpsStartScan(void);
#endif /* SME_SL868V2_EXECUTE_H_ */