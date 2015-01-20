/*
* controller.c
*
* Created: 02/01/2015 01:47:55
*  Author: speirano
*/

#include <sme_cmn.h>
#include "sme_i2c_task.h"
#include "..\Devices\I2C\nfc\nxpNfc.h"
#include "..\Devices\I2C\ZXYAxis\ZXYAxis.h"
#include "..\Devices\I2C\I2C.h"
#include "..\Devices\I2C\Humidity\HTS221.h"
#include "..\Devices\I2C\Pressure\LPS25H.h"

#define I2C_TASK_DELAY     (1000 / portTICK_RATE_MS)

#define MAX_I2C_SENSORS 4

// function pointer for the readValues functions on all sensor
typedef bool (*readValue)(char*);
// function pointer for the initialization of sensor
typedef bool (*initSensor)(void);


typedef struct {
	bool sensorInitialized;
	initSensor sensorInit;
	readValue sensorValue;
} sensorTaskStr;

static sensorTaskStr sensors[MAX_I2C_SENSORS];

xQueueHandle i2cCommandQueue;


char buffer[10];
static void readAllValues(void){
	
	for(int i=0; i<MAX_I2C_SENSORS; i++) {
		if (sensors[i].sensorInitialized == true)
		sensors[i].sensorValue(buffer);
	}
}

volatile static	uint8_t i2CId=0;
static void readSensorValue(messageU command){

	switch (command.fields.sensorId) {
		case NXPNFC_ADDRESS:
		i2CId = 1;
		break;
		
		case MMA8452_ADDRESS:
		i2CId = 0;
		break;
		
		default:
		return;
	}
	
	sensors[i2CId].sensorValue(buffer);
}

static void i2cInit(void) {
	/* Configure the I2C master module */
	configure_i2c_master();
	
	sensors[0].sensorInit  = ZXYInit;
	sensors[0].sensorValue = MMA8452getAccelData;
	
	sensors[1].sensorValue = /*getNxpUserData*/readSRAM;
	sensors[1].sensorInit  = nxpInit;
	
	sensors[2].sensorInit  = HTS221nit;
	sensors[2].sensorValue = HTS221getValues;
	
	sensors[3].sensorInit  = LPS25Hnit;
	sensors[3].sensorValue = LPS25HgetValues;
	
	for(int i=0; i<MAX_I2C_SENSORS; i++) {
		if (sensors[i].sensorInit())
		sensors[i].sensorInitialized=1;
	}
	
}

static uint8_t debugState;
static void debugRemove(i2cQueueS *current_message){
	switch(debugState) {
		case 1:
		current_message->command.fields.sensorId= MMA8452_ADDRESS;
		readSensorValue(current_message->command);
		debugState++;
		break;
		
		case 2:
		current_message->command.fields.sensorId= NXPNFC_ADDRESS;
		readSensorValue(current_message->command);
		debugState = 0;
		break;
		
		case 0:
		readAllValues();
		debugState++;
		break;
	}
}

/**
* \brief Terminal task
*
* This task prints the terminal text buffer to the display.
*
* \param params Parameters for the task. (Not used.)
*/
volatile char debugController;
static void i2cTask(void *params)
{
	i2cQueueS current_message;
	debugController++;
	for (;;) {

		if (xQueueReceive(i2cCommandQueue, &current_message, I2C_TASK_DELAY)) {
			//print_dbg("received message\r\n");
			switch (current_message.code){
				
				case justForDebugToBeRemoved:
				debugRemove(&current_message);
				break;
				
				case allSensorsReadValue:
				readAllValues();
				break;
				
				case sensorReadRegister:
				case sensorReadValue:
				readSensorValue(current_message.command);
				break;
				
				case sensorWriteRegister:
				break;
			}

		}

	}
}

BaseType_t sme_i2c_mgr_init(void)
{
	i2cInit();
	
	BaseType_t err;
	// create the I2c Queue
	i2cCommandQueue = xQueueCreate(64, sizeof(i2cQueueS));
	
	// create the I2C Task
	err= xTaskCreate(i2cTask,
	I2C_TASK_NAME,
	I2C_STACK_SIZE,
	NULL,
	I2C_TASK_PRIORITY,
	NULL);
	
	return err;
}
