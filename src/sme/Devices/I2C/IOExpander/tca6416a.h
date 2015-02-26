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
bool TCA6416a_init(void);
bool TCA6416a_input_port1_values(char *buffer);
bool TCA6416a_input_port0_values(char *buffer);
bool TCA6416a_input_ports_values(uint16_t *buffer);
bool TCA6416a_reset_devices(void);

void TCA6416a_gps_force_on(void);


#endif /* TCA6416A_H_ */