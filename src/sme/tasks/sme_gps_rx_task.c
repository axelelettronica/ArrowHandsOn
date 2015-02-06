/*
* sme_gps_controller.c
*
* Created: 22/01/2015 17:21:08
*  Author: speirano
*/

#include "sme_gps_task.h"
#include "sme_cmn.h"
#include "..\Devices\Uart\gps\sme_sl868v2_usart.h"
#include "..\Devices\Uart\gps\sme_sl868v2_rx_fsm.h"

#define GPS_TASK_DELAY       (1000 / portTICK_RATE_MS)
#define GPS_STACK_SIZE	  	 (configMINIMAL_STACK_SIZE*2)
#define GPS_TASK_PRIORITY    (tskIDLE_PRIORITY + 1)
#define GPS_SEMAPHORE_DELAY  (1000 / portTICK_RATE_MS)


/* task variables */

xSemaphoreHandle gps_rx_sem;
/* task variables */


volatile uint8_t test;

/**
* \brief Terminal task
*
* This task prints the terminal text buffer to the display.
*
* \param params Parameters for the task. (Not used.)
*/
static void gpsRxTask(void *params)
{
	uint8_t msg[SME_GPS_MAX_DATA_LEN];

    // Trigger the RX interrupt
    sl868v2ReceivedMessage(msg, SME_GPS_MAX_DATA_LEN);

	for (;;) {
 		BaseType_t preso = xSemaphoreTake(gps_rx_sem, GPS_SEMAPHORE_DELAY);
		if (preso) {
            if (sl868v2ReceivedMessage(msg, SME_GPS_MAX_DATA_LEN) == STATUS_OK){
            //		test++;
                sl868v2HandleRx(msg, SME_GPS_MAX_DATA_LEN);
                print_dbg("msg: %c", msg[0]);
        	} 
			//test++;
		}
	}
}


BaseType_t sme_gps_mgr_init(void)
{
	// create the sigFox semaphore
	gps_rx_sem = xSemaphoreCreateBinary();
	sl868InitRxData();	
	// create the sigFox Task
	return xTaskCreate(gpsRxTask,
	GPS_TASK_NAME,
	GPS_STACK_SIZE,
	NULL,
	GPS_TASK_PRIORITY,
	NULL);

}
