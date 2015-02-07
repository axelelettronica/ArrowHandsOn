/*
 * sme_i2c_parse.h
 *
 * Created: 1/22/2015 10:34:22 PM
 *  Author: mfontane
 */ 


#ifndef SME_I2C_PARSE_H_
#define SME_I2C_PARSE_H_



    /* I.E. i2c <address> [r/w] <register> <data> */
int parseI2CMsg(void);


#endif /* SME_I2C_PARSE_H_ */