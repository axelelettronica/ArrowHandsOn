/*
* tca6416a.c
*
* Created: 1/21/2015 8:46:36 PM
*  Author: mfontane
*/

#include "tca6416a.h"
#include "..\I2C.h"

#define CONF_PORT_0 0x7F
#define CONF_PORT_1 0x35//0x3F

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

#define TCA6416A_ADDRESS 0x20
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

#define _9AX_INT2_A_G   0x1
#define _9AX_INT1_A_G   0x2
#define _9AX_INT_M      0x4
#define _9AX_DRDY_M     0x8
#define PRE_INT         0x10
#define ALS_GPIO0       0x20
#define ALS_GPIO1       0x40
#define BLE_RESET       0x80

#define SFX_STDBY_STS   0x1
#define SFX_RESET       0x2
#define SFX_RADIO_STS   0x4
#define SFX_WAKEUP      0x8
#define NFC_FD          0x10
#define HUT_DRDY        0x20
#define GPS_FORCE_ON    0x40
#define GPS_RESET       0x80

#define INIT_P0         0b10000000
#define INIT_P1         0b11001010
#define RESET_P1        0xFE
#define RESET_P2        0xBE
/*
Initialize the output extension with the configuration of input/output ports.
Keep the reset High
*/
bool TCA6416a_init(void) {
    bool ret = false;
    //readRegister(TCA6416A_ADDRESS, CONFIG_PORT_0, &ret); //debug
    if (writeRegister(TCA6416A_ADDRESS, CONFIG_PORT_0, CONF_PORT_0)!=false) {
        writeRegister(TCA6416A_ADDRESS, OUTPUT_PORT_0, INIT_P0); // keep the BLE reset High
        
        ret = true;
    }

    if (writeRegister(TCA6416A_ADDRESS, CONFIG_PORT_1, CONF_PORT_1)!=false){
        writeRegister(TCA6416A_ADDRESS, OUTPUT_PORT_1, INIT_P1); // keep the resets High
        ret =  true;
        } else {
        ret = false;
    }
    
    return ret;
}

volatile uint8_t resets[4];
bool TCA6416a_reset_devices(void){
    
    uint8_t actual;
    writeRegister(TCA6416A_ADDRESS, OUTPUT_PORT_0, RESET_P1);
    readRegister(TCA6416A_ADDRESS, OUTPUT_PORT_0,(uint8_t *)&resets[0] );
    
    if (readRegister(TCA6416A_ADDRESS, OUTPUT_PORT_1, (uint8_t *)&actual) != false) {
        writeRegister(TCA6416A_ADDRESS, OUTPUT_PORT_1, ((actual&RESET_P2)&(~SFX_WAKEUP)));
            readRegister(TCA6416A_ADDRESS, OUTPUT_PORT_1,(uint8_t *)&resets[1]);
    }
    
    uint8_t delay;
    // wait a while
    for (int i=0; i<1000; i++) {
        delay++;
    }

    // move the reset high
    writeRegister(TCA6416A_ADDRESS, OUTPUT_PORT_0, ~RESET_P1);
    readRegister(TCA6416A_ADDRESS, OUTPUT_PORT_0,(uint8_t *)&resets[2] );
    
    writeRegister(TCA6416A_ADDRESS, OUTPUT_PORT_1, actual);
    readRegister(TCA6416A_ADDRESS, OUTPUT_PORT_1,(uint8_t *)&resets[3] );
    
    
    //ENABLE Interrupt
    extint_chan_enable_callback(SME_INT_IOEXT_EIC_LINE,	EXTINT_CALLBACK_TYPE_DETECT);
    resets[0]=1;
}

bool TCA6416a_input_port0_values(char *buffer) {
    readRegister(TCA6416A_ADDRESS, INPUT_PORT_0, (uint8_t *)buffer);
    return false;
}

bool TCA6416a_input_port1_values(char *buffer) {
    readRegister(TCA6416A_ADDRESS, INPUT_PORT_1, (uint8_t *)buffer);
    return false;
}

bool TCA6416a_input_ports_values(uint16_t *buffer) {
    uint8_t portValue;
    TCA6416a_input_port0_values((char *)&portValue);
    ((char*)buffer)[0] = portValue;

    TCA6416a_input_port1_values((char *)&portValue);
    ((char*)buffer)[1] = portValue;
    return false;
}

