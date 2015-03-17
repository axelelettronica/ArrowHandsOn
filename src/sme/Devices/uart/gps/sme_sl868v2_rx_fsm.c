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
#include "../../../tasks/sme_controller.h"
#include "sme\Devices\uart\gps\sme_sl868v2_execute.h"
#include "sme/Devices/uart/gps/sme_gps_timer.h"
#include "../../IO/sme_rgb_led.h"


#define SL868V2_MAX_MSG_LEN   100
typedef struct {
    xSemaphoreHandle sem;
    uint8_t idx;
    uint8_t data[SL868V2_MAX_MSG_LEN];
} sl868v2RxMsgT;

static sl868v2RxMsgT rxMsg;

static bool cold_boot = true;
static bool scan_in_progress = false;
/***/
typedef struct {
    uint8_t    *talker_p;
    uint8_t    *sentenceId_p;
    uint8_t    *data_p;
    uint8_t    dataLenght;
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

BaseType_t sl868RxMsgGive() {
    return xSemaphoreGive(rxMsg.sem);
}
BaseType_t sl868RxMsgTake(void) {
    return xSemaphoreTake(rxMsg.sem, UART_TASK_DELAY);
}

void sl868InitRxData(void)
{
    memset(&rxMsg, 0, sizeof(rxMsg));
    rxMsg.sem = xSemaphoreCreateMutex();
}



static bool crcCheck(uint8_t data[], uint8_t len) {

    char checksum = 0;
    char checksum_str[3]={};
    uint8_t i = 1;

    while (data[i] != '*') {
        if (i >= len) {
            print_err("Missing Checksum\n");
            return false;
        }
        checksum ^=data[i++];
    }
    i++;
    sprintf(checksum_str, "%0X", checksum);
    if (data[i++] != ((checksum > 15) ? checksum_str[0]:'0')) {
        print_err("Wrong MSB Checksum %d (%d)\n", data[i-1],checksum);
        return false;
        } else if (data[i++] != ((checksum > 15) ? checksum_str[1] : checksum_str[0])) {
        print_err("Wrong LSB Checksum %d (%d)\n", data[i-1],checksum);
        return false;
    };
    return true;
}

static void sl868v2ParseRx (void)
{
    uint8_t offset= 0;
    uint8_t i = 0;

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

        while( rxMsg.data[offset++] != '*') {
            if( rxMsg.data[offset] == 0) {
                return;
            }
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
        while( rxMsg.data[offset++] != '*') {
            i++;
        }
        msgPtrT.nmea_p.mtk_p.dataLength = i;

    }
    // check checksum
    //crcCheck(rxMsg.data, rxMsg.idx);
}


void gpsStartScan(exec_callback call_back) {
    
    print_gps("GPS status: %s\n",__FUNCTION__);

    // Check if it is required to enable step up.
    // This is required if just the battery pack power supply is provided.
    bool pin_state = port_pin_get_input_level(EXT_INT_PIN_POUT);
    if (!pin_state) {
        // no further power supply is present, enable STEP_UP
        port_pin_set_output_level(STEP_UP_PIN_PIN, true);
        port_pin_set_output_level(SME_LED_Y2_PIN, SME_LED_Y2_ACTIVE);
        print_gps("Enabling STEP-UP\n");
    }
    sendSl868v2Msg(SL868V2_WARM_RST_CMD,
    sizeof(SL868V2_WARM_RST_CMD));
    startGpsCommandTimer(call_back);
    scan_in_progress = true;
}


void gpsStopScan(void) {

    print_gps("GPS status: %s\n",__FUNCTION__);

    sendSl868v2Msg(SL868V2_SET_STDBY_CMD,
    sizeof(SL868V2_SET_STDBY_CMD));
    stopGpsCommandTimer();
    scan_in_progress = false;
    
    bool pin_state = port_pin_get_output_level(STEP_UP_PIN_PIN);
    if (pin_state) {
       // Deactivate Step-up
       port_pin_set_output_level(STEP_UP_PIN_PIN, false);
       port_pin_set_output_level(SME_LED_Y2_PIN, SME_LED_Y2_INACTIVE);
       print_gps("Disabling STEP-UP\n");
    }
}



typedef enum  {
    NMEA_UNMANAGED,
    NMEA_RMC,
    NMEA_GGA
}sl868v2_nmea_out_msg_id;


sl868v2_nmea_out_msg_id
sl868v2IdentifyNmeaRxMsg(uint8_t talker_p[], uint8_t sentenceId_p[])
{
    if ((talker_p[0] == 'G') && ((talker_p[1] == 'P')||
    (talker_p[1] == 'L') || (talker_p[1] == 'N'))) {
        if ((sentenceId_p[0] == 'R') &&
        (sentenceId_p[1] == 'M') &&
        (sentenceId_p[2] == 'C')) {
            return NMEA_RMC;
        }
        if ((sentenceId_p[0] == 'G') &&
        (sentenceId_p[1] == 'G') &&
        (sentenceId_p[2] == 'A')) {
            return NMEA_GGA;
        }
    }

    return NMEA_UNMANAGED;
}

void sl868v2ProcessRx(void)
{
    uint32_t lat;
    uint32_t longit;
    uint32_t alt;
    sl868v2_nmea_out_msg_id nmea_msg_id;
    if (cold_boot) {
        // set GPS in standby at first message
        sendSl868v2Msg(SL868V2_SET_STDBY_CMD,
        sizeof(SL868V2_SET_STDBY_CMD));
        cold_boot = false;
    }
    


    if (msgPtrT.messageType == STD_NMEA) {
        nmea_msg_id = sl868v2IdentifyNmeaRxMsg(msgPtrT.nmea_p.std_p.talker_p,
        msgPtrT.nmea_p.std_p.sentenceId_p);
        switch(nmea_msg_id) {
            case NMEA_RMC:
                sme_parse_coord(msgPtrT.nmea_p.std_p.data_p, msgPtrT.nmea_p.std_p.dataLenght, SME_LAT);
                sme_parse_coord(msgPtrT.nmea_p.std_p.data_p, msgPtrT.nmea_p.std_p.dataLenght, SME_LONG);
            break;
            
            case NMEA_GGA:
                sl868v2_parse_ssa(msgPtrT.nmea_p.std_p.data_p, msgPtrT.nmea_p.std_p.dataLenght);
            break;
            
            case NMEA_UNMANAGED:
            default:
            break;
        }
    }
}



uint8_t sl868v2HandleRx(uint8_t *msg, uint8_t msgMaxLen)
{
    if (rxMsg.idx < (SL868V2_MAX_MSG_LEN-2)) {
        
        if ((*msg < 21) && !((*msg == '\n') || (*msg == '\r')))  {
            return SME_OK;
            }  else if (*msg == '$') {
            memset(&rxMsg, 0, sizeof(rxMsg));
        }

        rxMsg.data[rxMsg.idx++] = *msg;

        if (*msg == '\n')  {
            if (rxMsg.idx > 3) {
                rxMsg.data[rxMsg.idx] = '\0';
                if (crcCheck(rxMsg.data, rxMsg.idx)) {
                    sl868v2ParseRx();
                    sl868v2ProcessRx();
                }
                print_dbg(" received: %s", rxMsg.data);
            }
        }
        } else {
        print_err("ERR: Msg too big, drop it: %s", rxMsg.data);
        memset(&rxMsg, 0, sizeof(rxMsg));
    }

    return SME_OK;
}