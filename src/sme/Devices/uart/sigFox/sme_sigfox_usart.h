
/*
 * sme_sigfox_usart.h
 *
 * Created: 10/01/2015 22:41:06
 *  Author: mfontane
 */ 


#ifndef SME_SIGFOX_USART_H_
#define SME_SIGFOX_USART_H_

#include "status_codes.h"

#define MAX_SIGFOX_RX_BUFFER_LENGTH   1

#define MAX_MESSAGE_OUT 6
// keep track of the messageId are active
uint8_t sfxMessageIdx[MAX_MESSAGE_OUT];


void sigFoxInit(void);
status_code_genare_t sigfoxSendMessage(const uint8_t *msg, uint8_t len);
status_code_genare_t sigfoxReceivedMessage(uint8_t *msg, uint8_t len);

#endif /* SME_SIGFOX_USART_H_ */