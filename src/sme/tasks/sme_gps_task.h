/*
 * sme_gps_controller.h
 *
 * Created: 22/01/2015 17:21:20
 *  Author: speirano
 */ 
#ifndef SME_GPS_TASK_H_
#define SME_GPS_TASK_H_

//#include <stdint-gcc.h>
#include "../sme_FreeRTOS.h"

#define SME_GPS_MAX_DATA_LEN             1 //82
#define GPS_TASK_NAME (const char *)   "GPS"


//! semaphore for incoming execution command

extern xSemaphoreHandle gps_rx_sem;



BaseType_t sme_gps_mgr_init(void);


#endif /* SME_GPS_CONTROLLER_H_ */