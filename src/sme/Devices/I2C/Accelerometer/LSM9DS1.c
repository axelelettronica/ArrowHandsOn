/*
* LSM9DS1.c
*
* Created: 05/01/2015 19:49:04
*  Author: mfontane
*/
#include "LSM9DS1.h"
#if NOT_SENSOR
// The SparkFun breakout board defaults to 1, set to 0 if SA0 jumper on the bottom of the board is set
#define LSM9DS1_ADDRESS 0x1D


//Define a few of the registers that we will be accessing on the MMA8452

#define WHO_AM_I        0x0D
#define WHO_AM_I_RETURN 0x2A


/*
the accelerometer operates in normal mode and the gyroscope is powered down.
put this in PD(Power Down) it is possible to set both Accel and Gyro (using CTRL_REG1_G)
* ODR_XL [5:7]
* Output data rate and power mode selection. default value: 000
* 0 0 0 Power-down
* 0 0 1 10 Hz 0x20
* 0 1 0 50 Hz 0x40
* 0 1 1 119 Hz0x60
* 1 0 0 238 Hz0x80
* 1 0 1 476 Hz0xa0
* 1 1 0 952 Hz
* 1 1 1 n.a.
*
* FS_XL [3:4]
* Accelerometer full-scale selection. Default value: 00
* (00: ±2g; 01: ±16 g; 10: ±4 g; 11: ±8 g)
*
*  BW_SCAL_ODR [2]
* Bandwidth selection. Default value: 0
* (0: bandwidth determined by ODR selection:
* - BW = 408 Hz when ODR = 952 Hz, 50 Hz, 10 Hz;
* - BW = 211 Hz when ODR = 476 Hz;
* - BW = 105 Hz when ODR = 238 Hz;
* - BW = 50 Hz when ODR = 119 Hz;
* 1: bandwidth selected according to BW_XL [2:1] selection)
*
* BW_XL [1:0]
* Anti-aliasing filter bandwidth selection. Default value: 00
* (00: 408 Hz; 01: 211 Hz; 10: 105 Hz; 11: 50 Hz)
*/
#define CTRL_REG6_XL 0x20
#define POWER_DOWN   ~0xE0
#define ACCEL_10_Hz  0x20
#define ACCEL_50_Hz  0x40
#define ACCEL_119_Hz 0x60
#define ACCEL_238_Hz 0x80
#define ACCEL_476_Hz 0xA0
#define ACCEL_952_Hz 0xC0

/*
* both accelerometer and gyroscope are activated at the same ODR
* ODR_G [7:5] Gyroscope output data rate selection. Default value: 000
* (Refer to Table 46 and Table 47)
*
* FS_G [3:4] Gyroscope full-scale selection. Default value: 00
* (00: 245 dps; 01: 500 dps; 10: Not Available; 11: 2000 dps)
*
* BW_G [1:0] Gyroscope bandwidth selection. Default value: 00
*/
#define CTRL_REG1_G 0x10

#else


// The SparkFun breakout board defaults to 1, set to 0 if SA0 jumper on the bottom of the board is set
#define LSM9DS1_ADDRESS 0x35


//Define a few of the registers that we will be accessing on the MMA8452

#define WHO_AM_I    0x0F
#define WHO_AM_I_RETURN 0x68
#endif

bool LSM9DS1nit(void) {
    return false;
}



bool LSM9DS1getValues(char *buffer) {
    *buffer = 1;
    return true;
}