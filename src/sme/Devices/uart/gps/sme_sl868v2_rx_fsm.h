/*
 * sme_sl868v2_rx_fsm.h
 *
 * Created: 1/31/2015 8:43:12 PM
 *  Author: speirano
 */ 


#ifndef SME_SL868V2_RX_FSM_H_
#define SME_SL868V2_RX_FSM_H_


#include "sme\model\sme_model_sl868v2.h"


uint8_t sl868v2HandleRx(uint8_t *msg, uint8_t msgMaxLen);
void sl868InitRxData(void);

#endif /* SME_SL868V2_RX_FSM_H_ */