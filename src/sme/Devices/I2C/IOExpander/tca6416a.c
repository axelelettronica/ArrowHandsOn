/*
* tca6416a.c
*
* Created: 1/21/2015 8:46:36 PM
*  Author: mfontane
*/

#include "tca6416a.h"
#include "..\I2C.h"

#define INIT_CONF_PORT_0 0xFF    // all ports in input
#define INIT_CONF_PORT_1 0x07    // first 3 ports in input

#if NOT_SENSOR
#define TCA6416A_ADDRESS 0x1D


#define INPUT_PORT_0    0x0D //Read byte          xxxx xxxx (undefined)
#define INPUT_PORT_1    0x13 //Read byte          xxxx xxxx
#define OUTPUT_PORT_0   0x02 //Read/write byte    1111 1111
#define OUTPUT_PORT_1   0x03 //Read/write byte    1111 1111
#define P_INVERT_PORT_0 0x04 //Read/write byte    0000 0000
#define P_INVERT_PORT_1 0x05 //Read/write byte    0000 0000
#define CONFIG_PORT_0   0x0E //Read/write byte    1111 1111
#define CONFIG_PORT_1   0x0F //Read/write byte    1111 1111
#else

#define TCA6416A_ADDRESS 0x2E
// Protocol          PowerOn Default
#define INPUT_PORT_0    0x00 //Read byte          xxxx xxxx (undefined)
#define INPUT_PORT_1    0x01 //Read byte          xxxx xxxx
#define OUTPUT_PORT_0   0x02 //Read/write byte    1111 1111
#define OUTPUT_PORT_1   0x03 //Read/write byte    1111 1111
#define P_INVERT_PORT_0 0x04 //Read/write byte    0000 0000
#define P_INVERT_PORT_1 0x05 //Read/write byte    0000 0000
#define CONFIG_PORT_0   0x06 //Read/write byte    1111 1111
#define CONFIG_PORT_1   0x07 //Read/write byte    1111 1111

#endif

//volatile uint8_t ret=0x10;
uint8_t ret=0x10;
bool TCA6416aInit(void) {

    readRegister(TCA6416A_ADDRESS, CONFIG_PORT_0, &ret); //debug
    if (writeRegister(TCA6416A_ADDRESS, CONFIG_PORT_0, INIT_CONF_PORT_0)) {
        readRegister(TCA6416A_ADDRESS, CONFIG_PORT_0, &ret);//debug
        readRegister(TCA6416A_ADDRESS, CONFIG_PORT_1, &ret);//debug
        if (writeRegister(TCA6416A_ADDRESS, CONFIG_PORT_1, INIT_CONF_PORT_1)){
            readRegister(TCA6416A_ADDRESS, CONFIG_PORT_1, &ret);//debug
            return true;
        }
    }
    return false;
}


bool TCA6416aPort0Values(char *buffer) {
    readRegister(TCA6416A_ADDRESS, INPUT_PORT_0, (uint8_t *)buffer);
    return false;
}

bool TCA6416aPort1Values(char *buffer) {
    readRegister(TCA6416A_ADDRESS, INPUT_PORT_1, (uint8_t *)buffer);
    return false;
}

bool TCA6416aPortsValues(uint16_t *buffer) {
    uint8_t portValue;
    TCA6416aPort0Values((char *)&portValue);
    ((char*)buffer)[0] = portValue;

    TCA6416aPort1Values((char *)&portValue);
    ((char*)buffer)[1] = portValue;
    return false;
}

