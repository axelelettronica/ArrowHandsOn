/*
 * ndefText.h
 *
 *  Created on: Mar 24, 2015
 *      Author: smkk
 */

#ifndef NDEFTEXT_H_
#define NDEFTEXT_H_

//#include "nfcForum.h"
#include <stdint-gcc.h>

#define BIT_STATUS (1<<7)
#define BIT_RFU	   (1<<6)


#define MASK_STATUS 0x80
#define MASK_RFU    0x40
#define MASK_IANA   0b00111111

typedef struct {
	uint8_t status;
	uint8_t language[2];
}NDEFTextTypeStr;


uint8_t addNDEFTextRecord(NDEFTextTypeStr *typeStr);
#endif /* NDEFTEXT_H_ */
