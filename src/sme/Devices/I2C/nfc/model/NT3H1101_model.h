#include <stdint-gcc.h>
/*
 * NT3H1101_model.h
 *
 * Created: 3/8/2015 11:37:26 AM
 *  Author: mfontane
 */ 


#ifndef NT3H1101_MODEL_H_
#define NT3H1101_MODEL_H_


#define NFC_PAGE_SIZE 16

extern  uint8_t  nfcPageBuffer[NFC_PAGE_SIZE];


typedef union manufactoringDataUnion{
    struct {
        uint8_t  slaveAddr;
        uint8_t  serialNumber[6];
        uint8_t  internalData[3];
        uint16_t lockBytes;
        uint32_t capabilityContainer;
    } manufS;
    uint8_t  pagemanufr[16];
} manufactoringDataU;

extern manufactoringDataU manuf;

#endif /* NT3H1101_MODEL_H_ */