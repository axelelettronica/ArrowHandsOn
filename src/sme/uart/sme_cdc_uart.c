/*
 * cdc.c
 *
 * Created: 31/12/2014 15:22:21
 *  Author: speirano
 */ 

/**
 * \defgroup edbg_cdc_rx_group Embedded Debugger (EDBG) Virtual COM Port driver
 *
 * Simple driver for receiving from the EDBG Virtual COM Port via SERCOM USART.
 *
 * This makes use of the \ref asfdoc_sam0_sercom_usart_group, but configures a
 * custom interrupt handler for the SERCOM instead of using the driver's own
 * handler.
 * The purpose of this driver is to avoid the overhead of the SERCOM USART
 * driver's callback functionality, allowing the received data to be handled
 * directly for increased throughput / reduced overhead.
 *
 * Both USART receiver and transmitter are enabled, but only the receive
 * interrupt is enabled since the intended use of the transmitter is to echo
 * back the received characters.
 *
 * The custom interrupt handler can, e.g., be a stripped down version of the
 * SERCOM USART callback driver's interrupt handler
 * (\ref _usart_interrupt_handler()).
 *
 * @{
 */

#include <sme_cmn.h>
#include "sme_cdc_uart.h"

//! Queue for incoming terminal characters
static xQueueHandle cdc_msg_in_queue;
	

//! Semaphore to signal busy terminal buffer
static xSemaphoreHandle cdc_msg_mutex;
	
//! Interrupt handler for reception from EDBG Virtual COM Port
static void cdc_rx_handler(uint8_t instance);

//! Instance for \ref edbg_cdc_rx_group
static struct usart_module cdc_usart;


sme_cli_msg_t sme_cli_msg = {};

static void cdc_task(void *params);

/*
 * cdc_rx_init to be commented out in a second time  (TX is not working yet
 */
/**
 * \brief Initialize USART for reception from EDBG Virtual COM Port
 *
 * This function initializes the specified SERCOM USART driver instance for use
 * with the Embedded Debugger (EDBG) Virtual COM Port. Both transmitter and
 * receiver are enabled, but only the receive interrupt is enabled.
 *
 * A custom handler function for the receive interrupt must must be supplied.
 * See the SERCOM USART callback driver's handler for an implementation
 * template: \ref _usart_interrupt_handler().
 *
 * \param usart Address of USART instance to use.
 * \param cdc_rx_handler Address of receive interrupt handler.
 */
static inline void cdc_rx_init(struct usart_module *const usart,
		sercom_handler_t cdc_rx_handler)
{
	struct usart_config config_struct;
	uint8_t instance_index;

	usart_get_config_defaults(&config_struct);

	config_struct.mux_setting = EDBG_CDC_SERCOM_MUX_SETTING;
	config_struct.pinmux_pad0 = EDBG_CDC_SERCOM_PINMUX_PAD0;
	config_struct.pinmux_pad1 = EDBG_CDC_SERCOM_PINMUX_PAD1;
	config_struct.pinmux_pad2 = EDBG_CDC_SERCOM_PINMUX_PAD2;
	config_struct.pinmux_pad3 = EDBG_CDC_SERCOM_PINMUX_PAD3;
	config_struct.baudrate    = 38400;

	while (usart_init(usart, EDBG_CDC_MODULE, &config_struct) != STATUS_OK) {
		// Intentionally left empty
	}

	// Inject our own interrupt handler
	instance_index = _sercom_get_sercom_inst_index(EDBG_CDC_MODULE);
	_sercom_set_handler(instance_index, cdc_rx_handler);
	
	// Enable the UART transceiver
	usart_enable(usart);
	usart_enable_transceiver(usart, USART_TRANSCEIVER_TX);
	usart_enable_transceiver(usart, USART_TRANSCEIVER_RX);

	// ..and the RX Complete interrupt
	((SercomUsart *)EDBG_CDC_MODULE)->INTENSET.reg = SERCOM_USART_INTFLAG_RXC;
		// ..and the RX Complete interrupt

}



/**
 * \internal
 * \brief UART interrupt handler for reception on EDBG CDC UART
 *
 * This function is based on the interrupt handler of the SERCOM USART callback
 * driver (\ref _usart_interrupt_handler()). It has been modified to only handle
 * the receive interrupt and to push the received data directly into the queue
 * for terminal characters (\ref terminal_in_queue), and echo the character back
 * to the sender.
 *
 * \param instance Instance number of SERCOM that generated interrupt.
 */
static void cdc_rx_handler(uint8_t instance)
{
	SercomUsart *const usart_hw = (SercomUsart *)EDBG_CDC_MODULE;
	uint16_t interrupt_status;
	uint16_t data;
	uint8_t error_code;

	// Wait for synch to complete
#if defined(FEATURE_SERCOM_SYNCBUSY_SCHEME_VERSION_1)
	while (usart_hw->STATUS.reg & SERCOM_USART_STATUS_SYNCBUSY) {
	}
#elif defined(FEATURE_SERCOM_SYNCBUSY_SCHEME_VERSION_2)
	while (usart_hw->SYNCBUSY.reg) {
	}
#endif

	// Read and mask interrupt flag register
	interrupt_status = usart_hw->INTFLAG.reg;

	if (interrupt_status & SERCOM_USART_INTFLAG_RXC) {
		// Check for errors
		error_code = (uint8_t)(usart_hw->STATUS.reg & SERCOM_USART_STATUS_MASK);
		if (error_code) {
			// Only frame error and buffer overflow should be possible
			if (error_code &
					(SERCOM_USART_STATUS_FERR | SERCOM_USART_STATUS_BUFOVF)) {
				usart_hw->STATUS.reg =
						SERCOM_USART_STATUS_FERR | SERCOM_USART_STATUS_BUFOVF;
			} else {
				// Error: unknown failure
			}
		// All is fine, so push the received character into our queue
		} else {
			data = (usart_hw->DATA.reg & SERCOM_USART_DATA_MASK);

			if (!xQueueSendFromISR(cdc_msg_in_queue, (uint8_t *)&data,
						NULL)) {
				// Error: could not enqueue character
			} else {
				// Echo back! Data reg. should empty fast since this is the
				// only place anything is sent.
				while (!(interrupt_status & SERCOM_USART_INTFLAG_DRE)) {
					interrupt_status = usart_hw->INTFLAG.reg;
				}
				usart_hw->DATA.reg = (uint8_t)data;
			}
		}
    }else {
		// Error: only RX interrupt should be enabled
	}
}


