/*
 * sme_cdc_io.h
 *
 * Created: 17/01/2015 16:40:47
 *  Author: speirano
 */ 


#ifndef SME_CDC_IO_H_
#define SME_CDC_IO_H_

#include <stdarg.h>
#include <usart.h>
#include <string.h>

struct usart_module cdc_usart;

/* Debug */

extern bool sme_dbg_enable;
extern bool sme_err_enable;
extern bool sme_dbg_sfx_enable;
extern bool sme_dbg_gps_enable;
extern bool sme_dbg_gps_msg_enable; //could be removed 
extern uint8_t dbg_buf[];

#define print_internal(__fmt_, ...)  ({sprintf((char *)dbg_buf, __fmt_, ##__VA_ARGS__) ;  \
                                       usart_write_buffer_wait(&cdc_usart, dbg_buf, strlen((char *)dbg_buf)); })

#define print_out(__fmt_, ...)   print_internal(__fmt_, ##__VA_ARGS__)
#define print_dbg(__fmt_, ...)   ({if(sme_dbg_enable) print_internal(__fmt_, ##__VA_ARGS__);})
#define print_err(__fmt_, ...)   ({if(sme_err_enable) print_internal(__fmt_, ##__VA_ARGS__);})
#define print_sfx(__fmt_, ...)     ({if(sme_dbg_sfx_enable) print_internal(__fmt_, ##__VA_ARGS__);})
#define print_gps(__fmt_, ...)     ({if(sme_dbg_gps_enable) print_internal(__fmt_, ##__VA_ARGS__);})
#define print_gps_msg(__fmt_, ...)     ({if(sme_dbg_gps_msg_enable) print_internal(__fmt_, ##__VA_ARGS__);})

#endif /* SME_CDC_IO_H_ */