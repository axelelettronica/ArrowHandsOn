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
#include "status_codes.h"
#include "sme_sfx_timer.h"
#include "sme_sigfox_rx_fsm.h"
#include "..\..\IO\sme_rgb_led.h"

/* interrupt USART variables */
static struct usart_module usart_sigfox;
volatile uint8_t rx_buffer[MAX_SIGFOX_RX_BUFFER_LENGTH];
/* interrupt USART variables */


// keep track of the messageId are active
uint8_t sfxMessageIdx[MAX_MESSAGE_OUT];



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
    sme_led_red_brightness(HALF_LIGTH);
}

volatile int init;

void sigFoxInit(void) {

    initSigFoxModel();
    initSfxTimer();

    // sigfox usart configuration
    struct usart_config config_usart;

    usart_get_config_defaults(&config_usart);
    //! [setup_sigfox_config]
    config_usart.baudrate    = SME_SIGFOX_BAUDRATE;
    config_usart.mux_setting = SME_SIGFOX_SERCOM_MUX_SETTING;
    config_usart.pinmux_pad0 = SME_SIGFOX_SERCOM_PINMUX_PAD0;
    config_usart.pinmux_pad1 = SME_SIGFOX_SERCOM_PINMUX_PAD1;
    config_usart.pinmux_pad2 = SME_SIGFOX_SERCOM_PINMUX_PAD2;
    config_usart.pinmux_pad3 = SME_SIGFOX_SERCOM_PINMUX_PAD3;

    do
    {
        init = usart_init(&usart_sigfox, SME_SIGFOX_MODULE, &config_usart);
    } while (init != STATUS_OK);


    usart_enable(&usart_sigfox);
    
    usart_register_callback(&usart_sigfox,
    usart_sigfox_write_callback, USART_CALLBACK_BUFFER_TRANSMITTED);
    usart_register_callback(&usart_sigfox,
    usart_sigfox_read_callback, USART_CALLBACK_BUFFER_RECEIVED);

    usart_enable_callback(&usart_sigfox, USART_CALLBACK_BUFFER_TRANSMITTED);
    usart_enable_callback(&usart_sigfox, USART_CALLBACK_BUFFER_RECEIVED);
    // endsigfox usart configuration
}

status_code_genare_t sigfoxSendMessage(const uint8_t *msg, uint8_t len) {

    // reset the receiver fsm to a start state
    resetRxFsm();

    memset((char *)rx_buffer,0,MAX_SIGFOX_RX_BUFFER_LENGTH);
    status_code_genare_t err =  usart_write_buffer_job(&usart_sigfox, (char *)msg, len);

    // sigfox message send w or w/ success, is it possible to release now the semaphore
    // and let other message take the huge memory
    releaseSigFoxModel();
    return err;
}

status_code_genare_t sigfoxReceivedMessage(uint8_t *msg, uint8_t len ){
    status_code_genare_t ret =  usart_read_buffer_job(&usart_sigfox, (uint8_t *)rx_buffer, MAX_SIGFOX_RX_BUFFER_LENGTH);
    if (ret == STATUS_OK) {
        memcpy((char *)msg, (char *)rx_buffer, MAX_SIGFOX_RX_BUFFER_LENGTH);
    }
    return ret;
}