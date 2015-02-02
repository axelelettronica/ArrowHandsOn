/*
 * sme_sl868v2_parse.h
 *
 * Created: 22/01/2015 12:21:59
 *  Author: speirano
 */ 


#ifndef SME_SL868V2_PARSE_H_
#define SME_SL868V2_PARSE_H_

/*
* I.E. gps <c> <Standard SMEA Sentence> 
* 
*  
* c = raw command to the sl868v2 in Standard SMEA Format excluding Start '$' and msg delimiter '*'
*     SMEA CMD is up to 76 characters long.
*/
int parseSl868v2Msg(void);


#endif /* SME_SL868V2_PARSE_H_ */