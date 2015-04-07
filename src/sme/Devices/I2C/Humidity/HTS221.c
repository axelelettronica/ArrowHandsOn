/*
* HTS221.c
*
* Created: 02/01/2015 20:50:20
*  Author: smkk
*/
#include "HTS221.h"
#include "..\..\I2CFreeRtos.h"


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
and lower register parts. In default mode (BDU = ?0?), the lower and upper register parts are
updated continuously. If it is not certain whether the read will be faster than output data rate,
it is recommended to set the BDU bit to ?1?. In this way, after the reading of the lower (upper)
register part, the content of that output register is not updated until the upper (lower) part is
read also.
*
* [1:0] ODR1, ODR0: output data rate selection (see table 17)
*/
#define CTRL_REG1   0x20
#define POWER_UP    0x80
#define BDU_SET     0x4
#define ODR0_SET    0x1   // setting sensor reading period 1Hz

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
/*
#define CALIB_T0_DEGC_X8   0x32
#define CALIB_T1_DEGC_X8   0x33
#define CALIB_T1_T0_MSB    0x35
#define CALIB_T0_OUT_L     0x3C
#define CALIB_T0_OUT_H     0x3D
#define CALIB_T1_OUT_L     0x3E
#define CALIB_T1_OUT_H     0x3F
*/

static inline bool humidityReady(uint8_t data) {
    return (data & 0x02);
}
static inline bool temperatureReady(uint8_t data) {
    return (data & 0x01);
}

static uint8_t h0_rH, h1_rH;
static uint16_t T0_degC, T1_degC, H0_T0, H1_T0, T0_OUT, T1_OUT;


bool HTS221nit(void) {
    uint8_t data;
    if (readRegister(HTS221_ADDRESS, WHO_AM_I, &data)) {
        if (data == WHO_AM_I_RETURN){
            if (HTS221Activate()){
                //HTS221BDUActivate();
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
                    H0_T0 |= data<<8;
                    break;
                    case CALIB_START+0xA:
                    H1_T0 = data;
                    break;
                    case CALIB_START+0xB:
                    H1_T0 |= data <<8;
                    break;
                    case CALIB_START+0xC:
                    T0_OUT = data;
                    break;
                    case CALIB_START+0xD:
                    T0_OUT |= data << 8;
                    break;
                    case CALIB_START+0xE:
                    T1_OUT = data;
                    break;
                    case CALIB_START+0xF:
                    T1_OUT |= data << 8;
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
        data |= ODR0_SET;
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


bool HTS221getValues(uint16_t *buffer)
{
    uint8_t data = 0;
    uint8_t read = 0;
    
    if (readRegister(HTS221_ADDRESS, STATUS_REG, &data)) {
        if (data & TEMPERATURE_READY) {
            if (readRegister(HTS221_ADDRESS, TEMP_H_REG, &read)) {
                ((uint8_t*)buffer)[0] = read;
                if (readRegister(HTS221_ADDRESS, TEMP_L_REG, &read)) {
                    ((uint8_t*)buffer)[1] = read;
                }
            }
            } else {
            return false;
        }

        if (data & HUMIDITY_READY) {
            if (readRegister(HTS221_ADDRESS, HUMIDITY_H_REG, &read)) {
                ((uint8_t*)buffer)[2] = read;
                if (readRegister(HTS221_ADDRESS, HUMIDITY_L_REG, &read)) {
                    ((uint8_t*)buffer)[3] = read;
                }
            }
            } else {
            return false;
        }

        return true;
    } else
    return false;
}

bool HTS221Decode(uint16_t *buffer, uint16_t *data1, uint16_t *data2)
{
    uint16_t t_out = 0;
    uint16_t h_out = 0;
    double t_temp = 0.0;
    double deg = 0.0;
    double h_temp = 0.0;
    double hum = 0.0;

    // Decode Temperature
    t_out =  ((uint8_t*)buffer)[0] << 8; // MSB
    t_out |= ((uint8_t*)buffer)[1];      // LSB

    deg    = ((int16_t)(T1_degC) - (int16_t)(T0_degC))/8.0; // remove x8 multiple

    // Calculate Temperature in decimal of grade centigrades i.e. 15.0 = 150.
    t_temp   = (((int16_t)t_out - (int16_t)T0_OUT) * deg) / ((int16_t)T1_OUT - (int16_t)T0_OUT);
    deg    = ((int16_t)T0_degC) / 8.0; // remove x8 multiple
    *data1 = (int16_t)((deg + t_temp)*10);   // provide signed decimal measurement unit


    // Decode Humidity
    h_out =  ((uint8_t*)buffer)[2] << 8; // MSB
    h_out |= ((uint8_t*)buffer)[3];      // LSB

    hum = ((int16_t)(h1_rH) - (int16_t)(h0_rH))/2.0;  // remove x2 multiple

    // Calculate humidity in decimal of grade centigrades i.e. 15.0 = 150.
    h_temp = (((int16_t)h_out - (int16_t)H0_T0) * hum) / ((int16_t)H1_T0 - (int16_t)H0_T0);
    hum    = ((int16_t)h0_rH) / 2.0; // remove x2 multiple
    *data2 = (int16_t)((hum + h_temp)*10); // provide signed decimal measurement unit

    return true;
}
