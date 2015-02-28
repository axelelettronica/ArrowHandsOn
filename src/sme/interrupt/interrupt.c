/*
* interrupt.c
*
* Created: 08/01/2015 21:30:08
*  Author: mfontane
*/

#include "system_interrupt.h"
#include "asf.h"
#include "..\extint\extint.h"
#include "interrupt.h"
#include "..\tasks\sme_controller.h"
#include "Devices\I2C\IOExpander\tca6416a.h"
#include "Devices\IO\sme_rgb_led.h"
#include "common\sme_cdc_io.h"

controllerQueueS interruptData;
volatile uint8_t intDetect;

bool nurembergSent = false;
static void extint0_detection_callback(void)
{

    bool pin_state = port_pin_get_input_level(SME_BUTTON1_PIN);

    
    //pin_state = port_pin_get_input_level(BUTTON_0_PIN);
    // just for first demo
    if (((intDetect&0x1) != 0x1) && (pin_state == true) && (!nurembergSent)){
        sme_led_green_brightness(HALF_LIGTH);
        nurembergSent = true;
        intDetect |=0x1;
        BaseType_t xHigherPriorityTaskWoken;
        interruptData.intE = button1Int;

        /* We have not woken a task at the start of the ISR. */
        xHigherPriorityTaskWoken = pdFALSE;
        
        xQueueSendToFrontFromISR(controllerQueue, (void *) &interruptData, &xHigherPriorityTaskWoken);
        
        /* Now the buffer is empty we can switch context if necessary.*/
        if( xHigherPriorityTaskWoken ) {
            /* Actual macro used here is port specific.*/
            portYIELD_FROM_ISR (xHigherPriorityTaskWoken);
        }
    }
}

static void extint15_detection_callback(void)
{

    bool pin_state=true;
    pin_state = port_pin_get_input_level(SME_BUTTON2_PIN);

    // just for first demo
    if (((intDetect&0x2) != 0x2) && (pin_state == true) && (!nurembergSent)){

        nurembergSent = true;
        intDetect |=0x2;
        BaseType_t xHigherPriorityTaskWoken;

        interruptData.intE = button2Int;

        /* We have not woken a task at the start of the ISR. */
        xHigherPriorityTaskWoken = pdFALSE;
        
        xQueueSendFromISR(controllerQueue, (void *) &interruptData, &xHigherPriorityTaskWoken);
        
        /* Now the buffer is empty we can switch context if necessary. */
        if( xHigherPriorityTaskWoken )
        {
            /* Actual macro used here is port specific.*/
            portYIELD_FROM_ISR (xHigherPriorityTaskWoken);
        }
    }
}


static void extint06_detection_callback(void)
{

    BaseType_t xHigherPriorityTaskWoken;
    interruptData.intE = externalInt;

    /* We have not woken a task at the start of the ISR. */
    xHigherPriorityTaskWoken = pdFALSE;
    
    xQueueSendFromISR(controllerQueue, (void *) &interruptData, &xHigherPriorityTaskWoken);
    
    /* Now the buffer is empty we can switch context if necessary. */
    if( xHigherPriorityTaskWoken )
    {
        /* Actual macro used here is port specific.*/
        portYIELD_FROM_ISR (xHigherPriorityTaskWoken);
    }
    
}

static void configure_extint_channel(void)
{
    struct extint_chan_conf config_extint_chan;
    extint_chan_get_config_defaults(&config_extint_chan);
    
    //configure INT0 Button_1
    config_extint_chan.gpio_pin = INT_BUTTON1_PIN;
    config_extint_chan.gpio_pin_mux = INT_BUTTON1_MUX;
    config_extint_chan.gpio_pin_pull = EXTINT_PULL_UP;
    config_extint_chan.detection_criteria = EXTINT_DETECT_BOTH;
    extint_chan_set_config(INT_BUTTON1_EIC_LINE, &config_extint_chan);
    
    //configure INT15 Button_2
    config_extint_chan.gpio_pin = INT_BUTTON2_PIN;
    config_extint_chan.gpio_pin_mux = INT_BUTTON2_MUX;
    config_extint_chan.gpio_pin_pull = EXTINT_PULL_UP;
    config_extint_chan.detection_criteria = EXTINT_DETECT_BOTH;
    extint_chan_set_config(INT_BUTTON2_EIC_LINE, &config_extint_chan);
    
    
    //configure INT06 I_O Extender Int
    config_extint_chan.gpio_pin = SME_INT_IOEXT_PIN;
    config_extint_chan.gpio_pin_mux = SME_INT_IOEXT_MUX;
    config_extint_chan.gpio_pin_pull = EXTINT_PULL_UP;
    config_extint_chan.detection_criteria = EXTINT_DETECT_BOTH;
    extint_chan_set_config(SME_INT_IOEXT_EIC_LINE, &config_extint_chan);
}

static void configure_extint_callbacks(void)
{
    //register INT0 Button_1
    extint_register_callback(extint0_detection_callback, INT_BUTTON1_EIC_LINE, EXTINT_CALLBACK_TYPE_DETECT);
    extint_chan_enable_callback(INT_BUTTON1_EIC_LINE,	EXTINT_CALLBACK_TYPE_DETECT);
    
    //register INT15 Button_2
    extint_register_callback(extint15_detection_callback, INT_BUTTON2_EIC_LINE, EXTINT_CALLBACK_TYPE_DETECT);
    extint_chan_enable_callback(INT_BUTTON2_EIC_LINE,	EXTINT_CALLBACK_TYPE_DETECT);
    
    //register INT06 I_O Extension start disabled
    extint_register_callback(extint06_detection_callback, SME_INT_IOEXT_EIC_LINE, EXTINT_CALLBACK_TYPE_DETECT);
    extint_chan_disable_callback(SME_INT_IOEXT_EIC_LINE,	EXTINT_CALLBACK_TYPE_DETECT);
}

void sme_init_isr_global(void) {
    configure_extint_channel();
    configure_extint_callbacks();
    system_interrupt_enable_global();
}

void clearInt(eventE interrupt) {
    switch( interrupt) {
        case button1Int:
        intDetect &= ~0x1;
        break;

        case button2Int:
        intDetect &= ~0x2;
        break;

        default:
        case externalInt:
        case nfcInt:
        case bteInt:
        break;
    }
}