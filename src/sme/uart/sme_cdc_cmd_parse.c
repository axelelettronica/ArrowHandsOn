/*
 * sme_cdc_cmd_parse.c
 *
 * Created: 02/01/2015 14:27:05
 *  Author: speirano
 */ 

#include <sme_cmn.h>
#include <string.h>
#include "sme_cdc_uart.h"
#include "../tasks/sme_i2c_task.h"

typedef union {
	// spi_msg
	// uart_msg
	i2cQueueS i2c_msg;
} cdc_queue_msg_t;


typedef cdc_queue_msg_t* (*cmd_callback) (__VA_ARGS__);

static int cdc_parser_show(void);
static int cdc_parser_help(void);
static int cdc_parser_dbg_i2c(cdc_queue_msg_t *data, xQueueHandle *queue);


typedef enum {
	CDC_0,
	CDC_S,
	CDC_C,
	CDC_8,
	CDC_16,
	CDC_32	
} sme_cdc_param_t;

typedef enum {
	CDC_HELP,
	CDC_SHOW,
	CDC_D_I2C,
	
	CDC_CMD_MAX,
} sme_cdc_cmd_t;


#define CDC_MAX_PARAM_NUM    8

typedef struct {
	sme_cdc_cmd_t cmd;
	uint8_t *cmd_str;
	cmd_callback func;
	uint8_t params_num;
	sme_cdc_param_t params[CDC_MAX_PARAM_NUM];
} cmd_cb_t;


/*****************************************************************************/
/*                              Variable definitions                         */
/*****************************************************************************/



cmd_cb_t  cmd_cb[] = {
	CDC_HELP, "help", cdc_parser_help, 0, {CDC_0, CDC_0, CDC_0, CDC_0, CDC_0, CDC_0, CDC_0, CDC_0},
	CDC_SHOW, "show", cdc_parser_show, 0, {CDC_0, CDC_0, CDC_0, CDC_0, CDC_0, CDC_0, CDC_0, CDC_0},
    /* I.E. i2c <address> [r/w] <register> <datalen> <data> */
    CDC_D_I2C, "i2c" , cdc_parser_dbg_i2c, 0, {CDC_8, CDC_C, CDC_32, CDC_8, CDC_32, CDC_32, CDC_32, CDC_0}
};


cdc_queue_msg_t data;

/*****************************************************************************/
/*                            Utilities functions                            */
/*****************************************************************************/

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


/*****************************************************************************/
/*                          I2C Parsing functions                            */
/*****************************************************************************/

int cdc_parser_help()
{
	return SME_OK;
}

int cdc_parser_show()
{
	return SME_OK;
}

int cdc_parser_dbg_i2c(cdc_queue_msg_t *data, xQueueHandle *queue)
{
	/* I.E. i2c <hex-addressd> [r/w] <hex-register> <len> <hex-data> */
	messageU  *msg = &(data->i2c_msg.command);
	int err = SME_OK;
	
	err |= sme_int_str_to_uint8(sme_cli_msg.token[1], 
                                &(msg->fields.sensorId));
	
	// read operation
	if (sme_cli_msg.token[2][0] == 'r') {
		data->i2c_msg.code = sensorReadRegister;
	} else if (sme_cli_msg.token[2][0] == 'w') {
        data->i2c_msg.code = sensorWriteRegister;
		err |= sme_int_str_to_uint8(sme_cli_msg.token[4], &(msg->fields.data));
	} else {
		// print help
		return SME_EINVAL;
	}

	err |= sme_hex_str_to_uint8(sme_cli_msg.token[3], &(msg->fields.i2cRegister));
	err |= sme_hex_str_to_uint8(sme_cli_msg.token[1], &(msg->fields.sensorId));
	//msg->fields.r_w         = sme_int_str_to_uint8(cdc_msg_buffer[1]);

	if (err) {
		// print help
		return SME_EINVAL;
	}
	
	queue = &i2cCommandQueue;
	return SME_OK;
}


int sme_cdc_cmd_execute(cmd_cb_t *cmd_cb)
{
	xQueueHandle    queue;
    int err = 0;
	
	// Fill queue_msg form msg
	if (!cmd_cb) {
		return SME_EINVAL;
	}
		
	memset(&data,0, sizeof(cdc_queue_msg_t));
	
	if (cmd_cb->func(&data, &queue)) {
			return SME_EINVAL;
	}

	// send to the proper queue
	if (!xQueueSend(queue, (void *)&data, NULL) != pdPASS) {
		// Error: could not enqueue character
		return SME_EBUSY;
   }
	
    return SME_OK;	
}

int
sme_cdc_cmd_parse (uint8_t token_num)
{	
	uint8_t i;
	uint8_t len;
	
	for (i = 0; i < CDC_CMD_MAX; ++i) {
		len = strlen(cmd_cb[i].cmd_str);
		if (!strncmp(cmd_cb[i].cmd_str, sme_cli_msg.token[0], len)) {
			sme_cdc_cmd_execute(&cmd_cb[i]);
			break;
		}
	}	

	return SME_OK;
}