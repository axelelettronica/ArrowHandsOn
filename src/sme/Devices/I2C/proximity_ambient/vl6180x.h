/*
 * vl6180x.h
 *
 * Created: 3/1/2015 10:32:19 PM
 *  Author: mfontane
 */ 


#ifndef VL6180X_H_
#define VL6180X_H_



bool vl6180x_init(void);
bool vl6180x_get_light(char *value);

#endif /* VL6180X_H_ */