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


typedef int(*cmd_callback) (__VA_ARGS__);

static int cdc_parser_show();
static int cdc_parser_help();

cdc_queue_msg_t data;

typedef enum {
	CDC_0,
	CDC_S,
	CDC_8,
	CDC_16,
	CDC_32	
} sme_cdc_param_t;

typedef enum {
	CDC_HELP,
	CDC_SHOW,
	
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


cmd_cb_t  cmd_cb[] = {
	CDC_HELP, "help", cdc_parser_help, 0, {CDC_0, CDC_0, CDC_0, CDC_0, CDC_0, CDC_0, CDC_0, CDC_0},
	CDC_SHOW, "show", cdc_parser_show, 0, {CDC_0, CDC_0, CDC_0, CDC_0, CDC_0, CDC_0, CDC_0, CDC_0}
	};

int cdc_parser_help()
{
	return 0;
}

int cdc_parser_show()
{
	return 0;
}

int
sme_cdc_msg_parse (cmd_cb_t *cmd_cb, cdc_queue_msg_t *data, xQueueHandle *queue)
{
    if (!data || !cmd_cb || !queue) {
		return 1;
	}
	
	// Just this i2c for now
	data->i2c_msg.code = allSensorsReadValue;
	
	queue = &i2cCommandQueue;
	return 0;
}


int sme_cdc_cmd_execute(cmd_cb_t *cmd_cb)
{
	xQueueHandle    queue;
    int err = 0;
	
	memset(&data,0, sizeof(cdc_queue_msg_t));
	// Fill queue_msg form msg 
    if (!cmd_cb) {
		return 1;
	}
	
	if(sme_cdc_msg_parse(cmd_cb, (void*)&data, &queue)) {
		return 1;
	}
	
	// send to the proper queue
	if (!xQueueSend(i2cCommandQueue, (void *)&data, NULL) != pdPASS) {
		// Error: could not enqueue character
		return 1;
   }
	
    return 0;	
}

int
sme_cdc_cmd_parse (uint8_t token_num)
{	
	uint8_t i;
	uint8_t len;
	
	for (i = 0; i < CDC_CMD_MAX; ++i) {
		len = strlen(cmd_cb[i].cmd_str);
		if (!strncmp(cmd_cb[i].cmd_str, cdc_msg_buffer[0], len)) {
			sme_cdc_cmd_execute(&cmd_cb[i]);
			break;
		}
	}	

	return 0;
}