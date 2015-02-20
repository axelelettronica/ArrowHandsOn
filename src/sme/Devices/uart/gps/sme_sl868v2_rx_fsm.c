/*
* sme_sl868v2_rx_fsm.c
*
* Created: 1/31/2015 8:42:56 PM
*  Author: speirano
*/
#include "sme_sl868v2_rx_fsm.h"
#include "sme\model\sme_model_sl868v2.h"
#include "../../../uart/sme_sl868v2_parse.h"
#include "sme_cmn.h"

#define SL868V2_MAX_MSG_LEN   80    
typedef struct {
    uint8_t idx;
    uint8_t data[SL868V2_MAX_MSG_LEN];
} sl868v2RxMsgT;

static sl868v2RxMsgT rxMsg;
/***/
typedef struct {
    void    *talker_p;
    void    *sentenceId_p;
    void    *data_p;
    uint8_t dataLenght;
} sl868v2StdMsgPtrT;

typedef struct {
    void    *talker_p;
    void    *msgId_p;
    void    *data_p;
    uint8_t dataLength;
} sl868v2MTKMsgPtrT;

typedef union {
    sl868v2StdMsgPtrT std_p;
    sl868v2MTKMsgPtrT mtk_p;
} sl868v2MsgPtrT;

typedef struct {
    sl868v2MsgE    messageType;
    sl868v2MsgPtrT nmea_p;
} sl868v2PtrT;

sl868v2PtrT  msgPtrT;
/***/
  
void sl868InitRxData(void)
{
    memset(&rxMsg, 0, sizeof(rxMsg));
}

static void sl868v2ParseRx (void)
{
    uint8_t offset= 0;
    uint8_t i = 0;
    char checksum = 0;
    char checksum_str[3]={};

    if (rxMsg.idx <= 1) {
        return;
    }
    offset++;   // skip $
     
    if (SME_OK != getTalkerType(&rxMsg.data[offset],  &msgPtrT.messageType)) {
        return;
    }

    if (msgPtrT.messageType == STD_NMEA) {

        msgPtrT.nmea_p.std_p.talker_p =  &rxMsg.data[offset];
        offset+=2;

        /* Filling Sentence Id */
        msgPtrT.nmea_p.std_p.sentenceId_p = &rxMsg.data[offset];
        offset += NMEA_SENTENCE_ID_LEN;

        /* Filling data */
        msgPtrT.nmea_p.std_p.data_p = &rxMsg.data[offset];

        while( &rxMsg.data[offset++] != '*') {
            i++;
        }
        msgPtrT.nmea_p.std_p.dataLenght = i;
        
    } else {
        msgPtrT.nmea_p.mtk_p.talker_p = &rxMsg.data[offset];
        offset+=4;
        msgPtrT.nmea_p.mtk_p.msgId_p = &rxMsg.data[offset];
        offset+=3;
        msgPtrT.nmea_p.mtk_p.data_p  = &rxMsg.data[offset];
        /* Filling data */
        while( &rxMsg.data[offset++] != '*') {
            i++;
        }
        msgPtrT.nmea_p.mtk_p.dataLength = i;

    }
    // check checksum
    i = 1;
    while (rxMsg.data[i] != '*') {
        checksum ^=rxMsg.data[i++];
    }
    i++;
    sprintf(checksum_str, "%x", checksum);
    if (rxMsg.data[i++] != ((checksum > 15) ? checksum_str[0]:'0')) {
        print_err("Wrong Checksum");
    } else if (rxMsg.data[i++] != ((checksum > 15) ? checksum_str[1] : checksum_str[0])) {
        print_err("Wrong Checksum");
    };
}

/*
typedef enum {
    headerRec,
    lenRec,
    typeRec,
    sequenceRec,
    payloadRec,
    crcRec,
    tailerRec
} sfxRxFSME;

sigFoxMessageTypeE cdcStatus=0;
sigFoxRxMessage answer;
sfxRxFSME       recFsm;
uint8_t         crcCounter;

inline void set_sfx_cdc_status(sigFoxMessageTypeE state) {
    cdcStatus = state;
}

static sfxRxFSME crcCheck(void) {
 uint16_t crc = calculateCRC(answer.length, answer.type,
                            answer.sequenceNumber, answer.payload);
 uint16_t *receivedCrc=answer.crc;

 if (*receivedCrc == crc)
 return tailerRec;
 else
 return headerRec;
}

static uint8_t handleData(uint8_t *msg, uint8_t msgMaxLen) {
    for (int i=0; i<msgMaxLen; i++){
        switch (recFsm) {
            case headerRec:
            if (SFX_MSG_HEADER != msg[i])
            return SME_SFX_KO;
            else
            recFsm = lenRec;
            break;

            case lenRec:
            answer.length = msg[i];
            recFsm = typeRec;
            break;

            case typeRec:
            answer.type  = msg[i];
            recFsm = sequenceRec;
            break;

            case sequenceRec:
            answer.sequenceNumber = msg[i];
            answer.payloadPtr = 0;
            recFsm = payloadRec;
            break;

            case payloadRec:
            answer.payload[answer.payloadPtr++]= msg[i];
            if (answer.payloadPtr == answer.length) {
                crcCounter =0;
                recFsm = crcRec;
            }
            break;

            case crcRec:
            answer.crc[crcCounter++] = msg[i];
            if (crcCounter == 2)
            recFsm = crcCheck();
            break;

            case tailerRec:
             recFsm = headerRec;
            if (SFX_MSG_TAILER == msg[i])
            return SME_SFX_OK;
            else
            return SME_SFX_KO;
        }
    }
    return SME_OK;
}

static uint8_t handleConf(uint8_t *msg, uint8_t msgMaxLen) {
    for (int i=0; i < msgMaxLen; i++) {
        // store all the received bye till the end of message
        if (msg[i] != SIGFOX_END_MESSAGE) {
            answer.payload[answer.payloadPtr++]= msg[i];
            print_out(&msg[i]);
            } else {
            // end found check if the answer is ok
            if (SGF_CONF_ERROR != answer.payload[0]) {
                return SME_SFX_OK;
                } else {
                return SME_SFX_KO;
            }
        }
    }
    return SME_OK;
}
*/
uint8_t sl868v2HandleRx(uint8_t *msg, uint8_t msgMaxLen) 
{
   if (rxMsg.idx < (SL868V2_MAX_MSG_LEN-1)) {
      rxMsg.data[rxMsg.idx++] = *msg;

      if (*msg == '\n')  {
         rxMsg.data[rxMsg.idx] = '\0';
         sl868v2ParseRx();
         sl868InitRxData();
         print_dbg(" received: %s", rxMsg.data);
      }
   } 



    /*
    switch(cdcStatus){
        // first byte reset the payload ptr
        case enterConfMode:
        case confCdcMessage:
        // move the FSM to idle when finis is OK or ERROR
        if (SME_OK != handleConf(msg, msgMaxLen)) {
            answer.payloadPtr=0;
        }
        
        case enterDataMode:
        case dataCdcMessage:
        if (SME_OK != handleData(msg, msgMaxLen)) {
            answer.payloadPtr=0;
        }
        return SME_OK;

        default:
        return SME_EINVAL;
    }*/
    return SME_EINVAL;
}