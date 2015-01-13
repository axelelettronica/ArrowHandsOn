/*
 * sme_sigfox_parse.h
 *
 * Created: 11/01/2015 12:21:59
 *  Author: mfontane
 */ 


#ifndef SME_SIGFOX_PARSE_H_
#define SME_SIGFOX_PARSE_H_



#include "sme_cdc_cmd_parse.h"

/*
* I.E. sf <c/d> <register> <data> 
* 
* c = move the sigfox in configuration mode (basically send "+++"
* d = move the sigfox in data mode (basically send "ATS220=23" or "ATX")
*/
int parseSigFoxMsg(void **componentStr);


#endif /* SME_SIGFOX_PARSE_H_ */