/*
 * sme_cdc_i2c.c
 *
 * Created: 1/22/2015 8:40:06 PM
 *  Author: smkk
 */


#include <stdbool.h>
#include "../sme_FreeRTOS.h"
#include "sme_model_i2c.h"
#include "../Devices/I2C/nfc/NT3H.h"
#include "../Devices/I2C/Humidity/HTS221.h"
#include "../Devices/I2C/Pressure/LPS25H.h"
#include "../Devices/I2C/Accelerometer/LSM9DS1.h"
#include "../Devices/I2C/IOExpander/tca6416a.h"
#include "../Devices/I2CFreeRtos.h"
#include "sme_cdc_io.h"
#include "sme_cmn.h"
#include "Devices/I2C/Accelerometer/LSM9DS1.h"
#include "Devices/I2C/proximity_ambient/vl6180x.h"
#include "behaviour/sme_decode_i2c_sfx.h"
#include "Devices/I2C/crypto/sha204.h"


/*
 *
 * Define all the I2C components
 *
 */

/*
 *
 * First list the generic use components.
 *
 */
#define TCA6416_POS 		  0
#define NXPNFC_POS            1
#define CRYPTO				  2
#define FIRST_I2C_SENSOR      CRYPTO+1

/*
 *
 * At the end lists all the I2C sensors.
 *
 */
#define LPS25_POS             FIRST_I2C_SENSOR
#define TS221_POS             (LPS25_POS+1)
#define VL6180X_POS           (TS221_POS+1)
#define LSM9DS1_A_POS         (VL6180X_POS+1)      // LSM9DS1- Accelerometer - Main init
#define LSM9DS1_G_POS         (LSM9DS1_A_POS+1)    // LSM9DS1 - Gyroscope
#define LSM9DS1_M_POS         (LSM9DS1_G_POS+1)    // LSM9DS1 - Magnetometer


#define MAX_I2C_COMPONENT     (LSM9DS1_M_POS+1)  // Postponed other sensors

/*
 *
 * END Define all the I2C components
 *
 */
#define SME_MAX_I2C_READ_BUF_LEN     10
static sensorTaskStr sensors[MAX_I2C_COMPONENT];

xQueueHandle i2cCommandQueue;

char buffer[SME_MAX_I2C_READ_BUF_LEN];

static void readSensor(uint8_t idx)
{
    bool read = false;

    if ((idx >= FIRST_I2C_SENSOR) && (idx < MAX_I2C_COMPONENT)) {
        if (sensors[idx].sensorInitialized == true) {
            if (sensors[idx].sensorValue) {
                memset(buffer, 0, SME_MAX_I2C_READ_BUF_LEN);
                read = sensors[idx].sensorValue(buffer);

                if (read && sensors[idx].decodeCb) {
                    sensors[idx].decodeCb(buffer,
                            &sensors[idx].decodedData1,
                            &sensors[idx].decodedData2,
                            &sensors[idx].decodedData3);
                }
            }
        }
    } else {
        print_err("Unforeseen sensor %d\n", idx);
    }
}


static void readAllValues(void)
{
    for(int i=FIRST_I2C_SENSOR; i< MAX_I2C_COMPONENT; ++i) {
        readSensor(i);
    }
}


static void readGenericRegister(i2cMessageS command) {
    uint8_t data;
    switch (command.fields.sensorId) {
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

    default:
        return;
    }

    sensors[i2CId].sensorValue(buffer);
}


