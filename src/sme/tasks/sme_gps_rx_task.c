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
#include "common/sme_timer_define.h"

#define GPS_TASK_DELAY       ONE_SECOND
#define GPS_STACK_SIZE	  	 (configMINIMAL_STACK_SIZE)
#define GPS_TASK_PRIORITY    (tskIDLE_PRIORITY + 2)
#define GPS_SEMAPHORE_DELAY  ONE_SECOND
#define GPS_RX_TASK_DELAY    (1 / portTICK_RATE_MS) // 1 msec


#define SME_GPS_MAX_DATA_LEN MAX_SL868V2_RX_BUFFER_LENGTH
/* task variables */


/* task variables */

//! Queue for incoming terminal characters
static xQueueHandle gps_msg_in_queue;
//! Semaphore to signal busy terminal buffer
//static xSemaphoreHandle gps_msg_mutex;

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
    
	for (;;) {		
        while (sl868v2ReceivedMessage(msg, SME_GPS_MAX_DATA_LEN) == SME_OK){
             xQueueSendFromISR(gps_msg_in_queue, (char *)&msg[0], NULL);
        } 

        vTaskDelay(1);
	} 

}

static void gpsProcRx_task(void *params)
{
    uint8_t current_char_ptr = 0;

    for (;;) {

        // Grab terminal mutex
        //xSemaphoreTake(gps_msg_mutex, portMAX_DELAY);

        while (pdTRUE == xQueueReceive(gps_msg_in_queue, &current_char_ptr, 0)) {
             sl868v2HandleRx(&current_char_ptr, SME_GPS_MAX_DATA_LEN);
            // Set zero-terminator at head
			current_char_ptr = '\0';
        }

       //xSemaphoreGive(gps_msg_mutex);

       vTaskDelay(GPS_RX_TASK_DELAY);
	}
}

BaseType_t sme_gps_mgr_init(void)
{
    gps_msg_in_queue = xQueueCreate(1024, sizeof(uint8_t));
    //gps_msg_mutex = xSemaphoreCreateMutex();

	sl868InitRxData();	

	// create the sigFox Task
	xTaskCreate(gpsRxTask, GPS_TASK_NAME, GPS_STACK_SIZE,
	            NULL, GPS_TASK_PRIORITY, NULL);

    xTaskCreate(gpsProcRx_task, (const char *) "GPS_RX", GPS_STACK_SIZE,
                NULL, GPS_TASK_PRIORITY, NULL);
}
