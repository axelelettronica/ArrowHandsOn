/*
 * privateZXYAxis.h
 *
 * Created: 28/12/2014 21:28:37
 *  Author: mfontane
 */ 


#ifndef PRIVATEZXYAXIS_H_
#define PRIVATEZXYAXIS_H_

#include <asf.h>

#define GSCALE 8  // Sets full-scale range to +/-2, 4, or 8g. Used to calc real g values.

uint8_t connected;



uint8_t MMA8452Standby(void);
uint8_t MMA8452Active(void);

#endif /* PRIVATEZXYAXIS_H_ */