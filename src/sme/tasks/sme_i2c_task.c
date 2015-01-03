/*
 * controller.c
 *
 * Created: 02/01/2015 01:47:55
 *  Author: speirano
 */ 

//#include <stdio.h>

#include "sme_cmn.h"
#include "sme_i2c_task.h"

static void i2c_task(void *params);
static void i2cInit(void);



//#include "..\..\TestSerial\src\ASF\sam0\drivers\port\port.h"
//#include "..\..\TestSerial\src\ASF\sam0\boards\samd21_xplained_pro\samd21_xplained_pro.h"
//#include "..\i2c\I2C.h"
//#include "..\i2c\ZXYAxis\ZXYAxis.h"
//#include "..\i2c\nfc\nxpNfc.h"

static xSemaphoreHandle i2c_mutex;
xQueueHandle i2cCommandQueue;
static bool isZXYAxis=false;
static char debug[255];



int sme_i2c_mgr_init(void)
{
	i2cInit();
	
	xTaskCreate(i2c_task,
	(const char *) "I2C",
	configMINIMAL_STACK_SIZE,
	NULL,
	I2C_TASK_PRIORITY,
	NULL);



	// Suspend these since the main task will control their execution
	//vTaskSuspend(terminal_task_handle);
}



static void readAllValues(void){
	
	if (isZXYAxis){
		//uint8_t conf = MMA8452Configure();
		
		if (isZXYAxis==1)
		sprintf(debug, "ZXY initialized = TRUE\r");
		else
		sprintf(debug, "ZXY initialized = FALSE\r");
		
		//if (conf==1)
		sprintf(debug, "ZXY Configured = TRUE");
		//else
		//sprintf(debug, "ZXY Configured = FALSE");
	}
	/*
	if (readManufactoringData()) {
		getNxpSerialNumber(&debug[30]);
		sprintf(debug, "NXP S/N = %s\r", &debug[30]);
		readUserData();
		readSRAM();
		getNxpUserData(debug);
		} else {
		sprintf(debug, "NXP does not work");
	}
	*/
}


static void i2cInit(void) {
	/* Configure the I2C master module */
	//configure_i2c_master();
	//i2cCommandQueue = xQueueCreate(64, sizeof(i2cQueueS));
	i2cCommandQueue = xQueueCreate(64, sizeof(i2cQueueS *));
	i2c_mutex = xSemaphoreCreateMutex();
	//isZXYAxis =ZXYInit();
}



/**
 * \brief i2c mng task
 *
 *
 * \param params Parameters for the task. (Not used.)
 */

static void i2c_task(void *params)
{
	i2cQueueS *current_message;
	
	for (;;) {
		//if (xSemaphoreTake(button_mutex, portMAX_DELAY)) {
			
		xSemaphoreTake(i2c_mutex, portMAX_DELAY);
		if (xQueueReceive(i2cCommandQueue, &current_message, I2C_TASK_DELAY)) {
		//while(xQueueReceive(i2cCommandQueue, &current_message, 0)) {

			//xSemaphoreTake(i2c_mutex, portMAX_DELAY);
			 
			switch (current_message->code){
				case allSensorsReadValue:
				readAllValues();
				break;
				
				case sensorReadValue:
				break;
				
				case sensorReadRegister:
				break;
				
				case sensorWriteRegister:
				break;
				
				default:
				break;
			}
			
			
			//xSemaphoreGive(i2c_mutex);
			//xSemaphoreGive(button_mutex);
		}
		
		xSemaphoreGive(i2c_mutex);
		//vTaskDelay(I2C_TASK_DELAY);
	}
}


