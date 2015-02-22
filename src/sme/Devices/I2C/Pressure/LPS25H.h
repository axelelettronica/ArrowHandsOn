/*
 * IncFile1.h
 *
 * Created: 02/01/2015 20:50:30
 *  Author: mfontane
 */ 


#ifndef LPS25H_H_
#define LPS25H_H_

#include <stdbool.h>
#include <stdint-gcc.h>

bool LPS25Hnit(void);
bool LPS25HActivate(void);
bool LPS25HDeactivate(void);
bool LPS25HgetValues(uint16_t *buffer);
bool LPS25HBDUDeactivate(void);
bool LPS25HBDUActivate(void);
bool LPS25HDecode(uint16_t *buffer, uint16_t *data1, uint16_t *data2);


#endif /* INCFILE1_H_ */