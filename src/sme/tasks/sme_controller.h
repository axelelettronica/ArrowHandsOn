/*
* controller.h
*
* Created: 02/01/2015 01:48:14
*  Author: speirano
*/

#include "../sme_FreeRTOS.h"

#ifndef SME_CONTROLLER_H_
#define SME_CONTROLLER_H_

typedef enum {
    interruptDetected,
    nfcInt,
    bteInt
}eventE;

typedef struct {
    eventE intE; // which is the interrupt that sends the message  
}controllerQueueS;

//! Queue for incoming execution command
extern xQueueHandle controllerQueue;

BaseType_t sme_ctrl_init(void);
#endif /* SME_CONTROLLER_H_ */