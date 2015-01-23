/*
* sme_sigfox_usart.c
*
* Created: 10/01/2015 22:40:43
*  Author: mfontane
*/
#include <asf.h>
#include "sme_sigfox_usart.h"
#include "..\..\..\sme_FreeRTOS.h"

#include "sme\tasks\sme_sigfox_task.h"
#include "sme\model\sme_model_sigfox.h"

/** \name Embedded debugger CDC Gateway USART interface definitions
* @{
*/
#define SIGFOX_MODULE              SERCOM5
#define SIGFOX_SERCOM_MUX_SETTING  USART_RX_1_TX_0_XCK_1
#define SIGFOX_SERCOM_PINMUX_PAD0  PINMUX_PB16C_SERCOM5_PAD0
#define SIGFOX_SERCOM_PINMUX_PAD1  PINMUX_PB17C_SERCOM5_PAD1
#define SIGFOX_SERCOM_PINMUX_PAD2  PINMUX_UNUSED
#define SIGFOX_SERCOM_PINMUX_PAD3  PINMUX_UNUSED
#define SIGFOX_SERCOM_DMAC_ID_TX   SERCOM5_DMAC_ID_TX
#define SIGFOX_SERCOM_DMAC_ID_RX   SERCOM5_DMAC_ID_RX
#define SIGFOX_BAUDRATE			   115200
/** @} */

/* interrupt USART variables */
static struct usart_module usart_sigfox;

volatile uint8_t rx_buffer[MAX_RX_BUFFER_LENGTH];
/* interrupt USART variables */

static void usart_sigfox_read_callback(const struct usart_module *const usart_module)
{
	
	BaseType_t xHigherPriorityTaskWoken; 
	xSemaphoreGiveFromISR(sigfox_sem, &xHigherPriorityTaskWoken );
	
	    /* If xHigherPriorityTaskWoken was set to true you
    we should yield.  The actual macro used here is 
    port specific. */
    portYIELD_FROM_ISR( xHigherPriorityTaskWoken );		
}

static void usart_sigfox_write_callback(const struct usart_module *const usart_module)
{

	port_pin_toggle_output_level(LED_0_PIN);

}

void sigFoxInit(void) {
	// sigfox usart configuration
	struct usart_config config_usart;

	usart_get_config_defaults(&config_usart);
	//! [setup_sigfox_config]
	config_usart.baudrate    = SIGFOX_BAUDRATE;
	config_usart.mux_setting = SIGFOX_SERCOM_MUX_SETTING;
	config_usart.pinmux_pad0 = SIGFOX_SERCOM_PINMUX_PAD0;
	config_usart.pinmux_pad1 = SIGFOX_SERCOM_PINMUX_PAD1;
	config_usart.pinmux_pad2 = SIGFOX_SERCOM_PINMUX_PAD2;
	config_usart.pinmux_pad3 = SIGFOX_SERCOM_PINMUX_PAD3;

	while (usart_init(&usart_sigfox, SIGFOX_MODULE, &config_usart) != STATUS_OK) {
	}

	usart_enable(&usart_sigfox);
	
	usart_register_callback(&usart_sigfox,
	usart_sigfox_write_callback, USART_CALLBACK_BUFFER_TRANSMITTED);
	usart_register_callback(&usart_sigfox,
	usart_sigfox_read_callback, USART_CALLBACK_BUFFER_RECEIVED);

	usart_enable_callback(&usart_sigfox, USART_CALLBACK_BUFFER_TRANSMITTED);
	usart_enable_callback(&usart_sigfox, USART_CALLBACK_BUFFER_RECEIVED);
	// endsigfox usart configuration
}

status_code_genare_t sigfoxSendMessage(uint8_t *msg, uint8_t len) {
	memset(rx_buffer,0,MAX_RX_BUFFER_LENGTH);
	status_code_genare_t err =  usart_write_buffer_job(&usart_sigfox, msg, len);

    // sigfox message send w or w/ success, is it possible to release now the semaphore
    // and let other message take the huge memory
    xSemaphoreGive(sf_sem);
    return err;
}

status_code_genare_t sigfoxReceivedMessage(uint8_t *msg, uint8_t len ){ 
	status_code_genare_t ret =  usart_read_buffer_job(&usart_sigfox, (uint8_t *)rx_buffer, MAX_RX_BUFFER_LENGTH);
	if (ret == STATUS_OK) {
		memcpy(msg, rx_buffer, MAX_RX_BUFFER_LENGTH);
	}
	return ret;
}