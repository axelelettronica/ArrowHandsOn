/*
* sme_cdc_i2c.c
*
* Created: 1/22/2015 8:40:06 PM
*  Author: mfontane
*/


#include <stdbool.h>
#include "../sme_FreeRTOS.h"
#include "sme_model_i2c.h"
#include "../Devices/I2C/ZXYAxis/ZXYAxis.h"
#include "../Devices/I2C/nfc/nxpNfc.h"
#include "../Devices/I2C/Humidity/HTS221.h"
#include "../Devices/I2C/Pressure/LPS25H.h"
#include "../Devices/I2C/IOExpander/tca6416a.h"
#include "../Devices/I2C/I2C.h"

#define MAX_I2C_SENSORS 5

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



void sme_cdc_i2c(i2cQueueS *current_message) {
    switch (current_message->code) {
        

        case justForDebugToBeRemoved:
        debugRemove(current_message);
        break;
        
        case allSensorsReadValue:
        readAllValues();
        break;
        
        case sensorReadRegister:
        case sensorReadValue:
        readSensorValue(current_message->command);
        break;
        
        case sensorWriteRegister:
        break;
    }
}

void sme_i2c_mgr_init(void) {
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
    
    sensors[4].sensorInit  = TCA6416aInit;
    sensors[4].sensorValue = TCA6416aPort1Values;
    
    for(int i=0; i<MAX_I2C_SENSORS; i++) {
        if (sensors[i].sensorInit())
        sensors[i].sensorInitialized=1;
    }
    
}