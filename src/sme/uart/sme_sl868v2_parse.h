/*
 * sme_sl868v2_parse.h
 *
 * Created: 22/01/2015 12:21:59
 *  Author: speirano
 */ 


#ifndef SME_SL868V2_PARSE_H_
#define SME_SL868V2_PARSE_H_



#include "sme_cdc_cmd_parse.h"
#include "../model/sme_model_sl868v2.h"

/*
* I.E. gps <c> <Standard SMEA Sentence> 
* 
*  
* c = raw command to the sl868v2 in Standard SMEA Format excluding Start '$' and msg delimiter '*'
*     SMEA CMD is up to 76 characters long.
*/

#define NMEA_TALKER_LEN       2
#define NMEA_SENTENCE_ID_LEN  3


int parseSl868v2Msg(void);

int getTalkerType(uint8_t *in, sl868v2MsgE *nmeaType);
#endif /* SME_SL868V2_PARSE_H_ */