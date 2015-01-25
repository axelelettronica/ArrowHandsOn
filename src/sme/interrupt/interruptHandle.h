/*
 * interruptHandle.h
 *
 * Created: 1/21/2015 10:14:10 PM
 *  Author: mfontane
 */ 


#ifndef INTERRUPTHANDLE_H_
#define INTERRUPTHANDLE_H_

#include <stdint-gcc.h>


#define I9AX_INT_0_POS    0
#define I9AX_INT_1_POS    1
#define I9AX_DRDY_POS     2
#define I9AX_INT_3_POS    3
#define PRE_INT_POS       4
#define HUT_DRDY_POS      5
#define ALS_GPIO1_POS     6
#define ALS_GPIO2_POS     7
#define NFC_FD_POS        8
#define SFX_RADIO_STS_POS 9
#define SFX_STDBY_STS_POS 0xa


#define I9AX_INT_2    0x01
#define I9AX_INT_1    0x02
#define I9AX_DRDY_M   0x04
#define I9AX_INT_M    0x08
#define PRE_INT       0x10
#define HUT_DRDY      0x20
#define ALS_GPIO1     0x40
#define ALS_GPIO2     0x80
#define NFC_FD        0x100
#define SFX_RADIO_STS 0x200
#define SFX_STDBY_STS 0x400

#define MAX_EXTERN_INTERRUPT 11


void interruptValueInitializzation(void); // con C++ avrei fatto interruptInitializzation
uint16_t interruptDetection(void);

#endif /* INTERRUPTHANDLE_H_ */