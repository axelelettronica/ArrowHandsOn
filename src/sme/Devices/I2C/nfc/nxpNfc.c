#include "..\I2C.h"
#include "nxpNfc.h"
#include <string.h>
/*
* nxpNfc.c
*
* Created: 29/12/2014 21:42:02
*  Author: mfontane
*/

static uint8_t  pageBuffer[16];
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
static uint8_t userData[USER_DATA_LEN];

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
	return readBufferRegister(NXPNFC_ADDRESS, CONFIG_REG, pageBuffer, sizeof(pageBuffer));
}

bool getSessionReg(void) {
	return readBufferRegister(NXPNFC_ADDRESS, SESSION_REG, pageBuffer, sizeof(pageBuffer));
}

bool readUserData(void){
	bool ret=false;
	uint16_t offset;
	for (int i = USER_START_REG; i<=USER_END_REG; i++) {
		offset = ((i-1)*16);
		ret = readBufferRegister(NXPNFC_ADDRESS, i, pageBuffer, sizeof(pageBuffer));
		if (ret==false) {
			return ret;
		}
		if (i!= USER_END_REG) {
			memcpy(&userData[offset], pageBuffer, sizeof(pageBuffer));
			} else {
			memcpy(&userData[offset], pageBuffer, 8);
		}
	}
	
	return ret;
}


bool readSRAM(void){
	bool ret=false;
	uint16_t offset;
	for (int i = SRAM_START_REG, j=0; i<=SRAM_END_REG; i++,j++) {
		offset = ((j)*16);
		ret = readBufferRegister(NXPNFC_ADDRESS, i, pageBuffer, sizeof(pageBuffer));
		if (ret==false) {
			return ret;
		}
		memcpy(&userData[offset], pageBuffer, sizeof(pageBuffer));		
	}
	return ret;
}


bool getNxpUserData(char* buffer) {
	if (readUserData()) {
	    memcpy(buffer, userData, USER_DATA_LEN)	;
	return true;
	} else 
	return false;
}