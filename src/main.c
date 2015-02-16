/**
* \file
*
* \brief FreeRTOS demo application main function.
*
* Copyright (C) 2013 Atmel Corporation. All rights reserved.
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

#include <asf.h>
#include <sme_cmn.h>
#include "sme/sme_FreeRTOS.h"
#include "sme/tasks/sme_controller.h"
#include "sme/interrupt/interrupt.h"
#include "sme/tasks/sme_sigfox_task.h"
#include "sme/tasks/sme_gps_task.h"
#include "sme/interrupt/interruptHandle.h"
#include "sme/model/sme_model_i2c.h"
#include "sme/Devices/uart/sigFox/sme_sigfox_usart.h"
#include "sme/Devices/uart/gps/sme_sl868v2_usart.h"
#include "sme/model/sme_model_sigfox.h"
#include "sme/model/sme_model_sl868v2.h"
#include "sme/Devices/IO/sme_rgb_led.h"


static void init_services(void);


//! Handle for terminal output task
//static xTaskHandle terminal_task_handle;



/**
* \brief Initialize tasks and resources for demo
*
* This function initializes the \ref oled1_xpro_io_group instance and the
* \ref edbg_cdc_rx_group instance for reception, then creates all
* the objects for FreeRTOS to run the demo.
*/
static void init_services(void)
{
    sme_cdc_mgr_init();
    sme_i2c_mgr_init();
    sme_init_isr_global();
    sme_sigfox_rx_mgr_init();
    sme_gps_mgr_init();

    //at the end the controller
    sme_ctrl_init();


    interruptValueInitializzation();
}

static void componentInit(void) {
    sigFoxInit();
    sl868v2Init();
    initSigFoxModel();
    initSl868v2Model();

    sme_led_rgb_init();
}


int main (void)//
{
    system_init(); 
    
    // init Hw components as first
    componentInit();

    // than all the tasks
    init_services();

    
    // ..and let FreeRTOS run tasks!
    vTaskStartScheduler();

    do {
        // Intentionally left empty
    } while (true);
}
