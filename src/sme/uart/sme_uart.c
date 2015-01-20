/*
 * uart.c
 *
 * Created: 30/12/2014 23:05:17
 *  Author: speirano
 */ 
/* \section appdoc_sam0_usart_unit_test_setup Setup
 * The following connections has to be made using wires:
 * - SAM D20/D21 Xplained Pro board
 *  - \b TX/RX: EXT1 PIN17 (PA04) <--> EXT1 PIN13 (PB09)
 * - SAM R21 Xplained Pro board
 *  - \b TX/RX: EXT1 PIN9  (PA22) <--> EXT1 PIN15 (PB03)
 * To run the test:
 *  - Connect the SAM Xplained Pro board to the computer using a
 *    micro USB cable.
 *  - Open the virtual COM port in a terminal application.
 *    \note The USB composite firmware running on the Embedded Debugger (EDBG)
 *          will enumerate as debugger, virtual COM port and EDBG data
 *          gateway.
 *  - Build the project, program the target and run the application.
 *    The terminal shows the results of the unit test.
 *
 * \section appdoc_sam0_usart_unit_test_usage Usage
 *  - The unit tests are carried out with one SERCOM on EXT1 as the USART
 *    transmitter and another SERCOM on EXT1 as the SERCOM USART receiver.
 *  - Data is transmitted from transmitter to receiver in lengths of a single
 *    byte as well as multiple bytes.
 *
 * \section appdoc_sam0_usart_unit_test_compinfo Compilation Info
 * This software was written for the GNU GCC and IAR for ARM.
 * Other compilers may or may not work.
 *
 * \section appdoc_sam0_usart_unit_test_contactinfo Contact Information
 * For further information, visit
 * <a href="http://www.atmel.com">http://www.atmel.com</a>.
 */
#include <sme_cmn.h>

#include <stdio_serial.h>
#include <string.h>
//#include "conf_test.h"

#include "sme_uart.h"

/* Test string to send */
#define TEST_STRING        "Hello world!"
/* Length of test string */
#define TEST_STRING_LEN    13

/* Speed to test USART at */
#define SME_USART_SPEED   115200

/* Structure for UART module connected to EDBG (used for unit test output) */
struct usart_module cdc_uart_module;

struct usart_module usart_rx_module;
struct usart_config usart_rx_config;
struct usart_module usart_tx_module;
struct usart_config usart_tx_config;

volatile bool transfer_complete;

static void uart_task(void *params);

/**
 * \internal
 * \brief USART interrupt callback function
 *
 * Called by USART driver when receiving is complete.
 *
 * * \param module USART module causing the interrupt (not used)
 */
static void usart_callback(const struct usart_module *const module)
{
	transfer_complete = true;
}


/**
 * \internal
 * \brief Test single 8-bit character send and receive.
 *
 * This test sends on 9-bit character and checks it's received properly
 * on the other end.
 *
 * \param test Current test case.
 */
static void run_transfer_single_8bit_char_test(const struct test_case *test)
{
	uint16_t tx_char = 0x53;
	volatile uint16_t rx_char = 0;

	/* Write and read the data */
	usart_write_wait(&usart_tx_module, tx_char);
	usart_read_wait(&usart_rx_module, (uint16_t*)&rx_char);

	/* Make sure we received what we sent */
	//test_assert_true(test, tx_char==rx_char,
	//		"Failed receiving sent byte. TX=%d, RX=%d", tx_char, rx_char);

}

/**
 * \internal
 * \brief Test single 9-bit character send and receive.
 *
 * This test sends one 9-bit character and checks it's received properly
 * on the other end.
 *
 * \param test Current test case.
 */
