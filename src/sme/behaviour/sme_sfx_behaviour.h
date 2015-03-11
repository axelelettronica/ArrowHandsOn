/*
 * sme_sfx_behaviour.h
 *
 * Created: 3/1/2015 9:54:24 PM
 *  Author: mfontane
 */ 


#ifndef SME_SFX_BEHAVIOUR_H_
#define SME_SFX_BEHAVIOUR_H_

typedef enum {  // Do not exchange the MSG Orders!!!!!!
    SME_SFX_DEBUG_MSG,  // RESERVED!!
    SME_SFX_I2C_1_MSG   = 1,
    SME_SFX_GPS_1_MSG   = 2,
    SME_SFX_I2C_XL_MSG  = 3,
    SME_SFX_I2C_GYR_MSG = 4,
    SME_SFX_I2C_MAG_MSG = 5,

    /* ADD NEW MESSAGES FROM NOW ON. KEEP THE INCREMENTAL NUNBERING */
    SME_SFX_END_MSG

}sme_sfx_report_t;


void sfxSendExecution(sme_sfx_report_t msg_id);


#endif /* SME_SFX_BEHAVIOUR_H_ */