/*
 * nxpNfc.c
 *
 * Created: 29/12/2014 21:42:02
 *  Author: smkk
 */
#include <string.h>
#include "../I2C.h"
#include "NT3H.h"
#include "ndef/ndef.h"
#include "nfc.h"
#include "ndef/rtd/nfcForum.h"



// due to the nature of the NT3H a timeout is required to
// protectd 2 consecutive I2C access
#define NFC_ACCESS_TIMEOUT 5



inline const uint8_t* get_last_ncf_page(void) {
    return nfcPageBuffer;
}


static bool writeTimeout( const uint8_t *data, uint8_t dataSend) {
    bool received = false;
    uint8_t timeOut=0;
    do {
        received = writeBufferRegister(NXPNFC_ADDRESS, data, dataSend);
        if (received == true) {
            goto end;
        }
        timeOut++;
    }while(!(timeOut == NFC_ACCESS_TIMEOUT));

    // timeout expired set the error
    received = false;

    end:
    return received;
}

static bool readTimeout(uint8_t nfcRegister, uint8_t *buffer) {
    bool received = false;
    uint8_t timeOut=0;
    do {
        received = readBufferRegister(NXPNFC_ADDRESS, nfcRegister, buffer, NFC_PAGE_SIZE);
        if (received == true) {
            goto end;
        }
        timeOut++;
    }while(!(timeOut == NFC_ACCESS_TIMEOUT));

    // timeout expired set the error
    received = false;

    end:
    return received;
}

bool NT3HReadHeaderNfc(uint8_t *endRecordsPtr, uint8_t *ndefHeader) {
    *endRecordsPtr=0;
    bool ret = NT3HReadUserData(0);

    // read the first page to see where is the end of the Records.
    if (ret == true) {
        // if the first byte is equals to NDEF_START_BYTE there are some records
        // store theend of that
        if ((NDEF_START_BYTE == nfcPageBuffer[0]) && (NTAG_ERASED != nfcPageBuffer[2])) {
            *endRecordsPtr = nfcPageBuffer[1];
            *ndefHeader    = nfcPageBuffer[2];
        }
        return true;
    } else {
        errNo = NT3HERROR_READ_HEADER;
    }

    return ret;
}


bool NT3HWriteHeaderNfc(uint8_t endRecordsPtr, uint8_t ndefHeader) {

    // read the first page to see where is the end of the Records.
    bool ret = NT3HReadUserData(0);
    if (ret == true) {

        nfcPageBuffer[1] = endRecordsPtr;
        nfcPageBuffer[2] = ndefHeader;
        ret = NT3HWriteUserData(0, nfcPageBuffer);
        if (ret == false) {
            errNo = NT3HERROR_WRITE_HEADER;
        }
    } else {
        errNo = NT3HERROR_READ_HEADER;
    }

    return ret;
}

bool NT3HInit(/*configS *configuration*/){
    manufS manuf;

    //not really needed, use for debug the I2C bus and
    // return false in case of problem
    bool ret = NT3HReaddManufactoringData(&manuf);

    return ret;

}

bool NT3HEraseAllTag(void) {
    bool ret = true;
    uint8_t erase[NFC_PAGE_SIZE+1] = {USER_START_REG, 0x03, 0x03, 0xD0, 0x00, 0x00, 0xFE};
    ret = writeTimeout(erase, sizeof(erase));

    if (ret == false) {
        errNo = NT3HERROR_ERASE_USER_MEMORY_PAGE;
    }
    return ret;
}

bool NT3HReaddManufactoringData(manufS *manuf) {
    return readTimeout(MANUFACTORING_DATA_REG, manuf);
}

bool NT3HReadConfiguration(configS *configuration){
    return readTimeout(CONFIG_REG, configuration);
}

bool getSessionReg(void) {
    return readTimeout(SESSION_REG, nfcPageBuffer);
}


bool NT3HReadUserData(uint8_t page) {
    uint8_t reg = USER_START_REG+page;
    // if the requested page is out of the register exit with error
    if (reg > USER_END_REG) {
        errNo = NT3HERROR_INVALID_USER_MEMORY_PAGE;
        return false;
    }

    bool ret = readTimeout(reg, nfcPageBuffer);

    if (ret == false) {
        errNo = NT3HERROR_READ_USER_MEMORY_PAGE;
    }

    return ret;
}


bool NT3HWriteUserData(uint8_t page, const uint8_t* data) {
    bool ret = true;
    uint8_t dataSend[NFC_PAGE_SIZE +1]; // data plus register
    uint8_t reg = USER_START_REG+page;

    // if the requested page is out of the register exit with error
    if (reg > USER_END_REG) {
        errNo = NT3HERROR_INVALID_USER_MEMORY_PAGE;
        ret = false;
        goto end;
    }

    dataSend[0] = reg; // store the register
    memcpy(&dataSend[1], data, NFC_PAGE_SIZE);
    ret = writeTimeout(dataSend, sizeof(dataSend));
    if (ret == false) {
        errNo = NT3HERROR_WRITE_USER_MEMORY_PAGE;
        goto end;
    }

    end:
    return ret;
}


bool NT3HReadSram(void){
    bool ret=false;
    for (int i = SRAM_START_REG, j=0; i<=SRAM_END_REG; i++,j++) {
        ret = readTimeout(i, nfcPageBuffer);
        if (ret==false) {
            return ret;
        }
        //memcpy(&userData[offset], pageBuffer, sizeof(pageBuffer));
    }
    return ret;
}


void NT3HGetNxpSerialNumber(char* buffer) {
    manufS manuf;

    if (NT3HReaddManufactoringData(&manuf)) {
        for(int i=0; i<6; i++) {
            buffer[i] = manuf.serialNumber[i];
        }
    }
}