static void run_transfer_single_9bit_char_test(const struct test_case *test)
{
	uint16_t tx_char = 0x166;
	uint16_t rx_char;

	/* Re-configure RX USART to operate with 9 bit character size */
	usart_disable(&usart_rx_module);
	usart_rx_config.character_size = USART_CHARACTER_SIZE_9BIT;
	usart_init(&usart_rx_module, CONF_RX_USART,	&usart_rx_config);
	usart_enable(&usart_rx_module);

	/* Re-configure TX USART to operate with 9 bit character size */
	usart_disable(&usart_tx_module);
	usart_tx_config.character_size = USART_CHARACTER_SIZE_9BIT;
	usart_init(&usart_tx_module, CONF_TX_USART,	&usart_tx_config);
	usart_enable(&usart_tx_module);

	/* Write and read the data */
	usart_write_wait(&usart_tx_module, tx_char);
	usart_read_wait(&usart_rx_module, &rx_char);

	/* Make sure we received what we sent */
	//test_assert_true(test, tx_char==rx_char,
	//		"Failed receiving sent byte. TX=%d, RX=%d", tx_char, rx_char);

	/* Put RX USART back in 8 bit mode */
	usart_disable(&usart_rx_module);
	usart_rx_config.character_size = USART_CHARACTER_SIZE_8BIT;
	usart_init(&usart_rx_module, CONF_RX_USART,	&usart_rx_config);
	usart_enable(&usart_rx_module);

	/* Put TX USART back in 8 bit mode */
	usart_disable(&usart_tx_module);
	usart_tx_config.character_size = USART_CHARACTER_SIZE_8BIT;
	usart_init(&usart_tx_module, CONF_TX_USART,	&usart_tx_config);
	usart_enable(&usart_tx_module);
}


/**
 * \internal
 * \brief Test sending (blocking) and receiving (interrupt) a string.
 *
 * This test sends one string from one USART to another.
 * It's sent using a blocking write, while it's received using interrupts.
 *
 * \param test Current test case.
 */
static void run_buffer_write_blocking_read_interrupt_test(const struct test_case *test)
{
	uint8_t tx_string[TEST_STRING_LEN] = TEST_STRING;
	volatile uint8_t rx_string[TEST_STRING_LEN] = "";
	int16_t result;

	/* We will read back the buffer using interrupt */
	usart_register_callback(&usart_rx_module, usart_callback,
			USART_CALLBACK_BUFFER_RECEIVED);
	usart_enable_callback(&usart_rx_module, USART_CALLBACK_BUFFER_RECEIVED);

	/* Start receiving */
	transfer_complete = false;
	usart_read_buffer_job(&usart_rx_module, (uint8_t*)rx_string,
			TEST_STRING_LEN);

	/* Send the string */
	usart_write_buffer_wait(&usart_tx_module, tx_string,
			TEST_STRING_LEN );

	uint16_t timeout_cycles = 0xFFFF;

	/* Wait until reception completes */
	do {
		timeout_cycles--;
		if (transfer_complete) {
			break;
		}
	} while (timeout_cycles != 0);

	//test_assert_true(test, timeout_cycles > 0,
	//		"Timeout in reception");

	usart_disable_callback(&usart_rx_module, USART_CALLBACK_BUFFER_RECEIVED);
	usart_unregister_callback(&usart_rx_module, USART_CALLBACK_BUFFER_RECEIVED);

	/* Compare strings */
	result = strcmp((char*)tx_string, (char*)rx_string);

	/* Make sure we 0-terminate the received string in case it's
		gibberish and we have to print it */
	rx_string[TEST_STRING_LEN-1] = 0;

	//test_assert_false(test, result,
	//		"Failed receiving string. TX='%s', RX='%s'", tx_string, rx_string);
}

/**
 * \internal
 * \brief Test sending and receiving a string using interrupts.
 *
 * This test sends one string from one USART to another.
 * Both the send and receive is done using interrupts.
 *
 * \param test Current test case.
 */