void sme_cdc_i2c(i2cQueueS *current_message) {
    switch (current_message->code) {

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


volatile int data;
static void wakeUpCrypto(void){
    struct port_config pin_conf;
    port_get_config_defaults(&pin_conf);


    /* Configure OUTPUT Pin */
    //first yellow led
    pin_conf.direction  = PORT_PIN_DIR_OUTPUT;
    port_pin_set_config(SME_SDA_PIN, &pin_conf);
    port_pin_set_config(SME_SCL_PIN, &pin_conf);
    
    for (int i=0; i<10000; i++) {
        data++;
    }
    port_pin_set_output_level(SME_SDA_PIN, true);
    for (int i=0; i<10000; i++) {
        data++;
    }
    port_pin_set_output_level(SME_SDA_PIN, false);
    

};


void sme_i2c_mgr_init(void) {
    
    //wakeUpCrypto();
    data++;
    /* Configure the I2C master module */
    configure_i2c_master(SME_I2C_SERCOM_PINMUX_PAD0, SME_I2C_SERCOM_PINMUX_PAD1, SME_I2C_MODULE);

    memset(sensors, 0, sizeof(sensorTaskStr));

    sensors[CRYPTO].sensorValue = NULL;
    sensors[CRYPTO].sensorInit  = sha204_init;

    sensors[NXPNFC_POS].sensorValue = /*getNxpUserData*/(readValue)NT3HReadSram;
    sensors[NXPNFC_POS].sensorInit  = NT3HInit;

    sensors[LPS25_POS].sensorInit  = LPS25Hnit;
    sensors[LPS25_POS].sensorValue = LPS25HgetValues;
    sensors[LPS25_POS].decodeCb    = LPS25HDecode;

    sensors[TS221_POS].sensorInit  = HTS221nit;
    sensors[TS221_POS].sensorValue = HTS221getValues;
    sensors[TS221_POS].decodeCb    = HTS221Decode;

    sensors[TCA6416_POS].sensorInit  = TCA6416a_init;
    sensors[TCA6416_POS].sensorValue = TCA6416a_input_port1_values;

    sensors[VL6180X_POS].sensorInit  = vl6180x_init;
    sensors[VL6180X_POS].sensorValue = vl6180x_get_light_range_polling;
    sensors[VL6180X_POS].decodeCb    = vl6180x_decode;

    sensors[LSM9DS1_A_POS].sensorInit  = LSM9DS1_A_Init;    
    sensors[LSM9DS1_A_POS].sensorValue = LSM9DS1_A_getValues;
    sensors[LSM9DS1_A_POS].decodeCb    = LSM9DS1_A_Decode;

    sensors[LSM9DS1_M_POS].sensorInit  = LSM9DS1_M_Init;
    sensors[LSM9DS1_M_POS].sensorValue = LSM9DS1_M_getValues;
    sensors[LSM9DS1_M_POS].decodeCb    = LSM9DS1_M_Decode;

    sensors[LSM9DS1_G_POS].sensorInit  = LSM9DS1_G_Init;
    sensors[LSM9DS1_G_POS].sensorValue = LSM9DS1_G_getValues;
    sensors[LSM9DS1_G_POS].decodeCb    = LSM9DS1_G_Decode;

    for(int i=0; i<MAX_I2C_COMPONENT; i++) {
        if (sensors[i].sensorInit && sensors[i].sensorInit()) {
            sensors[i].sensorInitialized=1;
        }
    }

    //if the IO extender as been initialized, reset the Devices
    if (sensors[TCA6416_POS].sensorInitialized)
        TCA6416a_reset_devices();
}


/*
 *  This function exports a pre formatted string with all read sensors
 */
int  sme_i2c_get_read_str (sme_i2c_msg_str_E type, char *msg, uint8_t *len, uint8_t msg_len)
{
    uint8_t offset = 0;

    if (!msg || !len) {
        return SME_ERR;
    }

    switch (type) {
    case SME_I2C_PRESS_HUM_TEMP_STR:    
        /*
        readSensor(LPS25_POS);
        readSensor(TS221_POS);*/
        sme_i2c_get_press_hum_read_str(buffer, msg, len, msg_len);
        break;

    case SME_I2C_XL_STR:
        readSensor(LSM9DS1_A_POS);
        LSM9DS1_get_A_str(msg, len, msg_len, &sensors[LSM9DS1_A_POS]);
        break;

    case SME_I2C_GYRO_STR:
        readSensor(LSM9DS1_G_POS);
        LSM9DS1_get_G_str(msg, len, msg_len, &sensors[LSM9DS1_G_POS]);
        break;

    case SME_I2C_MAGNET_STR:
        readSensor(LSM9DS1_M_POS);
        LSM9DS1_get_M_str(msg, len, msg_len, &sensors[LSM9DS1_M_POS]);
        break;

    case SME_I2C_PROX_STR:        
        readSensor(VL6180X_POS);
        *len = vl6180x_get_ALS_PROX_str(msg, msg_len, (uint8_t *)&sensors[VL6180X_POS].decodedData1);
        break;
    default:

    case SME_I2C_All_SENSOR:
        readAllValues();
        break;

        print_err("I2C GET error %d", type);
        break;
    }

    return SME_OK;
}
