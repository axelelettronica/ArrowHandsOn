/*
* ZXYAxis.c
*
* Created: 28/12/2014 21:18:22
*  Author: mfontane
*/

#include "ZXYAxis.h"
#include "../I2C.h"

bool ZXYInit(void){
	uint8_t ret;
	if (readRegister(MMA8452_ADDRESS, WHO_AM_I, &ret)) {
		return (ret == WHO_AM_I_RETURN);
		} else {
		return false;
	}
}



uint8_t MMA8452Configure(void){
	uint8_t data = MMA8452Standby();
	
	// Set up the full scale range to 2, 4, or 8g.
	uint8_t fsr = GSCALE;
	if(fsr > 8) fsr = 8; //Easy error check
	fsr >>= 2; // Neat trick, see page 22. 00 = 2G, 01 = 4A, 10 = 8G
	data = writeRegister(MMA8452_ADDRESS, XYZ_DATA_CFG, fsr);

	//The default data rate is 800Hz and we don't modify it in this example code

	data = MMA8452Active();  // Set to active to start reading

	return (data == 1);
}


uint8_t MMA8452Standby(void){
	uint8_t data=0;
	if (readRegister(MMA8452_ADDRESS, CTRL_REG1, &data)) {
		writeRegister(MMA8452_ADDRESS, CTRL_REG1, data & ~(0x01));
	}

	return data;
}


uint8_t MMA8452Active(void){
	uint8_t data=0;
	if (readRegister(MMA8452_ADDRESS, CTRL_REG1, &data)) {
		writeRegister(MMA8452_ADDRESS, CTRL_REG1, data | 0x01); //Set the active bit to begin detection
	}

	return data;
}

bool  MMA8452getAccelData(char *buffer) {

	if ( readRegister(MMA8452_ADDRESS, CTRL_REG1, (uint8_t*)buffer)) {
		
		return true;
	}
	
	return false;
}