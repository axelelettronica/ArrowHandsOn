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
bool LPS25HActivate(void);
bool LPS25HDeactivate(void);
bool LPS25HgetValues(char *buffer);
bool LPS25HBDUDeactivate(void);
bool LPS25HBDUActivate(void);



#endif /* INCFILE1_H_ */