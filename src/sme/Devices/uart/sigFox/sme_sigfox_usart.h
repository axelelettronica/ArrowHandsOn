
/*
 * sme_sigfox_usart.h
 *
 * Created: 10/01/2015 22:41:06
 *  Author: mfontane
 */ 


#ifndef SME_SIGFOX_USART_H_
#define SME_SIGFOX_USART_H_

#include "status_codes.h"

#define MAX_RX_BUFFER_LENGTH   5

void sigFoxInit(void);
status_code_genare_t sigfoxSendMessage(uint8_t *msg, uint8_t len);
status_code_genare_t sigfoxReceivedMessage(uint8_t *msg, uint8_t len);

#endif /* SME_SIGFOX_USART_H_ */