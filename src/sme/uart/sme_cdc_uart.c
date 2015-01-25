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
#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"
#include "task.h"

//! Queue for incoming terminal characters
static xQueueHandle cdc_msg_in_queue;
	
//! Semaphore to signal busy terminal buffer
static xSemaphoreHandle cdc_msg_mutex;



sme_cli_msg_t sme_cli_msg = {};


uint8_t dbg_buf[100] = {};
bool sme_dbg_enable = true;
bool sme_err_enable = true;

static void cdc_task_rx(void *params);
static void cdc_task(void *params);
void configure_usart(struct usart_module *const cdc_uart_module);


/* Function inizializing CDC UART
 *
 */
void configure_usart(struct usart_module *const cdc_uart_module)
{
	struct usart_config config_usart;

	usart_get_config_defaults(&config_usart);

	config_usart.mux_setting = EDBG_CDC_SERCOM_MUX_SETTING;
	config_usart.pinmux_pad0 = EDBG_CDC_SERCOM_PINMUX_PAD0;
	config_usart.pinmux_pad1 = EDBG_CDC_SERCOM_PINMUX_PAD1;
	config_usart.pinmux_pad2 = EDBG_CDC_SERCOM_PINMUX_PAD2;
	config_usart.pinmux_pad3 = EDBG_CDC_SERCOM_PINMUX_PAD3;
	config_usart.baudrate    = EDBG_CDC_SIGFOX_BAUDRATE;

	while (usart_init(cdc_uart_module,
           EDBG_CDC_MODULE, &config_usart) != STATUS_OK) {
	}

	usart_enable(cdc_uart_module);
}


int sme_cdc_mgr_init (void)
{

    /* Configure SERCOM USART for reception from EDBG Virtual COM Port */	
   configure_usart(&cdc_usart);

   cdc_msg_mutex = xSemaphoreCreateMutex();
   cdc_msg_in_queue = xQueueCreate(64, sizeof(uint8_t));


	xTaskCreate(cdc_task, (const char *) "CDC", configMINIMAL_STACK_SIZE,
                NULL, CDC_TASK_PRIORITY, NULL);
	
    xTaskCreate(cdc_task_rx, (const char *) "CDC_RX", configMINIMAL_STACK_SIZE,
    	        NULL, CDC_TASK_PRIORITY, NULL);

	return 0;
}

/*
 * Thread Enabling Console reading
 */
static void cdc_task_rx(void *params)
{
	uint16_t temp;

	while (true) {
		//! [main_read]
        vTaskDelay(UART_TASK_DELAY);
		if (usart_read_wait(&cdc_usart, &temp) == STATUS_OK) {
			xQueueSendFromISR(cdc_msg_in_queue, (uint8_t *)&temp, NULL);
			while (usart_write_wait(&cdc_usart, temp) != STATUS_OK) {
			}
		}
	}
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