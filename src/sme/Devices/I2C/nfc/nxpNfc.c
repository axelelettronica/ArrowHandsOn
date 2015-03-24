/*
 * nxpNfc.c
 *
 * Created: 29/12/2014 21:42:02
 *  Author: smkk
 */


#include <string.h>
#include "..\I2C.h"
#include "nxpNfc.h"
#include "model\NT3H1101_model.h"
#include "nfcSpecification\nfcForum.h"



inline const uint8_t* get_last_ncf_page(void) {
	return nfcPageBuffer;
}



#define DEMO_TEXT "PoppI"

bool nxpInit(void){
	NDEFRecordStr record;
	bool    ret=true;
	uint8_t recordLength;

	ret &= nfc_read_configuration_register();
	ret &= readManufactoringData();

	uint8_t data[NFC_PAGE_SIZE];
	NDEFHeaderStr header;


	recordLength = composeNDEFText(sizeof(DEMO_TEXT)-1, &record);
	header.startByte = NDEF_START_BYTE;
	header.payloadLength = sizeof(DEMO_TEXT)-1 + recordLength;
	composeNDEFMBME(true, true, &record);

	memcpy(data, &header, sizeof(header));
	memcpy(&data[sizeof(header)], &record, recordLength);
	memcpy(&data[sizeof(header)+recordLength], DEMO_TEXT, (sizeof(DEMO_TEXT)-1));
	data[sizeof(header)+recordLength+sizeof(DEMO_TEXT)-1] = NDEF_END_BYTE;


	nfc_write_user_data(0, data, NFC_PAGE_SIZE);


	return ret;

}
bool readManufactoringData(void) {
	bool ret = false;

	ret = readBufferRegister(NXPNFC_ADDRESS, MANUFACTORING_DATA_REG, manuf.page, sizeof(manuf.page));
	return ret;

}

bool nfc_read_configuration_register(void){
	bool ret=false;

	ret = readBufferRegister(NXPNFC_ADDRESS, CONFIG_REG, configuration.page, sizeof(configuration.page));

	return ret;
}


bool getConfiguration(void) {
	return readBufferRegister(NXPNFC_ADDRESS, CONFIG_REG, nfcPageBuffer, sizeof(nfcPageBuffer));
}

bool getSessionReg(void) {
	return readBufferRegister(NXPNFC_ADDRESS, SESSION_REG, nfcPageBuffer, sizeof(nfcPageBuffer));
}


bool nfc_read_user_data(uint8_t page) {
	uint8_t reg = USER_START_REG+page;
	// if the requested page is out of the register exit with error
	if (reg > USER_END_REG) {
		return false;
	}

	bool ret=false;

	ret = readBufferRegister(NXPNFC_ADDRESS, reg, nfcPageBuffer, sizeof(nfcPageBuffer));


	return ret;
}


bool nfc_write_user_data(uint8_t page, const uint8_t* data, uint16_t dataLen) {
	uint8_t dataSend[NFC_PAGE_SIZE +1]; // data plus register

	uint8_t reg = USER_START_REG+page;

	// if the requested page is out of the register exit with error
	if (reg > USER_END_REG) {
		return false;
	}

	uint8_t pages = dataLen/NFC_PAGE_SIZE +1;
	for (int i=reg; i<=page+pages; i++) {
		// reach the end of pages exit with success because of the others.
		if (i > USER_END_REG) {
			return true;
		}
		dataSend[0] = i; // store the register
		memcpy(&dataSend[1], &data[i-reg], NFC_PAGE_SIZE);
		writeBufferRegister(NXPNFC_ADDRESS, dataSend, sizeof(dataSend));
	}
}

bool nfc_read_sram_register(void){
	bool ret=false;
	for (int i = SRAM_START_REG, j=0; i<=SRAM_END_REG; i++,j++) {
		ret = readBufferRegister(NXPNFC_ADDRESS, i, nfcPageBuffer, sizeof(nfcPageBuffer));
		if (ret==false) {
			return ret;
		}
		//memcpy(&userData[offset], pageBuffer, sizeof(pageBuffer));
	}
	return ret;
}




void getNxpSerialNumber(char* buffer) {
	for(int i=0; i<6; i++) {
		buffer[i] = manuf.manufS.serialNumber[i];
	}
}
