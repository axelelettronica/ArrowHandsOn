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
#define INT15_PIN                   PIN_PA15
#define INT15_ACTIVE                false
#define INT15_INACTIVE              !INT15_ACTIVE
#define INT15_EIC_PIN               PIN_PA15A_EIC_EXTINT15
#define INT15_EIC_MUX               MUX_PA15A_EIC_EXTINT15
#define INT15_EIC_PINMUX            PINMUX_PA15A_EIC_EXTINT15
#define INT15_EIC_LINE              15
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

#endif /* INTERRUPT0_H_ */