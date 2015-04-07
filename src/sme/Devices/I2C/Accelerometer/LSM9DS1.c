/*
* LSM9DS1.c
*
* Created: 05/01/2015 19:49:04
*  Author: smkk
*/
#include "LSM9DS1.h"
#include "../../I2CFreeRtos.h"
#include "sme_cdc_io.h"
#include "sme_cmn.h"
#include "../../../model/sme_model_i2c.h"

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

#define AG_STATUS_REG          0x27
#define AG_ACCELEROMETER_READY 0x01
#define AG_GYROSCOPE_READY     0x02
#define AG_ACC_X_L             0x28
#define AG_ACC_X_H             0x29
#define AG_ACC_Y_L             0x2A
#define AG_ACC_Y_H             0x2B
#define AG_ACC_Z_L             0x2C
#define AG_ACC_Z_H             0x2D

#define AG_GYR_X_L             0x18
#define AG_GYR_X_H             0x19
#define AG_GYR_Y_L             0x1A
#define AG_GYR_Y_H             0x1B
#define AG_GYR_Z_L             0x1C
#define AG_GYR_Z_H             0x1D
/* Magnetometer */

#define LSM9DS1_M_ADDRESS      0x1C

#define M_WHO_AM_I            0x0F
#define M_WHO_AM_I_RETURN     0x3D

#define M_CTRL_REG1_G         0x20
#define M_ODR_SET             0x20
#define M_CTRL_REG3_G         0x22
#define M_OPER_MODE_CONT      0x00
#define M_OPER_MODE_SINGLE    0x01
#define M_OPER_MODE_DIS       0x03

#define M_STATUS_REG          0x27
#define M_ZYX_AXIS_READY      0x08

#define M_X_L                 0x28
#define M_X_H                 0x29
#define M_Y_L                 0x2A
#define M_Y_H                 0x2B
#define M_Z_L                 0x2C
#define M_Z_H                 0x2D


/*Sensors Sensitivity */

/*
 * Linear acceleration : Reg6_XL  FS[1:0] = 00g +-2, 10+-4g 11+-8g
 * ranges: 
 * +-2  g       mg/LSB = 0.061
 * +-4  g       mg/LSB = 0.122
 * +-8  g       mg/LSB = 0.244
 */
#define A_LSB_SENSIT_2MG       0.061
#define A_LSB_SENSIT_4MG       0.122
#define A_LSB_SENSIT_8MG       0.244

/*
 * Angular rate: CTRL_REG1  FS_G[1:0] 00 245, 01 500, 11 2000
 * +-245    mdps/LSB = 8.75
 * +-500    mdps/LSB = 17.50
 * +-2000   mdps/LSB = 70
 */
#define G_LSB_SENSIT_245MDPS    8.75
#define G_LSB_SENSIT_500DPS    17.5
#define G_LSB_SENSIT_2KMDPS    70
/*
 * Magnetic :  REG2_M FS[1:0] 00 +-4 01 +-8 10 +-12 11+-16
 * +-4       mgauss/LSB = 0.14
 * +-8       mgauss/LSB = 0.29
 * +-12      mgauss/LSB = 0.43
 * +-16      mgauss/LSB = 0.58
 */
#define M_LSB_SENSIT_4MG        0.14
#define M_LSB_SENSIT_8MG        0.29
#define M_LSB_SENSIT_12MG       0.43
#define M_LSB_SENSIT_16MG       0.58

#endif

float  a_lsb_sentivity = A_LSB_SENSIT_2MG;     //  FC[1:0] register set to 0
inline float LSM9DS1_get_a_lsb_sensitivity(void) {
    return a_lsb_sentivity;
}

float  g_lsb_sentivity = G_LSB_SENSIT_245MDPS; //  FC[1:0] register set to 0
inline float LSM9DS1_get_g_lsb_sentivity(void) {
    return g_lsb_sentivity;
}

float  m_lsb_sentivity = M_LSB_SENSIT_4MG;     //  FC[1:0] register set to 0
inline float LSM9DS1_get_m_lsb_sentivity(void) {
    return m_lsb_sentivity;
}
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
    return true; 
}


