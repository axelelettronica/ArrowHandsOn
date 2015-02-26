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
#include "sme_sl868v2_parse.h"
#include "../model/sme_model_sigfox.h"
#include "../model/sme_model_sl868v2.h"
#include "sme_cdc_io.h"

static char CDC_HELP_DBG[]   ="Help: dbg <verbose dump level>:\r\n\tdbg e|d|s|g: "
"enable errors and/or debugs\r\n\tdbg 0: all disabled\r\n";
static char CDC_HELP_SL868V2[]="Help: gps [c] <Standard NMEA Sentence> (between '$' and '*') ...\r\n";
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
    CDC_SL868V2,
    CDC_CMD_MAX,
} sme_cdc_cmd_t;


typedef int (*cmd_callback) (void);

typedef struct {
    sme_cdc_cmd_t cmd;
    const char *cmd_str;
    cmd_callback func;
} cmd_cb_t;

/*****************************************************************************/
/*                              Functions prototypes                         */
/*****************************************************************************/

static int cdc_parser_show(void);
static int cdc_parser_help(void);
static int cdc_parser_dbg (void);
static int cdc_parser_dbg_i2c(void);
static int cdc_parser_dbg_sl868v2(void);
static int cdc_parser_dbg_sigfox(void);

static int sme_cdc_cmd_execute(cmd_cb_t *cmd_cb);

/*****************************************************************************/
/*                              Variable definitions                         */
/*****************************************************************************/

cmd_cb_t  cmd_cb[] = {
    {CDC_HELP,   "help",  (cmd_callback)cdc_parser_help},
    {CDC_SHOW,   "show",  (cmd_callback)cdc_parser_show},
    /* I.E. dbg <dbg_strng>  */
    {CDC_DBG,     "dbg",  (cmd_callback)cdc_parser_dbg},
    {CDC_D_I2C,   "i2c",  (cmd_callback)cdc_parser_dbg_i2c},
    {CDC_SIGFOX,  "sf" ,  (cmd_callback)cdc_parser_dbg_sigfox},
    {CDC_SL868V2, "gps",  (cmd_callback)cdc_parser_dbg_sl868v2}
};





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

int cdc_parser_help(void)
{
    uint8_t i;

    print_out("Available commands:\r\n");
    for (i=0; i < CDC_CMD_MAX; ++i) {
        print_out("- %s\r\n",cmd_cb[i].cmd_str);
    }
    
    return SME_OK;
}

int cdc_parser_show()
{
    print_out(CDC_HELP_NA);
    return SME_OK;
}

/* dbg  <dbgstring>   (this enable the related verbose dbg level)
* <dbg string>:
*   e  : enable Errors, d  : enable Debug, ed: enable Error & Debug
*   0  : disable all
*/
int cdc_parser_dbg (void)
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
            
            case 's':
            sme_dbg_sfx_enable = !sme_dbg_sfx_enable;
            wrong = false;
            break;

            case 'g':
            sme_dbg_gps_enable = !sme_dbg_gps_enable;
            wrong = false;
            
            case 'm':
            sme_dbg_gps_msg_enable = !sme_dbg_gps_msg_enable;
            wrong = false;
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

int cdc_parser_dbg_i2c(void)
{
    int err = parseI2CMsg();

    return err;
}

/*
* I.E. i2c <hex-addressd> [r/w] <hex-register> <hex-data>
*/
int cdc_parser_dbg_sigfox()
{
    int err = SME_OK;
    
    err  |= parseSigFoxMsg();
    
    if (err) {
        releaseSigFoxModel();
        print_out(CDC_HELP_SIGFOX);
    }
    return err;
}

/*
* I.E. gps [c] <NMEA-string>
*      <NMEA-string> = CMD between '$' and '*'
*/
int cdc_parser_dbg_sl868v2(void)
{
    int err = SME_OK;
    
    err  |= parseSl868v2Msg();
    
    if (err) {
        releaseSl868v2Model();
        print_out(CDC_HELP_SL868V2);
    }
    return err;
}

int sme_cdc_cmd_execute(cmd_cb_t *cmd)
{
    if (!cmd) {
        return SME_EINVAL;
    }

    if (cmd->func()) {
        return SME_EINVAL;
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