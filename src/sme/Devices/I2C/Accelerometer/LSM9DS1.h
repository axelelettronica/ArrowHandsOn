/*
 * LSM9DS1.h
 *
 * Created: 05/01/2015 19:49:15
 *  Author: mfontane
 */ 


#ifndef LSM9DS1_H_
#define LSM9DS1_H_

#include <stdbool.h>

bool LSM9DS1nit(void);
bool LSM9DS1setOnlyAcc(char mode);
bool LSM9DS1PowwrOff(void);
bool LSM9DS1getValues(char *buffer);






#endif /* LSM9DS1_H_ */