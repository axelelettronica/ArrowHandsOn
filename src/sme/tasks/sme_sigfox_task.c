/*
* sme_sigfox_controller.c
*
* Created: 10/01/2015 17:21:08
*  Author: mfontane
*/

#include "sme_sigfox_task.h"
#include "..\Devices\Uart\sigFox\sme_sigfox_usart.h"


#define SIGFOX_TASK_DELAY       (1000 / portTICK_RATE_MS)
#define SIGFOX_STACK_SIZE		(configMINIMAL_STACK_SIZE*2)
#define SIGFOX_TASK_PRIORITY    (tskIDLE_PRIORITY + 1)
#define SIGFOX_SEMAPHORE_DELAY  (1000 / portTICK_RATE_MS)


/* task variables */

xSemaphoreHandle sigfox_sem;
/* task variables */


volatile uint8_t test;

/**
* \brief Terminal task
*
* This task prints the terminal text buffer to the display.
*
* \param params Parameters for the task. (Not used.)
*/
static void sigFoxTask(void *params)
{
	uint8_t msg[MAX_RX_BUFFER_LENGTH];
	for (;;) {
		if (sigfoxReceivedMessage(msg, MAX_RX_BUFFER_LENGTH) == STATUS_OK){
			test++;
		}
		BaseType_t preso = xSemaphoreTake(sigfox_sem, SIGFOX_SEMAPHORE_DELAY);
		if (preso) {

			test++;

		}
	}
}


BaseType_t sme_sigfox_mgr_init(void)
{

	
	// create the sigFox semaphore
	sigfox_sem = xSemaphoreCreateBinary();
	
	// create the sigFox Task
	return xTaskCreate(sigFoxTask,
	SIGFOX_TASK_NAME,
	SIGFOX_STACK_SIZE,
	NULL,
	SIGFOX_TASK_PRIORITY,
	NULL);

}
