/*
 * sme_cdc_cmd_parse.h
 *
 * Created: 11/01/2015 12:45:14
 *  Author: mfontane
 */ 


#ifndef SME_CDC_CMD_PARSE_H_
#define SME_CDC_CMD_PARSE_H_

#include "..\tasks\uart\sme_usart_tx_task.h"
#include "..\tasks\sme_i2c_task.h"
#include "sme_cmn.h"
#include <stdint-gcc.h>

typedef union {
	usartQueueS uart_msg;	// uart_msg
	i2cQueueS   i2c_msg; 	// spi_msg
} cdc_queue_msg_t;



int sme_hex_str_to_data(uint8_t *s, uint8_t *data, uint8_t *datalen);

inline int sme_int_str_to_uint8(uint8_t *s, uint8_t *value)
{
	if(!s) {
		return SME_EINVAL;
	}
	*value = atoi(s);
	
	return SME_OK;
}

inline int sme_hex_str_to_uint8(uint8_t *s, uint8_t *value)
{
	if(!s) {
		return SME_EINVAL;
	}
	*value = (int)strtol(s, NULL,
	((s[0] == '0') && (s[1] == 'x')) ? 0: 16);
	return SME_OK;
}

#endif /* SME_CDC_CMD_PARSE_H_ */