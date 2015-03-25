/*
 * sme_decode_i2c_sfx.h
 *
 * Created: 3/7/2015 8:39:45 AM
 *  Author: smkk
 */ 


#ifndef SME_DECODE_I2C_SFX_H_
#define SME_DECODE_I2C_SFX_H_
#include <stdint-gcc.h>
#include <stdbool.h>


int  sme_i2c_get_press_hum_read_str (char *buffer, char *msg, uint8_t *len, uint8_t msg_len);
int  vl6180x_get_ALS_PROX_str (char *msg, uint8_t msg_len, uint8_t *sensorValue);
int  LSM9DS1_get_A_str (char *msg, uint8_t *len, uint8_t msg_len, void *sensor);
int  LSM9DS1_get_G_str (char *msg, uint8_t *len, uint8_t msg_len, void *sensor);
int  LSM9DS1_get_M_str (char *msg, uint8_t *len, uint8_t msg_len, void *sensor);

#endif /* SME_DECODE_I2C_SFX_H_ */
