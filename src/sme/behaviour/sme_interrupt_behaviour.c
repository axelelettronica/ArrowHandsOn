#include <stdint-gcc.h>
#include "interrupt\interruptHandle.h"
/*
 * sme_interrupt_behaviour.c
 *
 * Created: 3/1/2015 9:44:06 PM
 *  Author: mfontane
 */ 
/*
* First Use case
detect NFC interrupt:
1) take data from sensor
2) take GPS position
3) Send all to SigFox
*/
void performExecution( uint16_t intDetection) {
    
    uint8_t data;

    // check which is the interrupt that wake-up the task
    if ((intDetection & MASK_NFC_FD_INT) == MASK_NFC_FD_INT) {
        data++;
    }
}