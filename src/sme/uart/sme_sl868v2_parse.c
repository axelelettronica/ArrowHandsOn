#include "sme_cdc_cmd_parse.h"
#include "sme_cmn.h"
#include "sme_cdc_uart.h"
#include "sme_sl868v2_parse.h"
//#include <ctype.h>
#include "..\model\sme_model_sl868v2.h"
#include "sme\Devices\uart\gps\sme_sl868v2_execute.h"
/*
* sme_sl868v2_parse.c
*
* Created: 22/01/2015 12:22:09
*  Author: speirano
*/

#define NMEA_TALKER_LEN       2
#define NMEA_SENTENCE_ID_LEN  3

static int getTalkerType(char *in, sl868v2MsgE *nmeaType)
{
    if (!in || !nmeaType) {
        return SME_EINVAL;
    }
    if (((in[0]=='G') && ((in[1]=='P') || (in[1]=='L') || (in[1]=='N')) ||
        ((in[0]=='B') && (in[1]=='D')))) 
    {
        *nmeaType = STD_NMEA;
        return SME_OK;
    } else if ((in[0]=='P') && (in[1]=='M') && (in[0]=='T') && (in[1]=='K')) {
        *nmeaType = MTK_NMEA;
        return SME_OK;
    }

    return SME_EINVAL;
}

static int getSentenceId(char *in, char *out)
{   
    bool ok = false;

    if (!in || !out) {
        return SME_EINVAL;
    }
    
    if (((in[0]=='G') && ((in[1]=='P') || (in[1]=='L') || (in[1]=='N'))) ||
        ((in[0]=='B') && (in[1]=='D'))) {
        out[0] = in[0];
        out[1] = in[1];
        return SME_OK;
    }
    return SME_EINVAL;
}

static int parseCommandToken(void){
    int ret = SME_OK;
    sl868v2T *usartMsg = getSl868v2Model();
    uint8_t  offset = 0;

    if (sme_cli_msg.token_idx != 3) {
        return SME_EINVAL;
    }

    memset(usartMsg, 0, sizeof(sl868v2T));
    
    /* Filling Talker */
    if (SME_OK != getTalkerType(&sme_cli_msg.token[2][offset],  &usartMsg->messageType)) {
        return SME_EINVAL;
    }

    if (usartMsg->messageType == STD_NMEA) {

        usartMsg->nmea_msg.std.talker[0] = sme_cli_msg.token[2][offset];
        offset++;
        usartMsg->nmea_msg.std.talker[1] = sme_cli_msg.token[2][offset];
        offset++;

        /* Filling Sentence Id */
        if (!getSentenceId(&sme_cli_msg.token[2][offset], &usartMsg->nmea_msg.std.sentenceId)) {
            return SME_EINVAL;
        }
        offset += NMEA_SENTENCE_ID_LEN;
        /* Filling data */
        usartMsg->nmea_msg.std.dataLenght = strlen(&sme_cli_msg.token[2][offset]);
        if(!usartMsg->nmea_msg.std.dataLenght) {
            return SME_EINVAL;
        }
        memcpy (usartMsg->nmea_msg.std.data, &sme_cli_msg.token[2][offset],
                usartMsg->nmea_msg.std.dataLenght);
    } else {   
        usartMsg->nmea_msg.mtk.talker[0] = sme_cli_msg.token[2][offset];
        offset++;
        usartMsg->nmea_msg.mtk.talker[1] = sme_cli_msg.token[2][offset];
        offset++;
        usartMsg->nmea_msg.mtk.talker[2] = sme_cli_msg.token[2][offset];
        offset++;
        usartMsg->nmea_msg.mtk.talker[3] = sme_cli_msg.token[2][offset];
        offset++;
        usartMsg->nmea_msg.mtk.msgId[0] = sme_cli_msg.token[2][offset];
        offset++;
        usartMsg->nmea_msg.mtk.msgId[1] = sme_cli_msg.token[2][offset];
        offset++;
        usartMsg->nmea_msg.mtk.msgId[2] = sme_cli_msg.token[2][offset];
        offset++;
        /* Filling data */
        usartMsg->nmea_msg.mtk.dataLenght = strlen(&sme_cli_msg.token[2][offset]);

        if(!usartMsg->nmea_msg.mtk.dataLenght) {
            return SME_EINVAL;
        }
        memcpy (usartMsg->nmea_msg.mtk.data, &sme_cli_msg.token[2][offset],
        usartMsg->nmea_msg.mtk.dataLenght);
    }


   
    return ret;
}

int parseSl868v2Msg(void **componentStr) {
    int err = SME_OK;
    sl868v2MsgT *usartMsg = getSl868v2Model();
    
    *componentStr = usartMsg; // assign the pointer to the struct
    
    // read operation
    if (sme_cli_msg.token[1][0] != 0) {
        switch(sme_cli_msg.token[1][0]) {
            case 'c':
                err |= parseCommandToken();
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