/*
 * sme_rgb_led.h
 *
 * Created: 2/7/2015 11:13:50 PM
 *  Author: smkk
 */ 


#ifndef SME_RGB_LED_H_
#define SME_RGB_LED_H_


#define HALF_LIGTH (0xFFFF / 2)
#define QUARTER_LIGTH (0xFFFF/4)
#define SIXTEEN_LIGTH (0xFFFF/164)

void sme_led_rgb_init(void);
void sme_led_rgb_off(void);

void sme_led_red_on(void);void sme_led_green_on(void);void sme_led_blue_on(void);

void sme_led_red_off(void);void sme_led_green_off(void);void sme_led_blue_off(void);

void sme_led_red_brightness(uint32_t brigthness);void sme_led_green_brightness(uint32_t brigthness);void sme_led_blue_brightness(uint32_t brigthness); #endif
