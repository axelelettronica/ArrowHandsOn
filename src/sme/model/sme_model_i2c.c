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
#include "sme_cdc_io.h"

#define MMA8452_POS 4
#define NXPNFC_POS  3
#define TS221_POS   1
#define LPS25_POS   2
#define TCA6416_POS 0

#define MAX_I2C_SENSORS 3

// function pointer for the readValues functions on all sensor
typedef bool (*readValue)(char*);
// function pointer for the initialization of sensor
typedef bool (*initSensor)(void);

#define SME_MAX_I2C_SENSOR_SIZE     10
#define SME_MAX_I2C_SENSOR_BUF_LEN (MAX_I2C_SENSORS*SME_MAX_I2C_SENSOR_SIZE)


typedef struct {
    bool sensorInitialized;
    initSensor sensorInit;
    readValue sensorValue;

} sensorTaskStr;

static sensorTaskStr sensors[MAX_I2C_SENSORS];

xQueueHandle i2cCommandQueue;

char buffer[SME_MAX_I2C_SENSOR_BUF_LEN];


//char buffer[10];
static void readAllValues(void){
    memset(buffer, 0, SME_MAX_I2C_SENSOR_BUF_LEN);
    for(int i=0; i<MAX_I2C_SENSORS; i++) {
        if (sensors[i].sensorInitialized == true) {
            sensors[i].sensorValue(buffer+(i*SME_MAX_I2C_SENSOR_SIZE));
        }
    }
}


static void readGenericRegister(i2cMessageS command) {
        uint8_t data;
    switch (command.fields.sensorId) {

        // it is a 16 bytes page
        case NXPNFC_ADDRESS:;
        if (readBufferRegister(command.fields.sensorId, command.fields.i2cRegister, nfcPageBuffer, sizeof(nfcPageBuffer))) {
            print_dbg("I2C READ Register %s - ", nfcPageBuffer);
        }else {
            print_dbg("I2C READ Fails");
        }
        return;

        default:
        if (readRegister(command.fields.sensorId, command.fields.i2cRegister, &data)){
            print_dbg("I2C READ Register %c - ", data);
        }else {
            print_dbg("I2C READ Fails");
        }

        return;
    }
}

static void readSensorValue(i2cMessageS command){
    uint8_t i2CId=0;
    switch (command.fields.sensorId) {
        case NXPNFC_ADDRESS:
        i2CId = NXPNFC_POS;
        break;
        
        case MMA8452_ADDRESS:
        i2CId = MMA8452_POS;
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
        readGenericRegister(current_message->command);
        break;
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
    
    /*sensors[MMA8452_POS].sensorInit  = ZXYInit;
    sensors[MMA8452_POS].sensorValue = MMA8452getAccelData;
    
    sensors[NXPNFC_POS].sensorValue = / *getNxpUserData* /(readValue)readSRAM;
    sensors[NXPNFC_POS].sensorInit  = nxpInit;*/
        
        sensors[LPS25_POS].sensorInit  = LPS25Hnit;
        sensors[LPS25_POS].sensorValue = LPS25HgetValues;
    
    sensors[TS221_POS].sensorInit  = HTS221nit;
    sensors[TS221_POS].sensorValue = HTS221getValues;

    
    sensors[TCA6416_POS].sensorInit  = TCA6416aInit;
    sensors[TCA6416_POS].sensorValue = TCA6416aPort1Values;
    
    for(int i=0; i<MAX_I2C_SENSORS; i++) {
        if (sensors[i].sensorInit())
        sensors[i].sensorInitialized=1;
    }
    
    
    //if the IO extender as been initialized, reset the Devices
    if (sensors[TCA6416_POS].sensorInitialized)
    TCA6416aResetDevices();
}


/*
 *  This function exports a preformatted string with all read sensors
 */
char *sme_i2c_get_read_str(void) {
    readAllValues();
    return buffer;
}