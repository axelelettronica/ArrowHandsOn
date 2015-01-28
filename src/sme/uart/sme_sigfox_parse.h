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
* I.E. sf <c/d> [r/w] <register> <data> 
* 
*  
* c = move the sigfox in configuration mode (basically send "+++"
* d = move the sigfox in data mode (basically send "ATS220=23" or "ATX")
*
* CONFIGURATION MODE        sf c r/w <register> <data> 
* Valid only in configuration mode
* r = read register
* w = write register
* register = the register in DEC mode
* data     = the value to write on the register (only in conf w mode)
*
* sending "sf c" (without parameters) move the sigfox in configuration mode
*
*
* DATA MODE         sf d  <type> <data>
* type could be one of the following value 
*	DATA: simple data
*	KEEP: used to send a keep alive message as described in the SIGFOX standard; 
*		  it’s in charge to the user to send the keep alive message every 24 hours
*	BIT: send a status bit following the SIGFOX protocol
*
* data could be one of the following value according with the type assigned
*	DATA: up to 255 bytes
*	KEEP: must be omitted
*	BIT: one byte with value 0 or 1
*/
int parseSigFoxMsg(void **componentStr);


#endif /* SME_SIGFOX_PARSE_H_ */