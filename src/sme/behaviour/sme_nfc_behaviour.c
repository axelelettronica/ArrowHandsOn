#include "model\sme_model_sigfox.h"
#include "Devices\I2C\nfc\nxpNfc.h"
#include <string.h>
#include "Devices\uart\sigFox\sme_sigfox_execute.h"
/*
 * sme_nfc_behaviour.c
 *
 * Created: 3/1/2015 9:41:18 PM
 *  Author: mfontane
 */ 



/*
* Third Use case
detect NFC interrupt:
1) take data from sensor
2) take GPS position
3) Send all to SigFox
*/
void nfcExecution(void) {
    sigFoxT *sfModel = getSigFoxModel();

    sfModel->messageType = dataIntMessage;
    sfModel->message.dataMode.type = SIGFOX_DATA;

    // point 1
    int offset;
    for (int page=0; page<255/NFC_PAGE_SIZE; page++){
        if (readUserData(page)) {
            
            sfModel->message.dataMode.length+=NFC_PAGE_SIZE;

            offset = ((page)*NFC_PAGE_SIZE);
            memcpy(&sfModel->message.dataMode.payload[offset],getLastNfcPage(),NFC_PAGE_SIZE);
        }
    }

    //point 2 (could be a FSM because has to be wait the GSM wake-up)
    // getPosition();

    //point 3 SEND !!!!!!!!!!!
    sfModel->message.dataMode.sequenceNumber = getNewSequenceNumber();

    executeCDCSigFox(sfModel);
}