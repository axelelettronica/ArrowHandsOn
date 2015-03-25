/*
 * sme_sl868v2_rx_fsm.h
 *
 * Created: 2/25/2015 6:42:01 PM
 *  Author: smkk
 */ 


#ifndef SME_SL868V2_RX_FSM.C_H_
#define SME_SL868V2_RX_FSM.C_H_




#include "sme\model\sme_model_sl868v2.h"


uint8_t sl868v2HandleRx(uint8_t *msg, uint8_t msgMaxLen);
void sl868InitRxData(void);


#endif /* SME_SL868V2_RX_FSM.C_H_ */
