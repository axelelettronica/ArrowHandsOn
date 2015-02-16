#include "..\I2C.h"
#include "nxpNfc.h"
#include <string.h>
/*
* nxpNfc.c
*
* Created: 29/12/2014 21:42:02
*  Author: mfontane
*/

uint8_t  nfcPageBuffer[NFC_PAGE_SIZE];
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

static manufactoringDataU manuf;

inline uint8_t* getLastNfcPage(void) {
    return nfcPageBuffer;
}

bool readManufactoringData(void) {
    bool ret = false;
    
    ret = readBufferRegister(NXPNFC_ADDRESS, MANUFACTORING_DATA_REG, manuf.pagemanufr, sizeof(manuf.pagemanufr));
    return ret;
    
}

void getNxpSerialNumber(char* buffer) {
    for(int i=0; i<6; i++) {
        buffer[i] = manuf.manufS.serialNumber[i];
    }
}


bool nxpInit(void){
    bool ret=true;

    ret &= readManufactoringData();
    ret &= getConfiguration();
    //ret &= getSessionReg();
    return ret;
}

bool getConfiguration(void) {
    return readBufferRegister(NXPNFC_ADDRESS, CONFIG_REG, nfcPageBuffer, sizeof(nfcPageBuffer));
}

bool getSessionReg(void) {
    return readBufferRegister(NXPNFC_ADDRESS, SESSION_REG, nfcPageBuffer, sizeof(nfcPageBuffer));
}

bool readUserData(uint8_t page){
    uint8_t reg = USER_START_REG+page;
    // if the requested page is out of the register exit with error
    if (reg > USER_END_REG) {
        return false;
    }

    bool ret=false;

    ret = readBufferRegister(NXPNFC_ADDRESS, reg, nfcPageBuffer, sizeof(nfcPageBuffer));
    
    
    return ret;
}


bool readSRAM(void){
    bool ret=false;
    uint16_t offset;
    for (int i = SRAM_START_REG, j=0; i<=SRAM_END_REG; i++,j++) {
        offset = ((j)*16);
        ret = readBufferRegister(NXPNFC_ADDRESS, i, nfcPageBuffer, sizeof(nfcPageBuffer));
        if (ret==false) {
            return ret;
        }
        //memcpy(&userData[offset], pageBuffer, sizeof(pageBuffer));
    }
    return ret;
}


bool getNxpUserData(char* buffer) {
    if (readUserData(2)) {
        memcpy(buffer, getLastNfcPage(), 9);
        return true;
    } else
    return false;
}