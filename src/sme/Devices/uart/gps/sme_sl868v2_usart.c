/*
 * sme_sl868v2_usart.c
 *
 * Created: 21/01/2015 22:41:06
 *  Author: speirano
 */ 

#include <asf.h>
//#include "sme_config_usart.h"
#include "..\..\..\sme_FreeRTOS.h"
#include "sme_cmn.h"
#include "sme\tasks\sme_gps_task.h"
#include "sme\model\sme_model_sl868v2.h"
#include "./sme_sl868v2_usart.h"
#include "..\..\IO\sme_rgb_led.h"
#include "sme_gps_timer.h"



#define VALID_BLUE_LEVEL (0xFFFF / 8)


/* interrupt USART variables */
static struct usart_module usart_gps;

volatile uint8_t gps_rx_buffer[MAX_SL868V2_RX_BUFFER_LENGTH] = {};
/* interrupt USART variables */



volatile int init;
void uartInit(const struct usart_module *const module,
             struct usart_config *const config,
             usart_callback_t tx_callback_func,
             usart_callback_t rx_callback_func) 
{
	while (usart_init(module, SME_GPS_MODULE, config) != STATUS_OK) {
	}

	usart_enable(module);
}


void sl868v2Init(void) {
    
    initGpsTimer();

    // gps usart configuration
    struct usart_config config_usart;

    usart_get_config_defaults(&config_usart);
    //! [setup_gps_config]
    config_usart.baudrate    = SME_GPS_BAUDRATE;
    config_usart.mux_setting = SME_GPS_SERCOM_MUX_SETTING;
    config_usart.pinmux_pad0 = SME_GPS_SERCOM_PINMUX_PAD0;
    config_usart.pinmux_pad1 = SME_GPS_SERCOM_PINMUX_PAD1;
    config_usart.pinmux_pad2 = SME_GPS_SERCOM_PINMUX_PAD2;
    config_usart.pinmux_pad3 = SME_GPS_SERCOM_PINMUX_PAD3;

    //uartInit(&usart_gps, &config_usart, usart_gps_write_callback,
    //         usart_gps_read_callback);
	while (usart_init(&usart_gps, SME_GPS_MODULE, &config_usart) != STATUS_OK) {
	}

	usart_enable(&usart_gps);
}

int 
sl868v2SendMessage(const uint8_t *msg, uint8_t len) 
{
    print_gps_msg("> %s", msg);
	memset((char *)gps_rx_buffer,0,MAX_SL868V2_RX_BUFFER_LENGTH);
	status_code_genare_t err =  usart_write_buffer_job(&usart_gps, (uint8_t *)msg, len);

    // message sent w or w/ success, is it possible to release now the semaphore
    // and let other message take the huge memory
    releaseSl868v2Model();

    if (err == STATUS_OK) {
        return SME_OK;
    } else {
        return SME_EIO;
    }

}

int 
sl868v2ReceivedMessage(uint8_t *msg, uint8_t len )
{ 
    uint16_t temp;

   	if (usart_read_wait(&usart_gps, &temp) == STATUS_OK) {
        *msg = temp;
        return SME_OK;
    }
    return SME_EIO;
}