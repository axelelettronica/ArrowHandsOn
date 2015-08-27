/**
 * \file
 *
 * \brief SAM D21 Xplained Pro board initialization
 *
 * Copyright (c) 2013-2014 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 */

#include <compiler.h>
#include <board.h>
#include <conf_board.h>
#include <port.h>

#if defined(__GNUC__)
void board_init(void) WEAK __attribute__((alias("system_board_init")));
#elif defined(__ICCARM__)
void board_init(void);
#  pragma weak board_init=system_board_init
#endif

void system_board_init(void)
{
	struct port_config pin_conf;
	port_get_config_defaults(&pin_conf);

	/* Configure OUTPUT Pin */
    //first yellow led
	pin_conf.direction  = PORT_PIN_DIR_OUTPUT;
	port_pin_set_config(SME_LED_Y1_PIN, &pin_conf);
	port_pin_set_output_level(SME_LED_Y1_PIN, SME_LED_Y1_INACTIVE);
	
	//second yellow led
	port_pin_set_config(SME_LED_Y2_PIN, &pin_conf);
	port_pin_set_output_level(SME_LED_Y2_PIN, SME_LED_Y2_INACTIVE);
		       
    // reset I_O Extender
    port_pin_set_config(SME_RESET_IOEXT_PIN, &pin_conf);
    port_pin_set_output_level(SME_RESET_IOEXT_PIN, SME_RESET_IOEXT_INACTIVE);
    
    // step up pin
    port_pin_set_config(STEP_UP_PIN_POUT, &pin_conf);
    port_pin_set_output_level(STEP_UP_PIN_POUT, false);

	/* END Configure OUTPUT Pin */    
    
	
    
    
	/* Configure INPUT Pin */
	pin_conf.direction  = PORT_PIN_DIR_INPUT;
	pin_conf.input_pull = PORT_PIN_PULL_UP;
    
	port_pin_set_config(SME_BUTTON1_PIN, &pin_conf);
	port_pin_set_config(SME_BUTTON2_PIN, &pin_conf);
    
    // initializing External Power detection Pin
    port_pin_set_config(EXT_POW_PIN_PIN, &pin_conf);
	/* END Configure INPUT Pin */    
    
    
    
    
    /* Configure INTERRUPT Pin */
    pin_conf.direction  = PORT_PIN_DIR_INPUT;
    pin_conf.input_pull = PORT_PIN_PULL_UP;
    
    port_pin_set_config(SME_INT_IOEXT_HW_PIN, &pin_conf);
    /* END Configure INTERRUPT Pin */   
}
