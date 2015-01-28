/*
* sme_cdc_cmd_parse.c
*
* Created: 02/01/2015 14:27:05
*  Author: speirano
*/

#include <sme_cmn.h>
#include <string.h>
#include "sme_cdc_uart.h"
#include "../sme_FreeRTOS.h"
#include "sme_cdc_cmd_parse.h"
#include "sme_sigfox_parse.h"
#include "../model/sme_model_i2c.h"
#include "sme_i2c_parse.h"
#include "../model/sme_model_sigfox.h"

static char CDC_HELP_DBG[]   ="Help: dbg <verbose dump level>:\r\n\tdbg e|d: "
"enable errors and/or debugs\r\n\tdbg 0: all disabled\r\n";
static char CDC_HELP_I2C[]   ="Help: i2c <hex-addressd> [r/w] <hex-register> <hex-data>\r\n";
static char CDC_HELP_SIGFOX[]="Help: sf <c/d> [r/w] <register> <data> \r\n";
static char CDC_HELP_NA[]="TBD\n";

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
    CDC_DBG,
    CDC_D_I2C,
    CDC_SIGFOX,
    CDC_CMD_MAX,
} sme_cdc_cmd_t;


typedef int (*cmd_callback) (cdc_queue_msg_t *data, ...);

typedef struct {
    sme_cdc_cmd_t cmd;
    const char *cmd_str;
    cmd_callback func;
} cmd_cb_t;

/*****************************************************************************/
/*                              Functions prototypes                         */
/*****************************************************************************/

static int cdc_parser_show(cdc_queue_msg_t *data, xQueueHandle *queue);
static int cdc_parser_help(cdc_queue_msg_t *data);
static int cdc_parser_dbg (cdc_queue_msg_t *data, xQueueHandle *queue);
static int cdc_parser_dbg_i2c(cdc_queue_msg_t *data, xQueueHandle *queue);
static int cdc_parser_dbg_sigfox(cdc_queue_msg_t *data, xQueueHandle *queue);
static int sme_cdc_cmd_execute(cmd_cb_t *cmd_cb);

/*****************************************************************************/
/*                              Variable definitions                         */
/*****************************************************************************/

cmd_cb_t  cmd_cb[] = {
    {CDC_HELP,   "help",  (cmd_callback)cdc_parser_help},
    {CDC_SHOW,   "show",  (cmd_callback)cdc_parser_show},
    /* I.E. dbg <dbg_strng>  */
    {CDC_DBG,    "dbg" ,  (cmd_callback)cdc_parser_dbg},
    {CDC_D_I2C,  "i2c" ,  (cmd_callback)cdc_parser_dbg_i2c},
    {CDC_SIGFOX,  "sf" ,  (cmd_callback)cdc_parser_dbg_sigfox}
};


cdc_queue_msg_t cdc_q_buffer;


/*****************************************************************************/
/*                            Utilities functions                            */
/*****************************************************************************/

int sme_hex_str_to_data(uint8_t *s, uint8_t *data, uint8_t *datalen)
{   uint8_t substr[3];
    uint8_t i = 0;
    uint8_t j = 0;
    if(!s) {
        return SME_EINVAL;
    }
    
    substr[0] = substr[1] = substr[2] = 0;
    while (*s) {
        substr[j] = *s;
        ++j;
        if (i && !((i+1)%2)) {
            *data = (int)strtol((char *)substr, NULL,
            ((substr[0] == '0') && (substr[1] == 'x')) ? 0: 16);
            ++data;
            ++(*datalen);
            if(*datalen >= (SME_CDC_MAX_DATA_LEN-1)) {
                return SME_EINVAL;
            }
            j = 0;
            substr[0] = substr[1] = 0;
        }
        s++;
        i++;
    }

    return SME_OK;
}


/*****************************************************************************/
/*                          I2C Parsing functions                            */
/*****************************************************************************/

int cdc_parser_help(cdc_queue_msg_t *data)
{
    uint8_t i;

    print_out("Available commands:\n");
    for (i=0; i < CDC_CMD_MAX; ++i) {
        print_out("- %s\n",cmd_cb[i].cmd_str);
    }
    
    return SME_OK;
}

int cdc_parser_show(cdc_queue_msg_t *data, xQueueHandle *queue)
{
    print_out(CDC_HELP_NA);
    return SME_OK;
}

/* dbg  <dbgstring>   (this enable the related verbose dbg level)
* <dbg string>:
*   e  : enable Errors, d  : enable Debug, ed: enable Error & Debug
*   0  : disable all
*/
int cdc_parser_dbg (cdc_queue_msg_t *data, xQueueHandle *queue)
{
    int i = 0;
    bool wrong = true;
    while (sme_cli_msg.token[1][i]) {
        switch(sme_cli_msg.token[1][i]) {
            case 'e':
            wrong = false;
            break;
            case 'd':
            wrong = false;
            break;
            case '0':
            return SME_OK;
            break;
            default:
            break;
        }
        i++;
    }
    if (wrong)
    print_out(CDC_HELP_DBG);

    return SME_OK;
}

int cdc_parser_dbg_i2c(cdc_queue_msg_t *data, xQueueHandle *queue)
{
    int err = parseI2CMsg(data);

    if (err != SME_OK) {
        // print help
        print_out(CDC_HELP_I2C);
        return SME_EINVAL;
    }

    // do not send to task anymore
    *queue = NULL;
    return SME_OK;
}

/*
* I.E. i2c <hex-addressd> [r/w] <hex-register> <hex-data>
*/
int cdc_parser_dbg_sigfox(cdc_queue_msg_t *data, xQueueHandle *queue)
{
    int err = SME_OK;
    *queue = usartCommandQueue;
    data->uart_msg.code = sigFox;
    
    err  |= parseSigFoxMsg(&data->uart_msg.componentStruct);
    
    if (err) {
        releaseSigFoxModel();
        print_out(CDC_HELP_SIGFOX);
    }
    return err;
}

int sme_cdc_cmd_execute(cmd_cb_t *cmd)
{
    xQueueHandle    queue;
    
    // Fill queue_msg form msg
    if (!cmd) {
        return SME_EINVAL;
    }
    
    memset(&cdc_q_buffer,0, sizeof(cdc_queue_msg_t));
    
    if (cmd->func(&cdc_q_buffer, &queue)) {
        return SME_EINVAL;
    }

    if (queue != NULL) {
        // send to the proper queue
        if (!xQueueSend(queue, (void *)&cdc_q_buffer, (int)NULL) != pdPASS) {
            // Error: could not enqueue character
            return SME_EBUSY;
        }
    }
    
    return SME_OK;
}

int sme_cdc_cmd_parse (uint8_t token_num)
{
    uint8_t i;
    uint8_t len;
    
    for (i = 0; i < CDC_CMD_MAX; ++i) {
        len = strlen((char *)cmd_cb[i].cmd_str);
        if (!strncmp((char *)cmd_cb[i].cmd_str,
        (char *)sme_cli_msg.token[0], len)) {
            sme_cdc_cmd_execute(&cmd_cb[i]);
            break;
        }
    }

    return SME_OK;
}