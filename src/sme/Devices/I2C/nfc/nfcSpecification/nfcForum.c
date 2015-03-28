/*
 * nfcForum.c
 *
 * Created: 3/23/2015 10:30:07 PM
 *  Author: smkk
 */ 

#include "nfcForum.h"
#include <string.h>

uint8_t composeNDEFText(uint8_t bodyLength, NDEFRecordStr *ndefRecord) {
    ndefRecord->header |= 1;
    ndefRecord->header |= BIT_SR;

    ndefRecord->typeLength =1;


    ndefRecord->type.typeCode=NDEF_TEXT;

    uint8_t payLoadLen = addNDEFTextRecord(&ndefRecord->type.typePayload.text);

    ndefRecord->payloadLength = bodyLength+payLoadLen; // added the typePayload

    return sizeof(ndefRecord->header) +
            sizeof(ndefRecord->typeLength) +
            sizeof(ndefRecord->payloadLength) +
            sizeof(ndefRecord->type.typePayload.text) +
            sizeof(ndefRecord->type.typeCode);
}

void composeNDEFMBME(bool isFirstRecord, bool isLastRecord, NDEFRecordStr *ndefRecord) {
    if (isFirstRecord)
        ndefRecord->header |= BIT_MB;
    else
        ndefRecord->header &= ~MASK_MB;

    if (isLastRecord)
        ndefRecord->header |= BIT_ME;
    else
        ndefRecord->header &= ~MASK_ME;
}
