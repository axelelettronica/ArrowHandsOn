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

typedef union  configuratioinUnion{
    struct {
        uint8_t nc_reg;
        uint8_t last_ndef_block;
        uint8_t sram_mirror_block;
        uint8_t wdt_ls;
        uint8_t wdt_ms;
        uint8_t i2c_clock_str;
        uint8_t reg_clock;
        uint8_t unused;
    } configS;
    uint8_t page[NFC_PAGE_SIZE];
}config_u;

extern config_u  configuration;

typedef union manufactoringDataUnion{
    struct {
        uint8_t  slaveAddr;
        uint8_t  serialNumber[6];
        uint8_t  internalData[3];
        uint16_t lockBytes;
        uint32_t capabilityContainer;
    } manufS;
    uint8_t  page[NFC_PAGE_SIZE];
} manufactoring_u;

extern manufactoring_u manuf;

#endif /* NT3H1101_MODEL_H_ */