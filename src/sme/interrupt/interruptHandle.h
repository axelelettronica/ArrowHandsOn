/*
 * interruptHandle.h
 *
 * Created: 1/21/2015 10:14:10 PM
 *  Author: mfontane
 */ 


#ifndef INTERRUPTHANDLE_H_
#define INTERRUPTHANDLE_H_

#include <stdint-gcc.h>


#define I9AX_INT_2_POS    0
#define I9AX_INT_1_POS    1
#define I9AX_DRDY_POS     2
#define I9AX_INT_POS      3
#define PRE_INT_POS       4
#define ALS_GPIO0_POS     5
#define ALS_GPIO1_POS     6
#define SFX_STDBY_STS_POS 7
#define SFX_RADIO_STS_POS 8
#define NFC_FD_POS        9
#define HUT_DRDY_POS      10

#define MASK_I9AX_INT_2    0x01
#define MASK_I9AX_INT_1    0x02
#define MASK_I9AX_DRDY_M   0x04
#define MASK_I9AX_INT_M    0x08
#define MASK_PRE_INT       0x10
#define MASK_ALS_GPIO0     0x20
#define MASK_ALS_GPIO1     0x40

#define MASK_SFX_STDBY_STS 0x0100
#define MASK_SFX_RADIO_STS 0x0200
#define MASK_NFC_FD_INT    0x1000
#define MASK_HUT_DRDY      0x2000



#define BUTTON_0_INT 0x800

#define MAX_EXTERN_INTERRUPT 11


void interruptValueInitializzation(void); // con C++ avrei fatto interruptInitializzation
uint16_t interruptDetection(void);

#endif /* INTERRUPTHANDLE_H_ */