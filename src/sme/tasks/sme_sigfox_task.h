/*
 * sme_sigfox_controller.h
 *
 * Created: 10/01/2015 17:21:20
 *  Author: smkk
 */ 
#ifndef SME_SIGFOX_TASK_H_
#define SME_SIGFOX_TASK_H_

#include <stdint-gcc.h>
#include "../sme_FreeRTOS.h"

#define SME_SIGFOX_MAX_DATA_LEN  20
#define SIGFOX_TASK_NAME (const char *)"SIGFOX"


//! semaphore for incoming execution command

extern xSemaphoreHandle sigfox_sem;


BaseType_t sme_sigfox_rx_mgr_init(void);


#endif /* SME_SIGFOX_CONTROLLER_H_ */
