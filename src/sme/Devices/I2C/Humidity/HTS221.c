/*
* HTS221.c
*
* Created: 02/01/2015 20:50:20
*  Author: mfontane
*/
#include "HTS221.h"
#include "..\I2C.h"


// The SparkFun breakout board defaults to 1, set to 0 if SA0 jumper on the bottom of the board is set
#define HTS221_ADDRESS 0xBE


//Define a few of the registers that we will be accessing on the MMA8452

#define WHO_AM_I    0x0F
#define WHO_AM_I_RETURN 0xBC // WHO_AM_I should always be 0x2A

#define AVERAGE_REG 0x10
#define AVERAGE_DEFAULT 0x1B


#define CTRL_REG1   0x20
#define CTRL_REG2   0x21
#define CTRL_REG3   0x22
#define REG_DEFAULT 0x00

#define STATUS_REG  0x27
static inline bool humidityReady(uint8_t data) {
	return (data & 0x02);
}
static inline bool temperatureReady(uint8_t data) {
	return (data & 0x01);
}

#define HUMIDITY_L_REG 0x28
#define HUMIDITY_H_REG 0x29
#define TEMP_L_REG     0x2A
#define TEMP_H_REG     0x2B


/*
* calibration registry should be read for temperature and humidity calculation.
* Before the first calculation of temperature and humidity,
* the master reads out the calibration coefficients.
* will do at init phase
*/
#define CALIB_START        0x30
#define CALIB_END	       0x3F


bool HTS221nit(void) {
	uint8_t data;
	if (readRegister(HTS221_ADDRESS, WHO_AM_I, &data)) {
		if (data == WHO_AM_I_RETURN);
		return HTS221getCalibration();
		} else {
		return false;
	}
}

bool HTS221getCalibration(void) {
	uint8_t data;
	for (int reg=CALIB_START; reg<CALIB_END; reg++) {
		if (readRegister(HTS221_ADDRESS, WHO_AM_I, &data)) {
			switch (reg) {
			case CALIB_START:
				break;
				
				// to cover any possible error
				default:
				return false;
			}
		}
		else {
			return false;
		}
	}
	return true;
}