/*
 * sme_sigfox_rx_fsm.h
 *
 * Created: 1/25/2015 8:43:12 PM
 *  Author: mfontane
 */ 


#ifndef SME_SIGset_sgf_fsmFOX_RX_FSM_H_
#define SME_SIGFOX_RX_FSM_H_


#include "sme\model\sme_model_sigfox.h"

void resetRxFsm(void);
uint8_t sfxHandleRx(uint8_t *msg, uint8_t msgMaxLen);

#endif /* SME_SIGFOX_RX_FSM_H_ */