/**
 * \brief Initialize the USART for unit test
 *
 * Initializes the SERCOM USART used for sending the unit test status to the
 * computer via the EDBG CDC gateway.
 *
static void cdc_uart_init(struct usart_module *const cdc_uart_module)
{
	struct usart_config usart_conf;

	// Configure USART for unit test output
	usart_get_config_defaults(&usart_conf);
	usart_conf.mux_setting = EDBG_CDC_SERCOM_MUX_SETTING;
	usart_conf.pinmux_pad0 = EDBG_CDC_SERCOM_PINMUX_PAD0;
	usart_conf.pinmux_pad1 = EDBG_CDC_SERCOM_PINMUX_PAD1;
	usart_conf.pinmux_pad2 = EDBG_CDC_SERCOM_PINMUX_PAD2;
	usart_conf.pinmux_pad3 = EDBG_CDC_SERCOM_PINMUX_PAD3;
	usart_conf.baudrate    = 38400;

	stdio_serial_init(cdc_uart_module, EDBG_CDC_MODULE, &usart_conf);
	usart_enable(cdc_uart_module);
}
*/

int sme_cdc_mgr_init (void)
{

    /* Configure SERCOM USART for reception from EDBG Virtual COM Port */
	
    cdc_rx_init(&cdc_usart, &cdc_rx_handler);	
    
	/*cdc_uart_init(&cdc_usart);  */

   cdc_msg_mutex = xSemaphoreCreateMutex();
   cdc_msg_in_queue = xQueueCreate(64, sizeof(uint8_t));

	xTaskCreate(cdc_task,
	(const char *) "CDC",
	configMINIMAL_STACK_SIZE,
	NULL,
	CDC_TASK_PRIORITY,
	NULL);
	
	return 0;
}



/**
 * \brief UART task
 *
 * This task manages the Board's console.
 * This is used for debugging purposes only.
 *
 * \param params Parameters for the task. (Not used.)
 */
static void cdc_task(void *params)
{
	uint8_t *current_token_ptr = 0;
	uint8_t *current_char_ptr = 0;
	uint8_t current_column = 0;

	for (;;) {

		// Grab terminal mutex
		xSemaphoreTake(cdc_msg_mutex, portMAX_DELAY);

		current_token_ptr = sme_cli_msg.token[sme_cli_msg.token_idx];
		current_char_ptr = current_token_ptr + current_column;

		while (xQueueReceive(cdc_msg_in_queue, current_char_ptr, 0)) {

			/* Newline-handling is difficult because all terminal emulators
			 * seem to do it their own way. The method below seems to work
			 * with Putty and Realterm out of the box.
			 */
			switch (*current_char_ptr) {
			case ' ':
			
			    if(sme_cli_msg.token[sme_cli_msg.token_idx][0] == ' ') {
					break;
				}
				// Replace \r with \0 and move head to next line
				*current_char_ptr = '\0';

				current_column = 0;
				sme_cli_msg.token_idx = (sme_cli_msg.token_idx + 1)
						% MAX_TOKEN_NUM;
				current_token_ptr = sme_cli_msg.token[sme_cli_msg.token_idx];
				current_char_ptr = current_token_ptr + current_column;
				break;
				
			case '\r':
			   	// Replace \r with \0 and move head to next line
			   	*current_char_ptr = '\0';

			   	current_column = 0;
			   	sme_cli_msg.token_idx = (sme_cli_msg.token_idx + 1)
			   				% MAX_TOKEN_NUM;
			   	current_token_ptr = sme_cli_msg.token[sme_cli_msg.token_idx];
			   	current_char_ptr = current_token_ptr + current_column;
				
				sme_cdc_cmd_parse(sme_cli_msg.token_idx);
				
				// clean
				//sme_clean_msg_buffer();
				sme_cli_msg.token_idx = 0;
	            current_column = 0;
				
			    break;
			
			case '\n':
				// For \n, do nothing -- it is replaced with \0 later
				break;
			
			case '\b':  // backspace
			    // Replace \b with \0 and move head to next line
		        *current_char_ptr = '\0';

			    if (current_column) {
				    --current_column;
			    }
		
			    break;
			
			default:
				// For all other characters, just move head to next char
				current_column++;
				if (current_column >= (MAX_TOKEN_LEN-1)) {
					current_column = 0;
					sme_cli_msg.token_idx = (sme_cli_msg.token_idx + 1)
							% MAX_TOKEN_NUM;
					current_token_ptr = sme_cli_msg.token[sme_cli_msg.token_idx];
				}
				current_char_ptr = current_token_ptr + current_column;
				//dbg_info("\r\nreceived %c \r\n", *current_char_ptr);
			}

			// Set zero-terminator at head
			*current_char_ptr = '\0';
		}

		xSemaphoreGive(cdc_msg_mutex);

		vTaskDelay(UART_TASK_DELAY);
	}
}