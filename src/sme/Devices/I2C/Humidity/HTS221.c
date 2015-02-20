/*
* HTS221.c
*
* Created: 02/01/2015 20:50:20
*  Author: mfontane
*/
#include "HTS221.h"
#include "..\I2C.h"

#if NOT_SENSOR
// The SparkFun breakout board defaults to 1, set to 0 if SA0 jumper on the bottom of the board is set
#define HTS221_ADDRESS 0x1D


//Define a few of the registers that we will be accessing on the MMA8452

#define WHO_AM_I    0x0D
#define WHO_AM_I_RETURN 0x2A

#define AVERAGE_REG 0x10
#define AVERAGE_DEFAULT 0x1B


/*
* [7] PD: power down control
* (0: power-down mode; 1: active mode)
*
* [6:3] Reserved
*
* [2] BDU: block data update
* (0: continuous update; 1: output registers not updated until MSB and LSB reading)
The BDU bit is used to inhibit the output register update between the reading of the upper
and lower register parts. In default mode (BDU = ‘0’), the lower and upper register parts are
updated continuously. If it is not certain whether the read will be faster than output data rate,
it is recommended to set the BDU bit to ‘1’. In this way, after the reading of the lower (upper)
register part, the content of that output register is not updated until the upper (lower) part is
read also.
*
* [1:0] ODR1, ODR0: output data rate selection (see table 17)
*/
#define CTRL_REG1   0x20
#define POWER_UP    0x80
#define BDU_SET     0x4


#define CTRL_REG2   0x21
#define CTRL_REG3   0x22
#define REG_DEFAULT 0x00


/*
* Status register; the content of this register is updated every one-shot reading, and
* after completion of every ODR cycle, regardless of BDU value in CTRL_REG1.
*
* [7:2] Reserved
*
* [1] H_DA: Humidity data available.
* (0: new data for Humidity is not yet available; 1: new data for Humidity is available)
* H_DA is set to 1 whenever a new humidity sample is available. H_DA is cleared anytime
* HUMIDITY_OUT_H (29h) register is read.
*
* [0] T_DA: Temperature data available.
* (0: new data for temperature is not yet available; 1: new data for temperature is available)
* T_DA is set to 1 whenever a new temperature sample is available. T_DA is cleared anytime
* TEMP_OUT_H (2Bh) register is read.
*/
#define STATUS_REG  0x0D
#define TEMPERATURE_READY 0xA
#define HUMIDITY_READY    0x20



#define HUMIDITY_L_REG 0x13
#define HUMIDITY_H_REG 0x14
#define TEMP_L_REG     0x13
#define TEMP_H_REG     0x14
/*
* calibration registry should be read for temperature and humidity calculation.
* Before the first calculation of temperature and humidity,
* the master reads out the calibration coefficients.
* will do at init phase
*/
#define CALIB_START        0x10
#define CALIB_END	       0x1F
#else

#define HTS221_ADDRESS     0x5F


//Define a few of the registers that we will be accessing on the HTS221
#define WHO_AM_I    0x0F
#define WHO_AM_I_RETURN 0xBC //This read-only register contains the device identifier, set to BCh

#define AVERAGE_REG 0x10	// To configure humidity/temperature average.
#define AVERAGE_DEFAULT 0x1B

/*
* [7] PD: power down control
* (0: power-down mode; 1: active mode)
*
* [6:3] Reserved
*
* [2] BDU: block data update
* (0: continuous update; 1: output registers not updated until MSB and LSB reading)
The BDU bit is used to inhibit the output register update between the reading of the upper
and lower register parts. In default mode (BDU = ‘0’), the lower and upper register parts are
updated continuously. If it is not certain whether the read will be faster than output data rate,
it is recommended to set the BDU bit to ‘1’. In this way, after the reading of the lower (upper)
register part, the content of that output register is not updated until the upper (lower) part is
read also.
*
* [1:0] ODR1, ODR0: output data rate selection (see table 17)
*/
#define CTRL_REG1   0x20
#define POWER_UP    0x80
#define BDU_SET     0x4


#define CTRL_REG2   0x21
#define CTRL_REG3   0x22
#define REG_DEFAULT 0x00

