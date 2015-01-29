
/*
 * sme_sl868v2_usart.h
 *
 * Created: 21/01/2015 22:41:06
 *  Author: speirano
 */ 


#ifndef SME_SL868V2_USART_H_
#define SME_SL868V2_USART_H_

//#include "status_codes.h"

#define MAX_SL868V2_RX_BUFFER_LENGTH   1

void sl868v2Init(void);
int sl868v2SendMessage(const uint8_t *msg, uint8_t len);
int sl868v2ReceivedMessage(uint8_t *msg, uint8_t len);

#endif /* SME_SL868V2_USART_H_ */