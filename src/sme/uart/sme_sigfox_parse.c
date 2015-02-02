#include "sme_cdc_cmd_parse.h"
#include "sme_cmn.h"
#include "sme_cdc_uart.h"
#include "sme_sigfox_parse.h"
#include <ctype.h>
#include "..\model\sme_model_sigfox.h"
#include "sme\Devices\uart\sigFox\sme_sigfox_execute.h"
/*
* sme_sigfox_parse.c
*
* Created: 11/01/2015 12:22:09
*  Author: mfontane
*/

#define SIGFOX_DATA_C     'D'
#define SIGFOX_KEEP_C     'K'
#define SIGFOX_BIT_C      'B'
#define SIGFOX_CONFIRM_C  'C'

#define DBG_ENABLE  1
#define DBG_DISABLE 0
#define DBG_ENABLE_STR  '1'
#define DBG_DISABLE_STR '0'

char CDC_HELP_SIGFOX[]="Help: sf <c/s> <mode> <register> <data> > \r\n";

bool sme_dbg_sfx_enable = false;

static int parseCommandToken(sigFoxT *usartMsg){
    int ret = SME_OK;

    //  request of command without parameters prepare to send "+++"
    if (sme_cli_msg.token_idx == 2) {
        usartMsg->messageType = enterConfMode;
        return ret;
    }
    
    usartMsg->messageType = confCdcMessage;
    usartMsg->message.confMode.access = toupper(sme_cli_msg.token[2][0]);
    if (!((SIGFOX_REGISTER_READ == usartMsg->message.confMode.access) ||
    (SIGFOX_REGISTER_WRITE == usartMsg->message.confMode.access)))
    return SME_EINVAL;
    
    memcpy(usartMsg->message.confMode.registerAddr, sme_cli_msg.token[3], 3);
    
    // if the command is for read from register the parse is finish correctly
    if (SIGFOX_REGISTER_READ == usartMsg->message.confMode.access) {
        return SME_OK;
    }
    
    // if the command is for write to register should contains something to write
    if (SIGFOX_REGISTER_WRITE==usartMsg->message.confMode.access) {
        if (sme_cli_msg.token_idx < 5) {
            return SME_EINVAL;
        } else {
        int len = strlen((char *)sme_cli_msg.token[4]);
            for (int i=0;i<len;i++) {
                usartMsg->message.confMode.data[i] = sme_cli_msg.token[4][i]; 
            }
        }
    }
    
    return ret;
}

static int parseDataToken(sigFoxT *usartMsg ){
       
    //  request of command without parameters prepare to send "+++"
    if (sme_cli_msg.token_idx == 2) {
        usartMsg->messageType = enterDataMode;
        return SME_OK;
    }
    
    usartMsg->messageType = dataCdcMessage;
    if (sme_cli_msg.token_idx < 3) {
        return SME_EINVAL;
    }
    
    // take the type and payload if needed
    switch(toupper(sme_cli_msg.token[2][0])){
        case SIGFOX_DATA_C:
        usartMsg->message.dataMode.type = SIGFOX_DATA;
        // take the payload
        uint8_t len=0;
        sme_hex_str_to_data(sme_cli_msg.token[3], usartMsg->message.dataMode.payload, &len);
        usartMsg->message.dataMode.length = len;
        if (len<1) { // if no payload retrun error
            return SME_EINVAL;
        }
        break;
        
        case  SIGFOX_KEEP_C:
        usartMsg->message.dataMode.type = SIGFOX_KEEP;
        usartMsg->message.dataMode.length = 0;
        break;
        
        case  SIGFOX_BIT_C:
        usartMsg->message.dataMode.type = SIGFOX_BIT;
        usartMsg->message.dataMode.length = 1;
        if (sme_cli_msg.token_idx==3) { // no data... send 0
            usartMsg->message.dataMode.payload[0]=0;
            } else {
            sme_hex_str_to_uint8(sme_cli_msg.token[3], usartMsg->message.dataMode.payload);
        }
        break;
    }    
    
    // everything is OK, add the new sequence number
    usartMsg->message.dataMode.sequenceNumber=getNewSequenceNumber();
    
    return SME_OK;
}



int parseSigFoxMsg(void) {
    int err = SME_OK;
    sigFoxT *usartMsg = getSigFoxModel();
        
    // read operation
    if (sme_cli_msg.token[1][0] != 0) {
        switch(sme_cli_msg.token[1][0]) {
            case 'c':
            err |= parseCommandToken(usartMsg);
            break;
            
            case 's':
            err |= parseDataToken(usartMsg);
            break;
           
            default:
            // print help           
            return SME_EINVAL;
            break;
        }
    } else {
        // print help
        return SME_EINVAL;
    }
    
    if (SME_OK == err) {
        if (executeCDCSigFox(usartMsg) != true){
            err = SME_EIO;
        }
    }

    return err;
}