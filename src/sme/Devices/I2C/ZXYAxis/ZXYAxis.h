/*
 * ZXYAxis.h
 *
 * Created: 28/12/2014 21:19:07
 *  Author: mfontane
 */ 


#ifndef ZXYAXIS_H_
#define ZXYAXIS_H_

//Define a few of the registers that we will be accessing on the MMA8452
#define OUT_X_MSB 0x01
#define XYZ_DATA_CFG  0x0E
#define WHO_AM_I   0x0D
#define CTRL_REG1  0x2A

#define WHO_AM_I_RETURN 0x2A // WHO_AM_I should always be 0x2A


// The SparkFun breakout board defaults to 1, set to 0 if SA0 jumper on the bottom of the board is set
#define MMA8452_ADDRESS 0x1D  // 0x1D if SA0 is high, 0x1C if low
#include "privateZXYAxis.h"

bool ZXYInit(void);

uint8_t MMA8452isConnected(void);
uint8_t MMA8452Configure(void);
bool MMA8452getAccelData(char *buffer);


#endif /* ZXYAXIS_H_ */