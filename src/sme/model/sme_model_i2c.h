/*
* sme_cdc_i2c.h
*
* Created: 1/22/2015 8:40:16 PM
*  Author: mfontane
*/


#ifndef SME_MODEL_I2C_H_
#define SME_MODEL_I2C_H_

#include <stdint-gcc.h>

/*
* Possible command for the I2C sensors.
* some of the command need an extra buffer to be completed
*/
typedef enum {
    justForDebugToBeRemoved,
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



void sme_cdc_i2c(i2cQueueS *current_message);
void sme_i2c_mgr_init(void);
sme_i2c_get_read_str (char *msg, uint8_t *len, uint8_t msg_len);
#endif /* SME_CDC_I2C_H_ */