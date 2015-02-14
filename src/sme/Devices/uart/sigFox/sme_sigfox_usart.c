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

/** \name Embedded debugger CDC Gateway USART interface definitions
* @{
*/
#define SIGFOX_SERCOM_MUX_SETTING  USART_RX_1_TX_0_XCK_1
#define SIGFOX_MODULE              SERCOM4
#define SIGFOX_SERCOM_PINMUX_PAD0  PINMUX_PB12C_SERCOM4_PAD0
#define SIGFOX_SERCOM_PINMUX_PAD1  PINMUX_PB13C_SERCOM4_PAD1
#define SIGFOX_SERCOM_PINMUX_PAD2  PINMUX_PB14C_SERCOM4_PAD2
#define SIGFOX_SERCOM_PINMUX_PAD3  PINMUX_UNUSED
#define SIGFOX_SERCOM_DMAC_ID_TX   SERCOM4_DMAC_ID_TX
#define SIGFOX_SERCOM_DMAC_ID_RX   SERCOM4_DMAC_ID_RX

#define SIGFOX_BAUDRATE			   19200

#define VALID_RED_LEVEL   (0xFFFF / 2)
#define VALID_BLUE_LEVEL  (0xFFFF / 16)

/** @} */

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
    sme_brigthness_led_red(VALID_RED_LEVEL);
     sme_brigthness_led_blue(VALID_BLUE_LEVEL);
}

volatile int init;

void sigFoxInit(void) {

    initSigFoxModel();
    initSfxTimer();

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

    do
    {
        init = usart_init(&usart_sigfox, SIGFOX_MODULE, &config_usart);
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
        port_pin_set_output_level(LED_0_PIN, LED_0_INACTIVE); // clear the led
        sme_led_red_off();
        sme_led_blue_off();
        memcpy((char *)msg, (char *)rx_buffer, MAX_SIGFOX_RX_BUFFER_LENGTH);
    }
    return ret;
}