/*
* LSM9DS1.c
*
* Created: 05/01/2015 19:49:04
*  Author: mfontane
*/
#include "LSM9DS1.h"
#include "..\I2C.h"

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
* (00: ?2g; 01: ?16 g; 10: ?4 g; 11: ?8 g)
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

/* Accelerometer / Gyroscope */
#define LSM9DS1_AG_ADDRESS     0x6A

#define AG_WHO_AM_I            0x0F
#define AG_WHO_AM_I_RETURN     0x68

#define AG_CTRL_REG1_G         0x10
#define AG_ODR_SET             0x20

/* Magnetometer */

#define LSM9DS1_M_ADDRESS      0x1C

#define M_WHO_AM_I            0x0F
#define M_WHO_AM_I_RETURN     0x68

#define M_CTRL_REG1_G         0x10
#define M_ODR_SET             0x20

#endif


/*****************************************************************************/
/*                         Accelerometer / Gyroscope                         */
/*****************************************************************************/

bool LSM9DS1_A_Init(void) 
{
	uint8_t data;
	if (readRegister(LSM9DS1_AG_ADDRESS, AG_WHO_AM_I, &data)) {
    	if (data == AG_WHO_AM_I_RETURN){
        	if (LSM9DS1_A_Activate()){
            	return true;
        	}
    	}
	}
    return false;
}

// Same physical sensor of Accelerometer
bool LSM9DS1_G_Init(void) 
{
    // same sensor as Accelerometer, skipping
    return 1; 
}


bool LSM9DS1_A_getValues(uint16_t *buffer) 
{
    *buffer = 1;
/*
    uint8_t data = 0;
    uint8_t read = 0;

	if (readRegister(LSM9DS1_AG_ADDRESS, STATUS_REG, &data)) {
    	if (data & TEMPERATURE_READY) {
			if (readRegister(LSM9DS1_AG_ADDRESS, TEMP_H_REG, &read)) {
                ((uint8_t*)buffer)[0] = read;
				if (readRegister(LSM9DS1_AG_ADDRESS, TEMP_L_REG, &read)) {
                    ((uint8_t*)buffer)[1] = read;
				}
			}
		} else {
            return false;
        }

        if (data & PRESSURE_READY) {
			if (readRegister(LSM9DS1_AG_ADDRESS, PRESSURE_H_REG, &read)) {
                ((uint8_t*)buffer)[2] = read;
				if (readRegister(LSM9DS1_AG_ADDRESS, PRESSURE_L_REG,&read)) {
                    ((uint8_t*)buffer)[3] = read;
                	if (readRegister(LSM9DS1_AG_ADDRESS, PRESSURE_XL_REG, &read)) {
                        ((uint8_t*)buffer)[4] = read;
                	}
				}
			}
		} else {
            return false;
        }

		return true;
	} else
	    return false;
*/

    return true;
}


bool LSM9DS1_G_getValues(uint16_t *buffer) 
{
     return LSM9DS1_A_getValues(buffer);   //TBD
}


bool LSM9DS1_A_Activate(void) {
    uint8_t data;

    
    if (readRegister(LSM9DS1_AG_ADDRESS, AG_CTRL_REG1_G, &data)) {
        //data |= POWER_UP;
        data |= AG_ODR_SET;
        if (writeRegister(LSM9DS1_AG_ADDRESS, AG_CTRL_REG1_G, data)) {
            return true;
        }
    }
    return false;
}

bool LSM9DS1_G_Activate(void) {
    return LSM9DS1_A_Activate();
}


bool LSM9DS1_A_Deactivate(void) {
    uint8_t data;
    
    if (readRegister(LSM9DS1_AG_ADDRESS, AG_CTRL_REG1_G, &data)) {
        data &= ~AG_ODR_SET;
        if (writeRegister(LSM9DS1_AG_ADDRESS, AG_CTRL_REG1_G, data))
            return true;
    }
    return false;
}


bool LSM9DS1_G_Deactivate(void) {
    return LSM9DS1_A_Deactivate();
}