static void run_buffer_read_write_interrupt_test(const struct test_case *test)
{
	volatile uint8_t tx_string[TEST_STRING_LEN] = TEST_STRING;
	volatile uint8_t rx_string[TEST_STRING_LEN] = "";
	int16_t result;

	usart_register_callback(&usart_rx_module, usart_callback,
			USART_CALLBACK_BUFFER_RECEIVED);
	usart_enable_callback(&usart_rx_module, USART_CALLBACK_BUFFER_RECEIVED);

	/* start send */
	transfer_complete = false;

	usart_write_buffer_job(&usart_tx_module, (uint8_t*)tx_string,
			TEST_STRING_LEN);
	usart_read_buffer_job(&usart_rx_module, (uint8_t*)rx_string,
			TEST_STRING_LEN);

	uint16_t timeout_cycles = 0xFFFF;

	/* Wait until reception completes */
	do {
		timeout_cycles--;
		if (transfer_complete) {
			break;
		}
	} while (timeout_cycles != 0);

	//test_assert_true(test, timeout_cycles > 0,
	//		"Timeout in send/receive");

	/* Compare strings */
	result = strcmp((char*)tx_string, (char*)rx_string);

	/* Make sure we 0-terminate the received string in case it's
		gibberish and we have to print it */
	rx_string[TEST_STRING_LEN-1] = 0;

	//test_assert_false(test, result,
	//		"Failed receiving string. TX='%s', RX='%s'", tx_string, rx_string);
}




/**
 * \brief Initialize USARTs for unit tests
 *
 * Initializes the two USARTs used by the unit test (one for RX and one for TX).
 *
 */
static void sme_uart_init(void)
{  /* - SAM D20/D21 Xplained Pro board
	*  - \b TX/RX: EXT1 PIN17 (PA04) <--> EXT1 PIN13 (PB09)
	* - SAM R21 Xplained Pro board
	*  - \b TX/RX: EXT1 PIN9  (PA22) <--> EXT1 PIN15 (PB03)
	*/

	/* Configure RX USART */
	usart_get_config_defaults(&usart_rx_config);
	usart_rx_config.mux_setting = CONF_RX_USART_SERCOM_MUX;
	usart_rx_config.pinmux_pad0 = CONF_RX_USART_PINMUX_PAD0;
	usart_rx_config.pinmux_pad1 = CONF_RX_USART_PINMUX_PAD1;
	usart_rx_config.pinmux_pad2 = CONF_RX_USART_PINMUX_PAD2;
	usart_rx_config.pinmux_pad3 = CONF_RX_USART_PINMUX_PAD3;
	usart_rx_config.baudrate    = SME_USART_SPEED;
	/* Apply configuration */
	usart_init(&usart_rx_module, CONF_RX_USART, &usart_rx_config);
	/* Enable USART */
	usart_enable(&usart_rx_module);

	/* Configure TX USART */
	usart_get_config_defaults(&usart_tx_config);
	usart_tx_config.mux_setting = CONF_TX_USART_SERCOM_MUX;
	usart_tx_config.pinmux_pad0 = CONF_TX_USART_PINMUX_PAD0;
	usart_tx_config.pinmux_pad1 = CONF_TX_USART_PINMUX_PAD1;
	usart_tx_config.pinmux_pad2 = CONF_TX_USART_PINMUX_PAD2;
	usart_tx_config.pinmux_pad3 = CONF_TX_USART_PINMUX_PAD3;
	usart_tx_config.baudrate    = SME_USART_SPEED;
	/* Apply configuration */
	usart_init(&usart_tx_module, CONF_TX_USART, &usart_tx_config);
	/* Enable USART */
	usart_enable(&usart_tx_module);
}


int sme_uart_mgr_init(void)
{

    sme_uart_init();


	xTaskCreate(uart_task,
	(const char *) "UART",
	configMINIMAL_STACK_SIZE,
	NULL,
	UART_TASK_PRIORITY,
	NULL);
	
	return 0;
}


/**
 * \brief uart mng task
 *
 *
 * \param params Parameters for the task. (Not used.)
 */
static void uart_task(void *params)
{
	//xTaskHandle temp_task_handle = NULL;

	for(;;) {
			// We can now safely suspend the previously resumed task
			//if (temp_task_handle) {
			//	vTaskSuspend(temp_task_handle);
			//	temp_task_handle = NULL;
			//}


			//xSemaphoreGive(display_mutex);

			// If a task handle was specified, resume it now
			//if (temp_task_handle) {
			//	vTaskResume(temp_task_handle);
			//}
		vTaskDelay(UART_TASK_DELAY);
	}
}

