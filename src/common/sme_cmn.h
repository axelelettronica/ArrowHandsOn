/*
 * common.h
 *
 * Created: 30/12/2014 23:06:17
 *  Author: speirano
 */ 

#ifndef COMMON_H_
#define COMMON_H_

#include <asf.h>
#include <stdio_serial.h>

//! \name Task configuration
//@{

#define CDC_TASK_PRIORITY      (tskIDLE_PRIORITY + 3)
#define CDC_TASK_DELAY         (10 / portTICK_RATE_MS)

#define UART_TASK_PRIORITY      (tskIDLE_PRIORITY + 3)
#define UART_TASK_DELAY         (51 / portTICK_RATE_MS)

#define CONTROL_TASK_PRIORITY      (tskIDLE_PRIORITY + 2)
#define CONTROL_TASK_DELAY         (100 / portTICK_RATE_MS)

#define I2C_TASK_PRIORITY     (tskIDLE_PRIORITY + 1)
#define I2C_TASK_DELAY        (50 / portTICK_RATE_MS)


//@}

/**
 * \defgroup freertos_sam0_demo_tasks_group FreeRTOS demo tasks
 *
 * The demo tasks demonstrate basic use of FreeRTOS, with inter-task
 * communication using queues and mutexes.
 *
 * For details on how the demo works, see \ref appdoc_intro.
 *
 * For detailed information on the tasks, see:
 * - \ref main_task()
 * - \ref graph_task()
 * - \ref terminal_task()
 * - \ref about_task()
 * - \ref uart_task()
 *
 * The demo tasks depend on the following drivers:
 * - \ref oled1_xpro_io_group
 * - \ref edbg_cdc_rx_group
 * - \ref asfdoc_common2_gfx_mono
 *
 * @{
 */
#define dbg(__fmt_)              
//#define dbg(__fmt_)             printf(__fmt_)
#define dbg_info(__fmt_, ...)   printf(__fmt_, __VA_ARGS__)
#define dbg_error(_x, ...)      printf(_x, __VA_ARGS__)
#define dbg_putchar(c)          putc(c, stdout)
#define dbg_vprintf_pgm(...)    vfprintf(stdout, __VA_ARGS__)


extern bool volatile tickless_enable;

int sme_cdc_mgr_init(void);
int sme_uart_mgr_init(void);
int sme_i2c_mgr_init(void);
int sme_ctrl_init(void);

/** @} */


//void terminal_task(void *params);


#endif /* COMMON_H_ */