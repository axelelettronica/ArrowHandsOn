/*
* controller.c
*
* Created: 02/01/2015 01:47:55
*  Author: speirano
*/

#include "sme_cmn.h"
#include "sme_controller.h"
#include "..\Devices\I2C\Humidity\HTS221.h"
#include "..\Devices\I2C\Pressure\LPS25H.h"
#include "uart\sme_sigfox_execute.h"

static void control_task(void *params);
xQueueHandle controllerQueue;
BaseType_t sme_ctrl_init(void)
{

	BaseType_t err;
	controllerQueue = xQueueCreate(64, sizeof(controllerQueueS));
	if( controllerQueue != 0 )
	{
		err= xTaskCreate(control_task,
		(const char *) "Control",
		configMINIMAL_STACK_SIZE,
		NULL,
		CONTROL_TASK_PRIORITY,
		NULL);
	}
	
	return err;
}


/*
* Send the Sensor value to SigFox.
* If it is required add the GPS Location
*/
static void sendValueToSigFox(uint8_t value, char withGPS){
	
	sigFoxT sigFoxMsg;
	executeSigFox(&sigFoxMsg);
}


static bool prepareNfcSensorValues(const controllerQueueS *current_message, uint8_t *value) {

	bool valueRead=false;
	switch (current_message->intSensor.nfc.sensor) {
		case humidityValue:
		valueRead = HTS221getValues(value);
		break;
		
		case pressureValue:
		valueRead = LPS25HgetValues(value);
		break;
	}
	return valueRead;
}



/**
*
* This task is the board controller.
* This manage the SmartEverything controller as required
*
* \param params Parameters for the task. (Not used.)
*/

static void control_task(void *params)
{
	bool valueRead=false;
	controllerQueueS current_message;
	uint8_t value;
	

	for(;;) {
		if (xQueueReceive(controllerQueue, &current_message, CONTROL_TASK_DELAY)) {

			interruptE intType = current_message.intE;
			switch (intType) {
				case nfcInt:
				prepareNfcSensorValues(&current_message, &value);
				break;
				
				default:
				break;
			}
		}
		
		if (valueRead) {
			sendValueToSigFox(value, current_message.withGPS);
		}
	}
}

