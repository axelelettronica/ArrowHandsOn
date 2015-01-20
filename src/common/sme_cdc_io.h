/*
 * sme_cdc_io.h
 *
 * Created: 17/01/2015 16:40:47
 *  Author: speirano
 */ 


#ifndef SME_CDC_IO_H_
#define SME_CDC_IO_H_

#include <stdarg.h>


extern uint8_t dbg_buf[];

int sprintf(char *out, const char *format, ...);
int snprintf( char *buf, unsigned int count, const char *format, ... );

struct usart_module cdc_usart;

#define print_internal(__fmt_, ...)   

#define print_out(__fmt_, ...)   
#define print_dbg(__fmt_, ...)   
#define print_err(__fmt_, ...)  
#endif /* SME_CDC_IO_H_ */