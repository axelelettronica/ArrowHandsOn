/*
 * IncFile1.h
 *
 * Created: 02/01/2015 20:50:30
 *  Author: mfontane
 */ 


#ifndef LPS25H_H_
#define LPS25H_H_

#include <stdbool.h>

bool LPS25Hnit(void);
bool LPS25HgetCalibration(void);
bool LPS25HgetValues(char *buffer);



#endif /* INCFILE1_H_ */