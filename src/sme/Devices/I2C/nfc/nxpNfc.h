/*
 * nfc.h
 *
 * Created: 29/12/2014 21:41:20
 *  Author: mfontane
 */ 


#ifndef NFC_H_
#define NFC_H_

#define NXPNFC_ADDRESS 85  // 0x4 is the default for every NXP io ho visto 85

#define MANUFACTORING_DATA_REG 0x0

#define USER_START_REG 0x1
#define USER_END_REG   0x38 // just the first 8 bytes
#define USER_DATA_LEN 888

#define SRAM_START_REG 0xF8
#define SRAM_END_REG   0xFB // just the first 8 bytes




bool readManufactoringData(void);
void getNxpSerialNumber(char* buffer);
bool readUserData(void);
bool getNxpUserData(char* buffer);

bool readSRAM(void);

#endif /* NFC_H_ */