#define STATUS_REG  0x27
#define TEMPERATURE_READY 0x1
#define HUMIDITY_READY    0x2

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
#endif
static inline bool humidityReady(uint8_t data) {
	return (data & 0x02);
}
static inline bool temperatureReady(uint8_t data) {
	return (data & 0x01);
}

static uint8_t h0_rH, h1_rH;
static uint16_t T0_degC, T1_degC, H0_T0, H1_T1, T0, T1;
volatile uint8_t data;
bool HTS221nit(void) {
	if (readRegister(HTS221_ADDRESS, WHO_AM_I, &data)) {
		if (data == WHO_AM_I_RETURN){
			if (HTS221Activate()){
				return HTS221getCalibration();
			}			
		}
	}
	
	return false;
}

bool HTS221getCalibration(void) {
	uint8_t data;
	uint16_t tmp;
	
	for (int reg=CALIB_START; reg<=CALIB_END; reg++) {
		if ((reg!=CALIB_START+8) && (reg!=CALIB_START+9) && (reg!=CALIB_START+4)) {
			if (readRegister(HTS221_ADDRESS, reg, &data)) {
				switch (reg) {
					case CALIB_START:
					h0_rH = data;
					break;
					case CALIB_START+1:
					h1_rH = data;
					break;
					case CALIB_START+2:
					T0_degC = data;
					break;
					case CALIB_START+3:
					T1_degC = data;
					break;
					
					case CALIB_START+5:
					tmp = T0_degC;
					T0_degC = (data&0x3)<<8;
					T0_degC |= tmp;
					
					tmp = T1_degC;
					T1_degC = ((data&0xC)>>2)<<8;
					T1_degC |= tmp;
					break;
					case CALIB_START+6:
					H0_T0 = data;
					break;
					case CALIB_START+7:
					H0_T0 = data<<8;
					break;
					case CALIB_START+0xA:
					H1_T1 = data;
					break;
					case CALIB_START+0xB:
					H1_T1 = data <<8;
					break;
					case CALIB_START+0xC:
					T0 = data;
					break;
					case CALIB_START+0xD:
					T0 = data << 8;
					break;
					case CALIB_START+0xE:
					T1 = data;
					break;
					case CALIB_START+0xF:
					T1 = data << 8;
					break;
					
					
					case CALIB_START+8:
					case CALIB_START+9:
					case CALIB_START+4:
					//DO NOTHING
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
	}
	return true;
}

bool HTS221BDUActivate(void) {
	uint8_t data;
	if (readRegister(HTS221_ADDRESS, CTRL_REG1, &data)) {
		data |= BDU_SET;
		if (writeRegister(HTS221_ADDRESS, CTRL_REG1, data))
		return true;
	}
	return false;
}

bool HTS221BDUDeactivate(void) {
	uint8_t data;
	
	if (readRegister(HTS221_ADDRESS, CTRL_REG1, &data)) {
		data &= ~BDU_SET;
		if (writeRegister(HTS221_ADDRESS, CTRL_REG1, data))
		return true;
	}
	return false;
}

bool HTS221Activate(void) {
	uint8_t data;
	if (readRegister(HTS221_ADDRESS, CTRL_REG1, &data)) {
		data |= POWER_UP;
		if (writeRegister(HTS221_ADDRESS, CTRL_REG1, data))
		return true;
	}
	return false;
}

bool HTS221Deactivate(void) {
	uint8_t data;
	
	if (readRegister(HTS221_ADDRESS, CTRL_REG1, &data)) {
		data &= ~POWER_UP;
		if (writeRegister(HTS221_ADDRESS, CTRL_REG1, data))
		return true;
	}
	return false;
}


bool HTS221getValues(char *buffer){
	
	if (readRegister(HTS221_ADDRESS, STATUS_REG, (uint8_t*)buffer)) {
		if (*buffer & TEMPERATURE_READY) {
			if (readRegister(HTS221_ADDRESS, TEMP_H_REG, (uint8_t*)buffer)) {
				if (readRegister(HTS221_ADDRESS, TEMP_L_REG, (uint8_t*)buffer)) {
				}
			}
		}
		if (*buffer & HUMIDITY_READY) {
			if (readRegister(HTS221_ADDRESS, HUMIDITY_H_REG, (uint8_t*)buffer)) {
				if (readRegister(HTS221_ADDRESS, HUMIDITY_L_REG, (uint8_t*)buffer)) {
				}
			}
		}
		return true;
	} else
	return false;
}