
/*
 * nfc.c
 *
 * Created: 4/1/2015 11:24:09 PM
 *  Author: mfontane
 */ 

#include <stdint-gcc.h>
#include <stdbool.h>
#include "ndef\rtd\rtdText.h"
#include "ndef\rtd\rtdUri.h"
#include "ndef\ndef.h"
#include "nfc.h"

bool storeUrihttp(RecordPosEnu position, uint8_t *http){

    NDEFDataStr data;


    prepareUrihttp(&data, position, http);
    return   NT3HwriteRecord( &data );
}



bool storeText(RecordPosEnu position, uint8_t *text){
    NDEFDataStr data;


    prepareText(&data, position, text);
    return NT3HwriteRecord( &data );
}
