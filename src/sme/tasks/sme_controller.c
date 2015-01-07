/*
 * controller.c
 *
 * Created: 02/01/2015 01:47:55
 *  Author: speirano
 */ 

#include "sme_cmn.h"

#include "../sme_FreeRTOS.h"

static void control_task(void *params);

int sme_ctrl_init(void)
{
	xTaskCreate(control_task,
	(const char *) "Control",
	configMINIMAL_STACK_SIZE,
	NULL,
	CONTROL_TASK_PRIORITY,
	NULL);



	// Suspend these since the main task will control their execution
	//vTaskSuspend(terminal_task_handle);
	
	return 0;
}


/**
 * \brief control task
 *
 * This task keeps track of which screen the user has selected, which tasks
 * to resume/suspend to draw the selected screen, and also draws the menu bar.
 *
 * This task is the board controller.
 * This manage the SmartEverything controller as required
 *
 * \param params Parameters for the task. (Not used.)
 */
static void control_task(void *params)
{
	//xTaskHandle temp_task_handle = NULL;

	for(;;) {
			// We can now safely suspend the previously resumed task
			//if (temp_task_handle) {
			//	vTaskSuspend(temp_task_handle);
			//	temp_task_handle = NULL;
			//}


			//xSemaphoreGive(display_mutex);

			// If a task handle was specified, resume it now
			//if (temp_task_handle) {
			//	vTaskResume(temp_task_handle);
			//}
        dbg("control_task Loop\r\n");
		vTaskDelay(CONTROL_TASK_DELAY);
	}
}