bool LSM9DS1_A_Decode(uint16_t *buffer, uint16_t *data1, uint16_t *data2, uint16_t *data3)
{
/*
    double t_temp = 0.0;
    double p_temp = 0.0;
    uint16_t t_out;
    uint32_t p_out;

    // Decode Temperature
    t_out =  ((uint8_t*)buffer)[0] << 8; // MSB
    t_out |= ((uint8_t*)buffer)[1];       // LSB
    t_temp = 42.5 +((int16_t)t_out/480);
    *data1 = t_temp*10;  // temp in 1/10 Celsius degree

    // Decode pressure
    p_out  =  ((uint8_t*)buffer)[2] << 16; // MSB
    p_out |=  ((uint8_t*)buffer)[3] << 8;  // LSB
    p_out |=  ((uint8_t*)buffer)[4];       // XLSB
    p_temp = ((int32_t) p_out) / 4096.0;
    *data2 = p_temp;
    */
    return true;
}

bool LSM9DS1_G_Decode(uint16_t *buffer, uint16_t *data1, uint16_t *data2, uint16_t *data3)
{
    return true;
}

/*****************************************************************************/
/*                                Magnetometer                               */
/*****************************************************************************/

bool LSM9DS1_M_Init(void) 
{
	uint8_t data;
	if (readRegister(LSM9DS1_M_ADDRESS, M_WHO_AM_I, &data)) {
    	if (data == M_WHO_AM_I_RETURN){
        	if (LSM9DS1_M_Activate()){
            			return true;
        	}
    	}
	}
    return false;
}



bool LSM9DS1_M_getValues(uint16_t *buffer) 
{
    *buffer = 1;
/*
    uint8_t data = 0;
    uint8_t read = 0;

	if (readRegister(LSM9DS1_M_ADDRESS, STATUS_REG, &data)) {
    	if (data & TEMPERATURE_READY) {
			if (readRegister(LSM9DS1_M_ADDRESS, TEMP_H_REG, &read)) {
                ((uint8_t*)buffer)[0] = read;
				if (readRegister(LSM9DS1_ADDRESS, TEMP_L_REG, &read)) {
                    ((uint8_t*)buffer)[1] = read;
				}
			}
		} else {
            return false;
        }

        if (data & PRESSURE_READY) {
			if (readRegister(LSM9DS1_M_ADDRESS, PRESSURE_H_REG, &read)) {
                ((uint8_t*)buffer)[2] = read;
				if (readRegister(LSM9DS1_M_ADDRESS, PRESSURE_L_REG,&read)) {
                    ((uint8_t*)buffer)[3] = read;
                	if (readRegister(LSM9DS1_M_ADDRESS, PRESSURE_XL_REG, &read)) {
                        ((uint8_t*)buffer)[4] = read;
                	}
				}
			}
		} else {
            return false;
        }

		return true;
	} else
	    return false;
*/

    return true;
}

bool LSM9DS1_M_Activate(void) {
    uint8_t data;

    // use defaults
    return true;

}

bool LSM9DS1_M_Deactivate(void) {
    uint8_t data;

    // use default
    return true;
}

bool LSM9DS1_M_Decode(uint16_t *buffer, uint16_t *data1, uint16_t *data2, uint16_t *data3)
{
/*
    double t_temp = 0.0;
    double p_temp = 0.0;
    uint16_t t_out;
    uint32_t p_out;

    // Decode Temperature
    t_out =  ((uint8_t*)buffer)[0] << 8; // MSB
    t_out |= ((uint8_t*)buffer)[1];       // LSB
    t_temp = 42.5 +((int16_t)t_out/480);
    *data1 = t_temp*10;  // temp in 1/10 Celsius degree

    // Decode pressure
    p_out  =  ((uint8_t*)buffer)[2] << 16; // MSB
    p_out |=  ((uint8_t*)buffer)[3] << 8;  // LSB
    p_out |=  ((uint8_t*)buffer)[4];       // XLSB
    p_temp = ((int32_t) p_out) / 4096.0;
    *data2 = p_temp;
    */
    return true;
}