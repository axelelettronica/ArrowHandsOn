/*
 * sme_usart_tx_task.h
 *
 * Created: 10/01/2015 18:46:57
 *  Author: mfontane
 *
 * This is the task that received the message from the console.
 * The queue of the task contains the enum that identify the component will be drive
 * and a specific structure that is used by the component to execute the action.
 * 
 */ 


#ifndef SME_USART_TX_TASK_H_
#define SME_USART_TX_TASK_H_


#include "../../sme_FreeRTOS.h"

#define SME_TASK_MAX_DATA_LEN  20

typedef enum {
	sigFox,
	gps,
	blte
} taskCommandE;

typedef struct {
	taskCommandE code;		// code that identify the component that will be used
	void *componentStruct;  // the specific structure of the pointed component
}usartQueueS;


//! Queue for incoming execution command
extern xQueueHandle usartCommandQueue;

BaseType_t sme_usart_init(void);


#endif /* SME_USART_TX_TASK_H_ */