bool LSM9DS1_A_getValues(uint16_t *buffer) 
{
    uint8_t data = 0;
    uint8_t read = 0;
    uint8_t i = 0;

	if (readRegister(LSM9DS1_AG_ADDRESS, AG_STATUS_REG, &data)) {
    	if (data & AG_ACCELEROMETER_READY) {
			if (!readRegister(LSM9DS1_AG_ADDRESS, AG_ACC_X_L, &read)) {
                return false;
            } 
            ((uint8_t*)buffer)[i++] = read;
	     	if (!readRegister(LSM9DS1_AG_ADDRESS, AG_ACC_X_H, &read)) {
               return false;
            }
            ((uint8_t*)buffer)[i++] = read;
			if (!readRegister(LSM9DS1_AG_ADDRESS, AG_ACC_Y_L, &read)) {
                return false;
			}
			((uint8_t*)buffer)[i++] = read;
			if (!readRegister(LSM9DS1_AG_ADDRESS, AG_ACC_Y_H, &read)) {
                return false;
			}
			((uint8_t*)buffer)[i++] = read;
			if (!readRegister(LSM9DS1_AG_ADDRESS, AG_ACC_Z_L, &read)) {
                return false;
			}
			((uint8_t*)buffer)[i++] = read;
			if (!readRegister(LSM9DS1_AG_ADDRESS, AG_ACC_Z_H, &read)) {
                return false;
			}
			((uint8_t*)buffer)[i++] = read;
        } else {
            return false;
        }
	} else {
	    return false;
    }

    return true;
}


bool LSM9DS1_G_getValues(uint16_t *buffer) 
{
    uint8_t data = 0;
    uint8_t read = 0;
    uint8_t i = 0;

    if (readRegister(LSM9DS1_AG_ADDRESS, AG_STATUS_REG, &data)) {
        if (data & AG_GYROSCOPE_READY) {
            if (!readRegister(LSM9DS1_AG_ADDRESS, AG_GYR_X_L, &read)) {
                return false;
            }
            ((uint8_t*)buffer)[i++] = read;
            if (!readRegister(LSM9DS1_AG_ADDRESS, AG_GYR_X_H, &read)) {
                return false;
            }
            ((uint8_t*)buffer)[i++] = read;
            if (!readRegister(LSM9DS1_AG_ADDRESS, AG_GYR_Y_L, &read)) {
                return false;
            }
            ((uint8_t*)buffer)[i++] = read;
            if (!readRegister(LSM9DS1_AG_ADDRESS, AG_GYR_Y_H, &read)) {
                return false;
            }
            ((uint8_t*)buffer)[i++] = read;
            if (!readRegister(LSM9DS1_AG_ADDRESS, AG_GYR_Z_L, &read)) {
                return false;
            }
            ((uint8_t*)buffer)[i++] = read;
            if (!readRegister(LSM9DS1_AG_ADDRESS, AG_GYR_Z_H, &read)) {
                return false;
            }
            ((uint8_t*)buffer)[i++] = read;
       } else {
           return false;
       }
   } else {
       return false;
   } 

   return true;
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
    float x_f = 0, y_f = 0, z_f = 0;
    // Decode Accel x-axis
    *data1  = ((uint8_t*)buffer)[0];      // LSB
    *data1 |= ((uint8_t*)buffer)[1] << 8; // MSB

    // Decode Accel y-axis
    *data2  = ((uint8_t*)buffer)[2];      // LSB
    *data2 |= ((uint8_t*)buffer)[3] << 8; // MSB

    // Decode Accel z-axis
    *data3  = ((uint8_t*)buffer)[4];      // LSB
    *data3 |= ((uint8_t*)buffer)[5] << 8; // MSB
 
    x_f = ((int16_t)(*data1)*a_lsb_sentivity);
    y_f = ((int16_t)(*data2)*a_lsb_sentivity);
    z_f = ((int16_t)(*data3)*a_lsb_sentivity);

    print_dbg("Accelerometer X: %8d mg      Y: %8d mg      Z: %8d mg\n",
             (int16_t) x_f, (int16_t)y_f, (int16_t)z_f);
    return true;
}




