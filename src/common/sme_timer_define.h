/*
 * sme_timer_define.h
 *
 * Created: 2/14/2015 9:39:03 PM
 *  Author: mfontane
 */ 


#ifndef SME_TIMER_DEFINE_H_
#define SME_TIMER_DEFINE_H_

#include "../sme/sme_FreeRTOS.h"

#define ONE_SECOND  (1000 / portTICK_RATE_MS)   // one second
#define TWO_SECOND  (2000 / portTICK_RATE_MS)   // one second
#define FIFTEEN_SECOND  (15000 / portTICK_RATE_MS)  //one Mminute Timeout



#define HALF_MINUTE  (30000 / portTICK_RATE_MS)  //one Mminute Timeout
#define ONE_MINUTE  (60000 / portTICK_RATE_MS)  //one Mminute Timeout
#define FIVE_MINUTE (300000 / portTICK_RATE_MS) // 5 Minuts


#endif /* SME_TIMER_DEFINE_H_ */