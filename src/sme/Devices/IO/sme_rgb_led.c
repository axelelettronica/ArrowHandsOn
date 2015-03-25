/*
* sme_rgb_led.c
*
* Created: 2/7/2015 11:13:33 PM
*  Author: smkk
*/
#include "tcc.h"
#include "sme_rgb_led.h"
#include <sme_pin_definition.h>
struct tcc_module  tcc_rgb_instance;typedef struct {    uint8_t led_r_init:1;    uint8_t led_g_init:1;    uint8_t led_b_init:1;}init_led_t;static init_led_t leds_initilized;


void sme_led_rgb_init(void) {
    struct tcc_config config_tcc;
    tcc_get_config_defaults(&config_tcc, SME_PWM_RGB_MODULE);

    // configure the counter width, wave generation mode, and the compare channel 0 value.
    config_tcc.counter.period = 0xffff;
    config_tcc.compare.wave_generation = TCC_WAVE_GENERATION_SINGLE_SLOPE_PWM;


    
    // Initialize BLUE LED
    config_tcc.compare.match[SME_CC_B_REGISTER] = 0xFFFF;
    //configure the PWM output on a physical device RED pin.
    config_tcc.pins.enable_wave_out_pin[SME_WO_B_REGISTER] = true;
    config_tcc.pins.wave_out_pin[SME_WO_B_REGISTER] = SME_PWM_B_OUT_PIN;
    config_tcc.pins.wave_out_pin_mux[SME_WO_B_REGISTER] = SME_PWM_B_OUT_MUX;

    // Initialize GREEN LED
    config_tcc.compare.match[SME_CC_G_REGISTER] = 0xFFFF;
    //configure the PWM output on a physical device RED pin.
    config_tcc.pins.enable_wave_out_pin[SME_WO_G_REGISTER] = true;
    config_tcc.pins.wave_out_pin[SME_WO_G_REGISTER] = SME_PWM_G_OUT_PIN;
    config_tcc.pins.wave_out_pin_mux[SME_WO_G_REGISTER] = SME_PWM_G_OUT_MUX;

   // Initialize RED LED
    config_tcc.compare.match[SME_CC_R_REGISTER] = 0xFFFF;
    //configure the PWM output on a physical device RED pin.
    config_tcc.pins.enable_wave_out_pin[SME_WO_R_REGISTER] = true;
    config_tcc.pins.wave_out_pin[SME_WO_R_REGISTER] = SME_PWM_R_OUT_PIN;
    config_tcc.pins.wave_out_pin_mux[SME_WO_R_REGISTER] = SME_PWM_R_OUT_MUX;
    
    //Configure the RGB TCC module with the desired settings.
    tcc_init(&tcc_rgb_instance, SME_PWM_RGB_MODULE, &config_tcc);
    
    leds_initilized.led_r_init=1;
    leds_initilized.led_g_init=1;
    leds_initilized.led_b_init=1;    // enable RGB    tcc_enable(&tcc_rgb_instance);}void sme_led_red_on(void){    //ligth the RED led at maximun level.
    if(leds_initilized.led_r_init)
    tcc_set_compare_value(&tcc_rgb_instance, SME_CC_R_REGISTER, 0);}void sme_led_green_on(void){    //ligth the GREEN led at maximun level
    if(leds_initilized.led_g_init)
    tcc_set_compare_value(&tcc_rgb_instance, SME_CC_G_REGISTER, 0);
    }void sme_led_blue_on(void){    //ligth the BLUE led at maximun level
    if(leds_initilized.led_b_init)
    tcc_set_compare_value(&tcc_rgb_instance, SME_CC_B_REGISTER, 0);
}void sme_led_red_off(void) {    // switch off the RED led    if(leds_initilized.led_r_init)
    tcc_set_compare_value(&tcc_rgb_instance, SME_CC_R_REGISTER, 0xFFFF);}void sme_led_green_off(void){    // switch off the GREEN led    if(leds_initilized.led_g_init)
    tcc_set_compare_value(&tcc_rgb_instance, SME_CC_G_REGISTER, 0xFFFF);}void sme_led_blue_off(void){    // switch off the BLUE led    if(leds_initilized.led_b_init)
    tcc_set_compare_value(&tcc_rgb_instance, SME_CC_B_REGISTER, 0xFFFF);}
void sme_led_rgb_off(void){    sme_led_green_off();    sme_led_red_off();    sme_led_green_off();    sme_led_blue_off();}void sme_led_red_brightness(uint32_t brigthness) {    // ligth on a desired intensity the RED led    if(leds_initilized.led_r_init)
    tcc_set_compare_value(&tcc_rgb_instance, SME_CC_R_REGISTER, brigthness);}void sme_led_green_brightness(uint32_t brigthness) {    // ligth on a desired intensity the RED led    if(leds_initilized.led_g_init)
    tcc_set_compare_value(&tcc_rgb_instance, SME_CC_G_REGISTER, brigthness);}void sme_led_blue_brightness(uint32_t brigthness) {    // ligth on a desired intensity the RED led    if(leds_initilized.led_b_init)
    tcc_set_compare_value(&tcc_rgb_instance, SME_CC_B_REGISTER, brigthness);}
