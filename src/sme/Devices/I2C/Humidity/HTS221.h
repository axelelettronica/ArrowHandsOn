/*
 * IncFile1.h
 *
 * Created: 02/01/2015 20:50:30
 *  Author: smkk
 */ 


#ifndef HTS221_H_
#define HTS221_H_

#include <stdbool.h>
#include <stdint-gcc.h>

bool HTS221nit(void);
bool HTS221getCalibration(void);
bool HTS221getValues(uint16_t *buffer);
bool HTS221Activate(void);
bool HTS221Deactivate(void);
bool HTS221BDUActivate(void);
bool HTS221BDUDeactivate(void);
bool HTS221Decode(uint16_t *buffer, uint16_t *data1, uint16_t *data2);


#endif /* INCFILE1_H_ */
