#include "..\tasks\sme_controller.h"
/*
* interrupt.h
*
* Created: 08/01/2015 21:30:22
*  Author: mfontane
*/


#ifndef INTERRUPT0_H_
#define INTERRUPT0_H_

/** \name INT15 definitions
*  @{ */
    #ifdef SMARTEVERYTHING
    #define INT_BUTTON1_PIN                   PIN_PB00
    #define INT_BUTTON1__ACTIVE                false
    #define INT_BUTTON1__INACTIVE              !INT_BUTTON1__ACTIVE
    #define INT_BUTTON1_PIN               PIN_PB00A_EIC_EXTINT0
    #define INT_BUTTON1_MUX               MUX_PB00A_EIC_EXTINT0
    #define INT_BUTTON1_EIC_PINMUX            PINMUX_PB00A_EIC_EXTINT0
    #define INT_BUTTON1_EIC_LINE              0
    #else
    #define INT_BUTTON1_PIN                   PIN_PA15
    #define INT_BUTTON1__ACTIVE                false
    #define INT_BUTTON1__INACTIVE              !INT_BUTTON1__ACTIVE
    #define INT_BUTTON1_PIN               PIN_PA15A_EIC_EXTINT15
    #define INT_BUTTON1_MUX               MUX_PA15A_EIC_EXTINT15
    #define INT_BUTTON1_EIC_PINMUX            PINMUX_PA15A_EIC_EXTINT15
    #define INT_BUTTON1_EIC_LINE              15
    #endif


    #ifdef SMARTEVERYTHING
    #define INT_BUTTON2_PIN                   PIN_PA27
    #define INT_BUTTON2__ACTIVE                false
    #define INT_BUTTON2__INACTIVE              !INT_BUTTON2__ACTIVE
    #define INT_BUTTON2_PIN               PIN_PA27A_EIC_EXTINT15
    #define INT_BUTTON2_MUX               MUX_PA27A_EIC_EXTINT15
    #define INT_BUTTON2_EIC_PINMUX            PINMUX_PA27A_EIC_EXTINT15
    #define INT_BUTTON2_EIC_LINE              15
    #else
    #define INT_BUTTON2_PIN                   PIN_PA15
    #define INT_BUTTON2__ACTIVE                false
    #define INT_BUTTON2__INACTIVE              !INT_BUTTON2__ACTIVE
    #define INT_BUTTON2_PIN               PIN_PA15A_EIC_EXTINT15
    #define INT_BUTTON2_MUX               MUX_PA15A_EIC_EXTINT15
    #define INT_BUTTON2_EIC_PINMUX            PINMUX_PA15A_EIC_EXTINT15
    #define INT_BUTTON2_EIC_LINE              15
    #endif
    /** @} */


    /** \name INT0 definitions
    *  @{ */
        #define INT0_PIN                   PIN_PA16
        #define INT0_ACTIVE                false
        #define INT0_INACTIVE              !INT0_ACTIVE
        #define INT0_EIC_PIN               PIN_PA16A_EIC_EXTINT0
        #define INT0_EIC_MUX               MUX_PA16A_EIC_EXTINT0
        #define INT0_EIC_PINMUX            PINMUX_PA16A_EIC_EXTINT0
        #define INT0_EIC_LINE              0
        /** @} */




        void sme_init_isr_global(void);
        void clearInt(eventE interrupt);

        #endif /* INTERRUPT0_H_ */