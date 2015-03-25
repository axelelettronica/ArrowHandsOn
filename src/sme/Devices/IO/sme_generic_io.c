/*
* sme_generic_io.c
*
* Created: 3/7/2015 11:13:33 PM
*  Author: speirano
*/
#include <sme_pin_definition.h>
#include <sme_cdc_io.h>
#include <port.h>

/* Uset to increase the Voltage level in case the board
 * is powered by an external Battery pack
 *
 */
void voltageStepUp(bool enable)
{
    bool pin_state = false;

    if (enable) {
        // Check if it is required to enable step up.
        // This is required if just the battery pack power supply is provided.
        pin_state = port_pin_get_input_level(EXT_POW_PIN_PIN);
        //if (!pin_state) {
        // no further power supply is present, enable STEP_UP
        port_pin_set_output_level(STEP_UP_PIN_POUT, true);
        port_pin_set_output_level(SME_LED_Y2_PIN, SME_LED_Y2_ACTIVE);
        print_gps("Enabling STEP-UP\n");
        
        pin_state = port_pin_get_output_level(STEP_UP_PIN_POUT);
        if (!pin_state) {
            print_gps("Enabling STEP-UP FAILED!\n");
            port_pin_set_output_level(SME_LED_Y1_PIN, SME_LED_Y1_ACTIVE);
        }
        //}
        } else {
        pin_state = port_pin_get_output_level(STEP_UP_PIN_POUT);
        if (pin_state) {
            // Deactivate Step-up
            port_pin_set_output_level(STEP_UP_PIN_POUT, false);
            port_pin_set_output_level(SME_LED_Y2_PIN, SME_LED_Y2_INACTIVE);
            print_gps("Disabling STEP-UP\n");
        }
    }
}