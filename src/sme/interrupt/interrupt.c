#include "system_interrupt.h"
#include "asf.h"
#include "..\extint\extint.h"
#include "port.h"
#include "interrupt.h"
#include "..\tasks\sme_controller.h"
/*
* interrupt.c
*
* Created: 08/01/2015 21:30:08
*  Author: mfontane
*/

controllerQueueS queueData;

volatile bool pin_state;
volatile dbg=0;
static void extint15_detection_callback(void)
{
	pin_state = port_pin_get_input_level(BUTTON_0_PIN);
	port_pin_set_output_level(LED_0_PIN, pin_state);
	
	BaseType_t xHigherPriorityTaskWoken;
	BaseType_t  err;
	queueData.intE=nfcInt;
	queueData.intSensor.nfc.sensor=humidityValue;
	queueData.withGPS = 1;
	/* We have not woken a task at the start of the ISR. */
	xHigherPriorityTaskWoken = pdFALSE;
	
	if (xQueueSendFromISR(controllerQueue, (void *) &queueData, NULL/*&xHigherPriorityTaskWoken*/) != pdPASS) {
		dbg=pdFALSE;
	} else {
		dbg = pdPASS;
	}
	
	/* Now the buffer is empty we can switch context if necessary.
	if( xHigherPriorityTaskWoken )
	{
	/* Actual macro used here is port specific.
	portYIELD_FROM_ISR (xHigherPriorityTaskWoken);
	}*/
	}

	static void extint0_detection_callback(void)
	{
	pin_state = port_pin_get_input_level(BUTTON_0_PIN);
	port_pin_set_output_level(LED_0_PIN, false);
	
	BaseType_t  err;
	BaseType_t xHigherPriorityTaskWoken;

	queueData.intE=nfcInt;
	queueData.intSensor.nfc.sensor=pressureValue;
	queueData.withGPS = 1;
	/* We have not woken a task at the start of the ISR. */
	xHigherPriorityTaskWoken = pdFALSE;
	
	err = xQueueSendFromISR(controllerQueue, (void *) &queueData, NULL/*&xHigherPriorityTaskWoken*/);
	dbg=err;
	
	/* Now the buffer is empty we can switch context if necessary.
	if( xHigherPriorityTaskWoken )
	{
	/* Actual macro used here is port specific.
	portYIELD_FROM_ISR (xHigherPriorityTaskWoken);
	}*/
	}

	static void configure_extint_channel(void)
	{
	struct extint_chan_conf config_extint_chan;
	extint_chan_get_config_defaults(&config_extint_chan);
	
	//configure INT15
	config_extint_chan.gpio_pin = INT15_EIC_PIN;
	config_extint_chan.gpio_pin_mux = INT15_EIC_MUX;
	config_extint_chan.gpio_pin_pull = EXTINT_PULL_UP;
	config_extint_chan.detection_criteria = EXTINT_DETECT_BOTH;
	extint_chan_set_config(INT15_EIC_LINE, &config_extint_chan);
	
	//configure INT0
	config_extint_chan.gpio_pin = INT0_EIC_PIN;
	config_extint_chan.gpio_pin_mux = INT0_EIC_MUX;
	config_extint_chan.gpio_pin_pull = EXTINT_PULL_DOWN;
	config_extint_chan.detection_criteria = EXTINT_DETECT_BOTH;
	extint_chan_set_config(INT0_EIC_LINE, &config_extint_chan);
	}

	static void configure_extint_callbacks(void)
	{
	//register INT15
	extint_register_callback(extint15_detection_callback, INT15_EIC_LINE, EXTINT_CALLBACK_TYPE_DETECT);
	extint_chan_enable_callback(INT15_EIC_LINE,	EXTINT_CALLBACK_TYPE_DETECT);
	
	//register INT0
	extint_register_callback(extint0_detection_callback, INT0_EIC_LINE, EXTINT_CALLBACK_TYPE_DETECT);
	extint_chan_enable_callback(INT0_EIC_LINE,	EXTINT_CALLBACK_TYPE_DETECT);
	}

	void sme_init_isr_global(void) {
	configure_extint_channel();
	configure_extint_callbacks();
	system_interrupt_enable_global();
	}