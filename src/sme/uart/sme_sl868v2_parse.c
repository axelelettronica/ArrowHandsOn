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


static int parseNmeaCommandToken(sl868v2T *usartMsg);
bool sme_dbg_gps_enable = true;
bool sme_dbg_gps_msg_enable = true;


int getTalkerType(uint8_t *in, sl868v2MsgE *nmeaType)
{
    if (!in || !nmeaType) {
        return SME_EINVAL;
    }
    if ((((in[0]=='G') && ((in[1]=='P') || (in[1]=='L') || (in[1]=='N'))) ||
        ((in[0]=='B') && (in[1]=='D')))) 
    {
        *nmeaType = STD_NMEA;
        return SME_OK;
    } else if ((in[0]=='P') && (in[1]=='M') && (in[2]=='T') && (in[3]=='K')) {
        *nmeaType = MTK_NMEA;
        return SME_OK;
    }

    return SME_EINVAL;
}

static int getSentenceId(uint8_t *in, uint8_t *out)
{   
    bool found = false;

    if (!in || !out) {
        return SME_EINVAL;
    }
    print_dbg("sentence %c %c %c  \n", in[0], in[1], in[2]);
    
    if ((in[0]=='G') && (in[1]=='G') && (in[2]=='A')) {
        found = true;
        goto end;
    }
    if ((in[0]=='G') && (in[1]=='L') && (in[2]=='L')) {
        found = true;
        goto end;
    }
    if ((in[0]=='G') && (in[1]=='S') && (in[2]=='A')) {
        found = true;
        goto end;
    }
    if ((in[0]=='G') && (in[1]=='S') && (in[2]=='V')) {
        found = true;
        goto end;
    }
    if ((in[0]=='R') && (in[1]=='M') && (in[2]=='C')) {
        found = true;
        goto end;
    }
    if ((in[0]=='V') && (in[1]=='T') && (in[2]=='G')) {
        found = true;
        goto end;
    }
    if ((in[0]=='Z') && (in[1]=='D') && (in[2]=='A')) {
        found = true;
        goto end;
    }
end:
    if (found) {
        out[0] = in[0];
        out[1] = in[1];
        out[2] = in[2];
        return SME_OK;
    }
    return SME_EINVAL;
}

static int parseNmeaCommandToken(sl868v2T *usartMsg)
{
    int ret = SME_OK;
    uint8_t  offset = 0;

    if (sme_cli_msg.token_idx < 3) {
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
        if (SME_OK !=getSentenceId(&sme_cli_msg.token[2][offset], usartMsg->nmea_msg.std.sentenceId)) {
            return SME_EINVAL;
        }
        offset += NMEA_SENTENCE_ID_LEN;
        /* Filling data */
        usartMsg->nmea_msg.std.dataLenght = strlen((const char *)&sme_cli_msg.token[2][offset]);
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
        usartMsg->nmea_msg.mtk.dataLenght = strlen((const char *)&sme_cli_msg.token[2][offset]);

        if(usartMsg->nmea_msg.mtk.dataLenght) {
            memcpy(usartMsg->nmea_msg.mtk.data, &sme_cli_msg.token[2][offset],
                   usartMsg->nmea_msg.mtk.dataLenght);
        }
    }

    return ret;
}

int parseSl868v2Msg(void) {
    int err = SME_OK;
    sl868v2T *usartMsg = getSl868v2Model();
    
    // read operation
    if (sme_cli_msg.token[1][0] != 0) {
        switch(sme_cli_msg.token[1][0]) {
            case 'c':
                err |= parseNmeaCommandToken(usartMsg);
            break;
            
             case 'd':
                usartMsg->messageType=MTK_NMEA;
                memcpy(usartMsg->nmea_msg.mtk.talker, SL868V2_CONF_CMD, sizeof(usartMsg->nmea_msg.mtk.talker));
                memset(usartMsg->nmea_msg.mtk.msgId, 0x30, sizeof(usartMsg->nmea_msg.mtk.msgId));
                 memset(usartMsg->nmea_msg.mtk.data, 0, MTK_NMEA_MAX_DATA_LEN);
                 usartMsg->nmea_msg.mtk.dataLenght=0;
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
        executeSl868v2(usartMsg);
    }
    
    return err;
}