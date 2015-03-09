/*
* sme_cdc_i2c.h
*
* Created: 1/22/2015 8:40:16 PM
*  Author: mfontane
*/


#ifndef SME_MODEL_I2C_H_
#define SME_MODEL_I2C_H_

#include <stdint-gcc.h>


// function pointer for the readValues functions on all sensor
typedef bool (*readValue)(uint16_t*);
// function pointer for the initialization of sensor
typedef bool (*initSensor)(void);
// function pointer decoding  Data on all sensor
typedef bool (*sensorDecode)(uint16_t* buf, uint16_t *data1, ...);

typedef struct {
    bool          sensorInitialized;
    initSensor    sensorInit;
    readValue     sensorValue;
    sensorDecode  decodeCb;
    uint16_t      decodedData1;
    uint16_t      decodedData2;
    uint16_t      decodedData3;
} sensorTaskStr;


/*
* Possible command for the I2C sensors.
* some of the command need an extra buffer to be completed
*/
typedef enum {
    allSensorsReadValue,
    sensorReadValue,
    sensorReadRegister,
    sensorWriteRegister
} sensorCommandE;

#define SME_CDC_MAX_DATA_LEN  20

typedef struct {
    struct {
        uint8_t sensorId;
        uint8_t i2cRegister;
        uint8_t datalen;
        uint8_t data[SME_CDC_MAX_DATA_LEN];
    }fields;
}i2cMessageS;

typedef struct {
    sensorCommandE code; // code of command to execute to the sensor
    i2cMessageS   command;     // must be allocated and released by the caller
}i2cQueueS;


typedef enum {
    SME_I2C_PRESS_HUM_TEMP_STR,
    SME_I2C_XL_STR,
    SME_I2C_GYRO_STR,
    SME_I2C_MAGNET_STR,
    SME_I2C_PROX_STR,
    SME_I2C_All_SENSOR
} sme_i2c_msg_str_E;


void sme_cdc_i2c(i2cQueueS *current_message);
void sme_i2c_mgr_init(void);
sme_i2c_get_read_str (sme_i2c_msg_str_E type, char *msg, uint8_t *len, uint8_t msg_len);
#endif /* SME_CDC_I2C_H_ */