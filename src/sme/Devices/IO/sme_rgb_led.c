/*
* sme_rgb_led.c
*
* Created: 2/7/2015 11:13:33 PM
*  Author: mfontane
*/
#include "tcc.h"
#include "sme_rgb_led.h"
struct tcc_module  tcc_rgb_instance;typedef struct {    uint8_t led_r_init:1;    uint8_t led_g_init:1;    uint8_t led_b_init:1;}init_led_t;static init_led_t leds_initilized;


void sme_led_rgb_init(void) {
    struct tcc_config config_tcc;
    tcc_get_config_defaults(&config_tcc, PWM_B_MODULE);

    // configure the counter width, wave generation mode, and the compare channel 0 value.
    config_tcc.counter.period = 0xffff;
    config_tcc.compare.wave_generation = TCC_WAVE_GENERATION_SINGLE_SLOPE_PWM;


    // Initialize BLUE LED
    config_tcc.compare.match[CC_B_REGISTER] = 0;
    //configure the PWM output on a physical device RED pin.
    config_tcc.pins.enable_wave_out_pin[WO_B_REGISTER] = true;
    config_tcc.pins.wave_out_pin[WO_B_REGISTER] = PWM_B_OUT_PIN;
    config_tcc.pins.wave_out_pin_mux[WO_B_REGISTER] = PWM_B_OUT_MUX;

    // Initialize GREEN LED
    config_tcc.compare.match[CC_G_REGISTER] = 0;
    //configure the PWM output on a physical device RED pin.
    config_tcc.pins.enable_wave_out_pin[WO_G_REGISTER] = true;
    config_tcc.pins.wave_out_pin[WO_G_REGISTER] = PWM_G_OUT_PIN;
    config_tcc.pins.wave_out_pin_mux[WO_G_REGISTER] = PWM_G_OUT_MUX;

    // Initialize RED LED
    config_tcc.compare.match[CC_R_REGISTER] = 0;
    //configure the PWM output on a physical device RED pin.
    config_tcc.pins.enable_wave_out_pin[WO_R_REGISTER] = true;
    config_tcc.pins.wave_out_pin[WO_R_REGISTER] = PWM_R_OUT_PIN;
    config_tcc.pins.wave_out_pin_mux[WO_R_REGISTER] = PWM_R_OUT_MUX;
    
    //Configure the RGB TCC module with the desired settings.
    tcc_init(&tcc_rgb_instance, PWM_B_MODULE, &config_tcc);
   
    leds_initilized.led_b_init=1;
    leds_initilized.led_r_init=1;
    leds_initilized.led_g_init=1;    // enable RGB    tcc_enable(&tcc_rgb_instance);}void sme_led_red_on(void){    //Enable the RED TC module to start the timer and begin PWM signal generation.
    if(leds_initilized.led_r_init)
    tcc_set_compare_value(&tcc_rgb_instance, CC_R_REGISTER, 0xffff);}void sme_led_green_on(void){    //Enable the GREEN TC module to start the timer and begin PWM signal generation.
      if(leds_initilized.led_g_init)
      tcc_set_compare_value(&tcc_rgb_instance, CC_G_REGISTER, 0xffff);
    }void sme_led_blu_on(void){    //Enable the BLU TC module to start the timer and begin PWM signal generation.
     if(leds_initilized.led_b_init)
     tcc_set_compare_value(&tcc_rgb_instance, CC_B_REGISTER, 0xffff);
}void sme_led_red_off(void) {    if(leds_initilized.led_r_init)
    tcc_set_compare_value(&tcc_rgb_instance, CC_R_REGISTER, 0);}void sme_led_green_off(void){    if(leds_initilized.led_g_init)
     tcc_set_compare_value(&tcc_rgb_instance, CC_G_REGISTER, 0);}void sme_led_blu_off(void){    if(leds_initilized.led_b_init)
     tcc_set_compare_value(&tcc_rgb_instance, CC_B_REGISTER, 0);}void sme_brigthness_led_red(uint32_t brigthness){    if(leds_initilized.led_r_init)
    tcc_set_compare_value(&tcc_rgb_instance, CC_R_REGISTER, brigthness);}void sme_brigthness_led_green(uint32_t brigthness){ if(leds_initilized.led_r_init)
    tcc_set_compare_value(&tcc_rgb_instance, CC_G_REGISTER, brigthness);}void sme_brigthness_led_blu(uint32_t brigthness){ if(leds_initilized.led_r_init)
    tcc_set_compare_value(&tcc_rgb_instance, CC_B_REGISTER, brigthness);}