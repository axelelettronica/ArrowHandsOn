/*
 * tca6416a.h
 *
 * Created: 1/21/2015 8:46:47 PM
 *  Author: mfontane
 */ 

/* 
 * IOExtender documentation
 * http://www.ti.com/lit/ds/symlink/tca6416a.pdf
 */
#ifndef TCA6416A_H_
#define TCA6416A_H_


#include <stdbool.h>
#include <stdint-gcc.h>

#define PORT0 0 
#define PORT1 1
bool TCA6416aInit(void);
bool TCA6416aPort1Values(char *buffer);
bool TCA6416aPort0Values(char *buffer);
bool TCA6416aPortsValues(uint16_t *buffer);



#endif /* TCA6416A_H_ */