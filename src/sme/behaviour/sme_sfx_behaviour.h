/*
 * sme_sfx_behaviour.h
 *
 * Created: 3/1/2015 9:54:24 PM
 *  Author: mfontane
 */ 


#ifndef SME_SFX_BEHAVIOUR_H_
#define SME_SFX_BEHAVIOUR_H_



typedef enum {
    SME_SFX_DEBUG_MSG,
    SME_SFX_I2C_1_MSG,
    SME_SFX_GPS_1_MSG
}sme_sfx_report_t;



void sfxSendExecution(sme_sfx_report_t msg_id);


#endif /* SME_SFX_BEHAVIOUR_H_ */