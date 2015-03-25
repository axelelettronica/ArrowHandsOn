#include <stdint-gcc.h>
#include "interrupt\interruptHandle.h"
#include "Devices\I2C\IOExpander\tca6416a.h"
/*
 * sme_interrupt_behaviour.c
 *
 * Created: 3/1/2015 9:44:06 PM
 *  Author: smkk
 */ 
/*
* First Use case
detect NFC interrupt:
1) take data from sensor
2) take GPS position
3) Send all to SigFox
*/
volatile uint16_t newValue;
void performExecution( uint16_t intDetection) {       
    uint8_t data;
    // check which is the interrupt that wake-up the task
    if ((intDetection & MASK_NFC_FD_INT) == MASK_NFC_FD_INT) {
         
         TCA6416a_input_ports_values(&newValue);
        data++;
    }
    
    if ((intDetection & MASK_I9AX_INT_M) == MASK_I9AX_INT_M) { 
        TCA6416a_input_ports_values(&newValue);
        data++;
    }
}
