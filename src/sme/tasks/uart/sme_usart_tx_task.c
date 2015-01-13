/*
* sme_usart_tx_task.c
*
* Created: 10/01/2015 18:46:40
*  Author: mfontane
*/
#include "sme_usart_tx_task.h"
#include "samd21_xplained_pro.h"
#include "port.h"
#include "..\..\Devices\uart\sigFox\sme_sigfox_usart.h"
#include "sme_sigfox_execute.h"

#define USART_TASK_DELAY     (1000 / portTICK_RATE_MS)

#define USART_TASK_NAME (const char *)"USART"
#define USART_STACK_SIZE (configMINIMAL_STACK_SIZE*2)
#define USART_TASK_PRIORITY  (tskIDLE_PRIORITY + 1)

xQueueHandle usartCommandQueue;

static void usartTxTask(void *params){
	usartQueueS current_message;
	for (;;) {

		if (xQueueReceive(usartCommandQueue, &current_message, USART_TASK_DELAY)) {
			switch (current_message.code){
				
				case sigFox:
				executeSigFox(current_message.componentStruct);				
				break;
				
				default:
				//error print Help
				break;
			}
		}
	}
}

BaseType_t sme_usart_init(void){
	
	sigFoxInit();
	
	// create the USART Queue
	usartCommandQueue = xQueueCreate(64, sizeof(usartQueueS));
	
	// create the USART Task
	return xTaskCreate(usartTxTask,
	USART_TASK_NAME,
	USART_STACK_SIZE,
	NULL,
	USART_TASK_PRIORITY,
	NULL);
}