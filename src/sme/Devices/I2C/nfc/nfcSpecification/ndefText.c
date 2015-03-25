/*
 * ndefText.c
 *
 *  Created on: Mar 24, 2015
 *      Author: smkk
 */

#include "ndefText.h"



uint8_t addNDEFTextRecord(NDEFTextTypeStr *typeStr) {

    //	return addNDEFTextPayload(bodyLength, ndefRecord);
    typeStr->status=0x2;
    typeStr->language[0]='e';
    typeStr->language[1]='n';

    return 3;
}
