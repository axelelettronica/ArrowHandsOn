/*
 * sme_gps_timer.h
 *
 * Created: 1/31/2015 3:57:42 PM
 *  Author: speirano
 */ 


#ifndef SME_GPS_TIMER_H_
#define SME_GPS_TIMER_H_

#define GPS_1_SEC_TIMER     (1000 / portTICK_RATE_MS)
#define GPS_60_SEC_TIMER    (60*GPS_1_SEC_TIMER) 
#define GPS_SCAN_TIMEOUT    (1*GPS_60_SEC_TIMER) // 1 min
#define GPS_5_MIN_TIMER     (5*GPS_60_SEC_TIMER) // 5 min

#define GPS_TIMEOUT_ID        1

bool isGpsCommandTimerExpired(void);

void initGpsTimer(void);
void stopGpsCommandTimer(void);
void startGpsCommandTimer(void);

#endif /* SME_GPS_TIMER_H_ */