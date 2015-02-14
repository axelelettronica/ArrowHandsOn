/*
 * sme_rgb_led.h
 *
 * Created: 2/7/2015 11:13:50 PM
 *  Author: mfontane
 */ 


#ifndef SME_RGB_LED_H_
#define SME_RGB_LED_H_

/* REFERENCE
#define EXT1_PWM_MODULE           TC6
#define EXT1_PWM_0_CHANNEL        0
#define EXT1_PWM_0_PIN            PIN_PB02E_TC6_WO0
#define EXT1_PWM_0_MUX            MUX_PB02E_TC6_WO0
#define EXT1_PWM_0_PINMUX         PINMUX_PB02E_TC6_WO0
#define EXT1_PWM_1_CHANNEL        1
#define EXT1_PWM_1_PIN            PIN_PB03E_TC6_WO1
#define EXT1_PWM_1_MUX            MUX_PB03E_TC6_WO1
#define EXT1_PWM_1_PINMUX         PINMUX_PB03E_TC6_WO1
*/
#define PWM_R_MODULE  TCC0 
#define PWM_R_OUT_PIN PIN_PB10F_TCC0_WO4
#define PWM_R_OUT_MUX MUX_PB10F_TCC0_WO4
#define CC_R_REGISTER 0
#define WO_R_REGISTER 4


#define PWM_G_MODULE  TCC0 
#define PWM_G_OUT_PIN PIN_PB11F_TCC0_WO5
#define PWM_G_OUT_MUX MUX_PB11F_TCC0_WO5
#define CC_G_REGISTER 1
#define WO_G_REGISTER 5


#define PWM_B_MODULE  TCC0
#define PWM_B_OUT_PIN PIN_PA12F_TCC0_WO6
#define PWM_B_OUT_MUX MUX_PA12F_TCC0_WO6
#define CC_B_REGISTER 2
#define WO_B_REGISTER 6

void sme_led_rgb_init(void);

void sme_led_red_on(void);void sme_led_green_on(void);void sme_led_blu_on(void);

void sme_led_red_off(void);void sme_led_green_off(void);void sme_led_blu_off(void);

void sme_brigthness_led_red(uint32_t brigthness);void sme_brigthness_led_green(uint32_t brigthness);void sme_brigthness_led_blu(uint32_t brigthness); #endif