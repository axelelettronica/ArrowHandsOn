/*
 * ndefTypes.h
 *
 *  Created on: Mar 24, 2015
 *      Author: smkk
 */

#ifndef NDEFTYPES_H_
#define NDEFTYPES_H_

#include "ndefText.h"
#include "ndefUri.h"


#define NDEF_TEXT 'T'
#define NDEF_URI 'U'

typedef union {
	NDEFTextTypeStr text;
	NDEFUriTypeStr uri;
} NDEFTypeUnion;

typedef struct {
	uint8_t typeCode;
	NDEFTypeUnion typePayload;
}NDEFTypeStr;

#endif /* NDEFTYPES_H_ */
