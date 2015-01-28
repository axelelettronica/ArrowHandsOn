/*
* sme_sl868v2_execute.c
*
* Created: 22/01/2015 20:43:25
*  Author: speirano
*/
#include "sme_sl868v2_execute.h"
#include "port.h"
#include "sme_sl868v2_usart.h"
#include "samd21_xplained_pro.h"
//#include "portmacro.h"
#include "sme_cmn.h"
#include "../../../sme_FreeRTOS.h"

#define SL868V2_DELIMITER      '*'   
#define SL868V2_START_MESSAGE  '$'

int executeSl868v2(const sl868v2T *data);

/* Generic NMEA Sentence Format 
 *
 * $GNVTG,76.25,T,,M,0.57,N,1.05,K,A*13<CR><LF>  total size 82
 *
 *Field  Example Comment:
 * $ Start of Sentence
 * Talker ID GN Talker Identifier 
 *    GP = GPS
 *    GL = GLONASS
 *    GN = Global Navigation 
 *    BD = BeiDou (COMPASS)
 * Sentence ID VTG NMEA Sentence Identifier
 *    GGA, GLL, GSA, GSV, RMC, VTG, ZDA
 * Data Fields
 *    76.25 The number of Data Fields depends upon the sentence type.
 *    T <NULL> M
 *    0.57
 *    N
 *    1.05
 *    K
 *    A
 *    
 *    A comma symbol ‘,’ is used to separate, or delimit, each data field from another. 
 *    NULL fields contain no characters, but are separated by commas from the other data fields.
 *
 *  End of Data Fields 
 *    *    Asterisk delimiter denoting the end of the Data Fields.
 *  Checksum 
 *    Two character hexadecimal checksum value computed by taking the 
 *    8-bit Exclusive OR (XOR) sum of all characters between the  ‘$’ and ‘*’ characters.
 *
 *  Terminator <CR><LF> 
 *    Carriage Return <CR> Line Feed <LF> sequence indicating the end of a packet
 */


#define  LS868V2_MAX_MSG_SIZE 82 
uint8_t message[LS868V2_MAX_MSG_SIZE];
//uint8_t sequenceNumber;


static int sendSl868v2Msg(const uint8_t *msg, uint8_t len) {
    port_pin_toggle_output_level(LED_0_PIN); // just for debug
    sl868v2SendMessage(msg, len);
    return SME_OK;
}


static int  sendSl868v2DataMessage(const sl868v2MsgT *packet) {
    int msgLen=0, i;
    char checksum = 0;
    char checksum_str[3]={};

    memset(message, 0, LS868V2_MAX_MSG_SIZE);
    message[msgLen++] = SL868V2_START_MESSAGE;
    message[msgLen++] = packet->std.talker[0];
    message[msgLen++] = packet->std.talker[1];
    message[msgLen++] = packet->std.sentenceId[0];
    message[msgLen++] = packet->std.sentenceId[1];
    message[msgLen++] = packet->std.sentenceId[2];

    for (i=0; i < packet->std.dataLenght; i++) {
        message[msgLen++] = packet->std.data[i];
    }
    for (i=1; i < msgLen; i++) {
       checksum ^=message[i]; // calculating XOR checksum
    }
    message[msgLen++] = SL868V2_DELIMITER;
    sprintf(checksum_str, "%x", checksum);
    message[msgLen++] = (checksum > 15) ? checksum_str[0]:'0';
    message[msgLen++] = (checksum > 15) ? checksum_str[1] : checksum_str[0];
    message[msgLen++] = '\r';
    message[msgLen++] = '\n';

    //finally SEND !!!
    sendSl868v2Msg(message, msgLen);
    return SME_OK;
};

int executeSl868v2(const sl868v2T *data) 
{
   return sendSl868v2DataMessage(&data->nmea_msg);
}