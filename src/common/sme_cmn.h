/*
 * common.h
 *
 * Created: 30/12/2014 23:06:17
 *  Author: speirano
 */ 

#ifndef COMMON_H_
#define COMMON_H_

#include <asf.h>
#include <stdio/stdio_serial/stdio_serial.h>
#include "FreeRTOS.h"
#include "task.h"
#include "sme_cdc_io.h"

/* Mapping error codes */
#define SME_OK           0
#define SME_EPERM        1  /* Operation not permitted */
#define SME_EIO          5  /* I/O error */
#define SME_ENXIO        6  /* No such device or address */
#define SME_ENOMEM      12  /* Out of memory */
#define SME_SFX_OK      13  /* SigFox answer OK */
#define SME_SFX_KO      14  /* SigFox answer KO */

#define SME_EBUSY       16  /* Device or resource busy */
#define SME_ENODEV      19  /* No such device */
#define SME_EINVAL      22  /* Invalid argument */



//! \name Task configuration
#define CDC_TASK_PRIORITY      (tskIDLE_PRIORITY + 3)
#define CDC_TASK_DELAY         (10 / portTICK_RATE_MS)

#define UART_TASK_PRIORITY      (tskIDLE_PRIORITY + 3)
#define UART_TASK_DELAY         (51 / portTICK_RATE_MS)

#define CONTROL_TASK_PRIORITY      (tskIDLE_PRIORITY + 2)
#define CONTROL_TASK_DELAY         (300000 / portTICK_RATE_MS) // 5 Minuts


extern bool volatile tickless_enable;

int sme_cdc_mgr_init(void);
int sme_uart_mgr_init(void);

#endif /* COMMON_H_ */