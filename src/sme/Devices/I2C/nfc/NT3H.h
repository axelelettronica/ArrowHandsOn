/*
 * nfc.h
 *
 * Created: 29/12/2014 21:41:20
 *  Author: smkk
 */ 


#ifndef NT3H_H_
#define NT3H_H_

#include "model\NT3H1101_model.h"
#include "stdbool.h"

#define NXPNFC_ADDRESS 85  // 0x4 is the default for every NXP io ho visto 85

#define MANUFACTORING_DATA_REG 0x0
#define USER_START_REG 0x1


#ifdef NT3H1201             // for th 2K
#define USER_END_REG   0x77 
#define CONFIG_REG	   0x7A
#elif  NT3H1101                     // for th 1K
#define USER_END_REG   0x38 // just the first 8 bytes for th 1K
#define CONFIG_REG	   0x3A
#else
#error NO NFC MODEL DEFINITION 
#endif

#define SRAM_START_REG 0xF8
#define SRAM_END_REG   0xFB // just the first 8 bytes

#define SESSION_REG	   0xFE


typedef enum {
    NDEFFirstPos,
    NDEFMiddlePos,
    NDEFLastPos
} RecordPosEnu;
/*
 * This strucure is used in the ADD record functionality
 * to store the last nfc page information, in order to continue from that point.
 */
typedef struct {
    uint8_t page;
    uint8_t usedBytes;
} UncompletePageStr;



typedef struct {
    RecordPosEnu ndefPosition;
    uint8_t rtdType;
    uint8_t *rtdPayload;
    uint8_t rtdPayloadlength;
    void    *specificRtdData;
}NDEFDataStr;

bool NT3HReadManufactoringData(manufS *manuf);
void NT3HGetNxpSerialNumber(char* buffer);

/*
 * read the user data from the requested page
 * first page is 0
 *
 * the NT3H1201 has 119 PAges 
 * the NT3H1101 has 56 PAges (but the 56th page has only 8 Bytes)
*/
bool NT3HReadUserData(uint8_t page);

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
bool NT3HWriteUserData(uint8_t page, const uint8_t* data);

/*
 * The function read the first page of user data where is stored the NFC Header.
 * It is important because it contains the total size of all the stored records.
 *
 * param endRecordsPtr return the value of the total size excluding the NDEF_END_BYTE
 * param ndefHeader    Store the NDEF Header of the first record
 */
bool NT3HReadHeaderNfc(uint8_t *endRecordsPtr, uint8_t *ndefHeader);

/*
 * The function write the first page of user data where is stored the NFC Header.
 * update the bytes that contains the payload Length and the first NDEF Header
 *
 * param endRecordsPtr The value of the total size excluding the NDEF_END_BYTE
 * param ndefHeader    The NDEF Header of the first record
 */
bool NT3HWriteHeaderNfc(uint8_t endRecordsPtr, uint8_t ndefHeader);

bool getSessionReg(void);
bool getNxpUserData(char* buffer);
bool NT3HReadSram(void);
bool NT3HReadSession(void);
bool NT3HInit(void);
bool NT3HReadConfiguration(configS *configuration);

bool NT3HEraseAllTag(void);

bool NT3HReaddManufactoringData(manufS *manuf);

bool NT3HResetUserData(void);

#endif /* NFC_H_ */
