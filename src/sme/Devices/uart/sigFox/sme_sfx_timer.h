/*
 * sme_sfx_timer.h
 *
 * Created: 1/31/2015 3:57:42 PM
 *  Author: mfontane
 */ 


#ifndef SME_SFX_TIMER_H_
#define SME_SFX_TIMER_H_


#define SFX_COMMAND_TIMEOUT  (120000 / portTICK_RATE_MS)
#define SFX_COMMAND_TIMEOUT_ID 1

bool isSfxCommandTimerExpired(void);

void initSfxTimer(void);
void stopSfxCommandTimer(void);
void startSfxCommandTimer(void);

#endif /* SME_SFX_TIMER_H_ */