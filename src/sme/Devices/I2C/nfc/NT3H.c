/*
 * nxpNfc.c
 *
 * Created: 29/12/2014 21:42:02
 *  Author: smkk
 */
#include <string.h>
#include "..\I2C.h"
#include "NT3H.h"
#include "nfcSpecification\nfcForum.h"
#include "NT3HText.h"



inline const uint8_t* get_last_ncf_page(void) {
    return nfcPageBuffer;
}

#define DEMO_TEXT  "Mangia "
#define DEMO_TEXT2 "TUTTA la pizza col pomodoro"

typedef int (*addFunct_T) (uint8_t *text, uint8_t textSize);


// due to the nature of the NT3H a timeout is required to
// protectd 2 consecutive I2C access
#define NFC_ACCESS_TIMEOUT 5

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
    uint8_t recordLength,mbMe;

    bool    ret=true;
    ret = NT3HReadHeaderNfc(&recordLength, &mbMe);


    if (ret) {
        ret =  NT3HwriteRecord(NDEF_TEXT, FIRST, DEMO_TEXT, sizeof(DEMO_TEXT)-1);
        ret &= NT3HwriteRecord(NDEF_TEXT, ADD,  DEMO_TEXT2, sizeof(DEMO_TEXT2)-1);
    }

    return ret;

}

bool NT3HEraseTag(void) {
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

static addFunct_T addFunct[2][2] = {
                                  {firstTextRecord, addTextRecord},
                                  {0,0}
                                 };


bool NT3HwriteRecord(uint8_t type, uint8_t add, uint8_t *text, uint8_t textSize) {

    uint8_t addedPayload;
    uint8_t recordLength=0, mbMe;
    bool ret = true;
    UncompletePageStr addPage;
    addPage.page = 0;

    uint8_t typeFunct=0;

    switch (type){
    case NDEF_TEXT:
        typeFunct =0;
        break;

    case NDEF_URI:
        typeFunct = 1;
        break;

    default:
        errNo = NT3HERROR_TYPE_NOT_SUPPORTED;
        ret = false;
        goto end;
        break;
    }


    // calculate the last used page
    if (add != FIRST ) {
        NT3HReadHeaderNfc(&recordLength, &mbMe);
        addPage.page  = (recordLength+sizeof(NDEFHeaderStr)+1)/NFC_PAGE_SIZE;

        //remove the NDEF_END_BYTE byte because it will overwrite by the new Record
        addPage.extra = (recordLength+sizeof(NDEFHeaderStr)+1)%NFC_PAGE_SIZE - 1;
    }


    // call the appropriate function and consider the pointer
    // within the NFC_PAGE_SIZE that need to be used
    uint8_t payloadPtr = addFunct[typeFunct][add](&addPage, textSize);

    uint8_t finish=payloadPtr+textSize;
    bool endRecord = false;
    uint8_t copyByte;

    // if the header is less then the NFC_PAGE_SIZE, fill it with the payload
    if (NFC_PAGE_SIZE>payloadPtr) {
        if (textSize > NFC_PAGE_SIZE-payloadPtr)
            copyByte = NFC_PAGE_SIZE-payloadPtr;
        else
            copyByte = textSize;

        memcpy(&nfcPageBuffer[payloadPtr], text, copyByte);
        addedPayload = copyByte;
    }

    //if it is sufficient one send add the NDEF_END_BYTE
    if ((addedPayload >= textSize)&&((payloadPtr+copyByte) < NFC_PAGE_SIZE)) {
        nfcPageBuffer[(payloadPtr+copyByte)] = NDEF_END_BYTE;
        endRecord = true;
    }

    ret = NT3HWriteUserData(addPage.page, nfcPageBuffer);

    while (!endRecord) {
        addPage.page++; // move to a new register
        memset(nfcPageBuffer,0,NFC_PAGE_SIZE);

        //special case just the NDEF_END_BYTE remain out
        if (addedPayload == textSize) {
            nfcPageBuffer[0] = NDEF_END_BYTE;
            ret = NT3HWriteUserData(addPage.page, nfcPageBuffer);
            endRecord = true;
            if (ret == false) {
                errNo = NT3HERROR_WRITE_NDEF_TEXT;
            }
            goto end;
        }

        if (addedPayload < textSize) {

            // add the NDEF_END_BYTE if there is enough space
            if ((textSize-addedPayload) < NFC_PAGE_SIZE){
                memcpy(nfcPageBuffer, &text[addedPayload], (textSize-addedPayload));
                nfcPageBuffer[(textSize-addedPayload)] = NDEF_END_BYTE;
            } else {
                memcpy(nfcPageBuffer, &text[addedPayload], NFC_PAGE_SIZE);
            }

            addedPayload += NFC_PAGE_SIZE;
            ret = NT3HWriteUserData(addPage.page, nfcPageBuffer);


            if (ret == false) {
                errNo = NT3HERROR_WRITE_NDEF_TEXT;
                goto end;
            }
        } else {
            endRecord = true;
        }

    }

    end:
    return ret;
}
