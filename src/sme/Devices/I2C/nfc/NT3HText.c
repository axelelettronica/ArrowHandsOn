/*
 * NT3HText.c
 *
 *  Created on: Mar 26, 2015
 *      Author: smkk
 */


#include "NT3HText.h"
#include "NT3H.h"
#include <string.h>
#include "nfcSpecification/nfcForum.h"

int firstTextRecord(uint8_t *param, uint8_t textSize) {

    NDEFRecordStr record;
    NDEFHeaderStr header;
    bool ret=true;

    memset(&record,0,sizeof(NDEFRecordStr));
    // this is the first record
    header.startByte = NDEF_START_BYTE;
    composeNDEFMBME(true, true, &record);

    // prepare the NDEF Header and payload
    uint8_t recordLength = composeNDEFText(textSize, &record);
    header.payloadLength = textSize + recordLength;

    // write first record
    memcpy(nfcPageBuffer, &header, sizeof(NDEFHeaderStr));
    memcpy(&nfcPageBuffer[sizeof(NDEFHeaderStr)], &record, recordLength);

    return sizeof(NDEFHeaderStr)+recordLength;

}

int addTextRecord(uint8_t *param, uint8_t textSize) {
    NDEFRecordStr record;
    NDEFHeaderStr header;
    uint8_t       newRecordPtr, mbMe;
    bool          ret = true;

    // first Change the Header of the first Record
    NT3HReadHeaderNfc(&newRecordPtr, &mbMe);
    record.header = mbMe;
    composeNDEFMBME(true, false, &record); // this is the lastrecord
    mbMe = record.header;

    memset(&record,0,sizeof(NDEFRecordStr));

    // prepare second record
    uint8_t recordLength = composeNDEFText(textSize, &record);
    composeNDEFMBME(false, true, &record); // this is the last record
    header.payloadLength += textSize + recordLength;

    // save the new value of length on the first page
    NT3HWriteHeaderNfc((newRecordPtr+header.payloadLength), mbMe);


    // load the last valid page and start to add the new record
    UncompletePageStr *lastPage = ((UncompletePageStr*)param);
    NT3HReadUserData(lastPage->page);
    memcpy(&nfcPageBuffer[lastPage->extra], &record, recordLength);
    return recordLength;
}