bool LSM9DS1_G_Decode(uint16_t *buffer, uint16_t *data1, uint16_t *data2, uint16_t *data3)
{
    float x_f = 0, y_f = 0, z_f = 0;

    // Decode Gyroscope x-axis
    *data1  = ((uint8_t*)buffer)[0];      // LSB
    *data1 |= ((uint8_t*)buffer)[1] << 8; // MSB

    // Decode Gyroscope y-axis
    *data2  = ((uint8_t*)buffer)[2];      // LSB
    *data2 |= ((uint8_t*)buffer)[3] << 8; // MSB

    // Decode Gyroscope z-axis
    *data3  = ((uint8_t*)buffer)[4];      // LSB
    *data3 |= ((uint8_t*)buffer)[5] << 8; // MSB
       
    x_f = ((int16_t)(*data1)*g_lsb_sentivity);
    y_f = ((int16_t)(*data2)*g_lsb_sentivity);
    z_f = ((int16_t)(*data3)*g_lsb_sentivity);

    print_dbg("Gyroscope     X: %8d mdps    Y: %8d mdps    Z: %8d mdps\n",
             (int16_t) x_f, (int16_t)y_f, (int16_t)z_f);


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
    uint8_t data = 0;
    uint8_t read = 0;
    uint8_t i = 0;
    #define M_STATUS_REG          0x27
    #define M_ZYX_AXIS_READY      0x08

    if (readRegister(LSM9DS1_M_ADDRESS, M_STATUS_REG, &data)) {
        if (data & M_ZYX_AXIS_READY) {
            if (!readRegister(LSM9DS1_M_ADDRESS, M_X_L, &read)) {
            }
            ((uint8_t*)buffer)[i++] = read;
            if (!readRegister(LSM9DS1_M_ADDRESS, M_X_H, &read)) {
            }
            ((uint8_t*)buffer)[i++] = read;
            if (!readRegister(LSM9DS1_M_ADDRESS, M_Y_L, &read)) {
            }
            ((uint8_t*)buffer)[i++] = read;
            if (!readRegister(LSM9DS1_M_ADDRESS, M_Y_H, &read)) {
            }
            ((uint8_t*)buffer)[i++] = read;
            if (!readRegister(LSM9DS1_M_ADDRESS, M_Z_L, &read)) {
            }
            ((uint8_t*)buffer)[i++] = read;
            if (!readRegister(LSM9DS1_M_ADDRESS, M_Z_H, &read)) {
            }
            ((uint8_t*)buffer)[i++] = read;
        } else {
            return false;
        }       
    } else {
        return false;
    }

    return true;
}

bool LSM9DS1_M_Activate(void)
{
    uint8_t data; 
    if (readRegister(LSM9DS1_M_ADDRESS, M_CTRL_REG3_G, &data)) {
        //data |= POWER_UP;
        //data |= 0x21;
        data &= ~M_OPER_MODE_DIS;
        //data &= (0xFC);
        if (writeRegister(LSM9DS1_M_ADDRESS, M_CTRL_REG3_G, data)) {
            return true;
        }
    }
    return false;
}

bool LSM9DS1_M_Deactivate(void) {
    uint8_t data; 
    if (readRegister(LSM9DS1_M_ADDRESS, M_CTRL_REG3_G, &data)) {
        //data |= POWER_UP;
        //data |= 0x21;
        data |= M_OPER_MODE_DIS;
        if (writeRegister(LSM9DS1_M_ADDRESS, M_CTRL_REG3_G, data)) {
            return true;
        }
    }
    return false;
}

bool LSM9DS1_M_Decode(uint16_t *buffer, uint16_t *data1, uint16_t *data2, uint16_t *data3)
{
    float x_f = 0, y_f = 0, z_f = 0;

    // Decode Magnetometer x-axis
    *data1  = ((uint8_t*)buffer)[0];      // LSB
    *data1 |= ((uint8_t*)buffer)[1] << 8; // MSB

    // Decode Magnetometer y-axis
    *data2  = ((uint8_t*)buffer)[2];      // LSB
    *data2 |= ((uint8_t*)buffer)[3] << 8; // MSB

    // Decode Magnetometer z-axis
    *data3  = ((uint8_t*)buffer)[4];      // LSB
    *data3 |= ((uint8_t*)buffer)[5] << 8; // MSB

    x_f = ((int16_t)(*data1)*g_lsb_sentivity);
    y_f = ((int16_t)(*data2)*g_lsb_sentivity);
    z_f = ((int16_t)(*data3)*g_lsb_sentivity);

    print_dbg("Magnetometer  X: %8d mgauss  Y: %8d mgauss  Z: %8d mgauss\n",
             (int16_t) x_f, (int16_t)y_f, (int16_t)z_f);
    return true;
}


