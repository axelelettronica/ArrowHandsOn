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
#ifdef NT3H1201
#define USER_END_REG   0x77 // for th 2K
#else
#define USER_END_REG   0x38 // just the first 8 bytes for th 1K
#endif

#define SRAM_START_REG 0xF8
#define SRAM_END_REG   0xFB // just the first 8 bytes
#define CONFIG_REG	   0x3A
#define SESSION_REG	   0xFE


bool readManufactoringData(void);
void getNxpSerialNumber(char* buffer);

/*
 * read the user data from the requested page
 * first page is 0
 *
 * the NT3H1201 has 119 PAges 
 * the NT3H1101 has 56 PAges (but the 56th page has only 8 Bytes)
*/
bool nfc_read_user_data(uint8_t page);

/*
 * Write data information from the starting requested page.
 * If the dataLen is bigger of NFC_PAGE_SIZE, the consecuiteve needed 
 * pages will be automatically used.
 * 
 * The functions stops to the latest available page.
 * 
 first page is 0
 * the NT3H1201 has 119 PAges 
 * the NT3H1101 has 56 PAges (but the 56th page has only 8 Bytes)
*/
bool nfc_write_user_data(uint8_t page, const uint8_t* data, uint16_t dataLen);
bool getNxpUserData(char* buffer);
bool getConfiguration(void);
bool readSRAM(void);
bool getSessionReg(void);
bool nxpInit(void);

const uint8_t* get_last_ncf_page(void);

#endif /* NFC_H_ */