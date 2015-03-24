/*
 * ndefText.c
 *
 *  Created on: Mar 24, 2015
 *      Author: smkk
 */

#include "ndefText.h"


//uint8_t addNDEFTextPayload(uint8_t bodyLength, NDEFRecordStr *ndefRecord) {
	//Status byte: This is UTF-8, and has a two-byte language code
//		ndefRecord->type.typePayload.text.status=0x2;
//		ndefRecord->type.typePayload.text.language[0]='e';
//		ndefRecord->type.typePayload.text.language[1]='n';
//
//		ndefRecord->payloadLength = bodyLength+3; // added the typePayload
//		return sizeof(ndefRecord->header) +
//				sizeof(ndefRecord->typeLength) +
//				sizeof(ndefRecord->payloadLength) +
//				sizeof(ndefRecord->type.typePayload.text) +
//				sizeof(ndefRecord->type.typeCode);
//}
