#include "sme_cdc_cmd_parse.h"
#include "sme_cmn.h"
#include "sme_cdc_uart.h"
#include "..\tasks\uart\sme_sigfox_execute.h"
/*
* sme_sigfox_parse.c
*
* Created: 11/01/2015 12:22:09
*  Author: mfontane
*/


sigFoxT usartMsg;

int parseSigFoxMsg(void **componentStr) {
	int err = SME_OK;
	
	*componentStr = &usartMsg; // assign the pointer to the struct
	
	// read operation
	if (sme_cli_msg.token[1][0] != 0) {
		switch(sme_cli_msg.token[1][0]) {
			case 'c':
			usartMsg.messageType = confMessage;
			break;
			
			case 'd':
			usartMsg.messageType = dataMessage;
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
	
	return err;
}