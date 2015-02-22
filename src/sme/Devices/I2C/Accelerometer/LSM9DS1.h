/*
 * LSM9DS1.h
 *
 * Created: 05/01/2015 19:49:15
 *  Author: mfontane
 */ 


#ifndef LSM9DS1_H_
#define LSM9DS1_H_

#include <stdbool.h>
#include <stdint-gcc.h>



bool LSM9DS1setOnlyAcc(char mode);
bool LSM9DS1PowwrOff(void);

bool LSM9DS1_A_Activate(void);
bool LSM9DS1_A_Deactivate(void);
bool LSM9DS1_A_Init(void);
bool LSM9DS1_A_getValues(uint16_t *buffer);
bool LSM9DS1_A_Decode(uint16_t *buffer, uint16_t *data1, uint16_t *data2, uint16_t *data3);

bool LSM9DS1_M_Activate(void);
bool LSM9DS1_M_Deactivate(void);
bool LSM9DS1_M_Init(void);
bool LSM9DS1_M_getValues(uint16_t *buffer);
bool LSM9DS1_M_Decode(uint16_t *buffer, uint16_t *data1, uint16_t *data2, uint16_t *data3);

bool LSM9DS1_G_Activate(void);
bool LSM9DS1_G_Deactivate(void);
bool LSM9DS1_G_Init(void);
bool LSM9DS1_G_getValues(uint16_t *buffer);
bool LSM9DS1_G_Decode(uint16_t *buffer, uint16_t *data1, uint16_t *data2, uint16_t *data3);

#endif /* LSM9DS1_H_ */