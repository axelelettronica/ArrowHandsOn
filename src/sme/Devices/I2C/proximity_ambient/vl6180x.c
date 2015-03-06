/*
* vl6180x.c
*
* Created: 3/1/2015 10:33:08 PM
*  Author: mfontane

https://github.com/kriswiner/VL6180X/blob/master/VL6180Xbasic.ino
*/

#include <stdbool.h>
#include "..\I2C.h"
#include "vl6180_reg.h"

typedef enum {  // define lower nibble of ALS gain register
    alsGain20 = 0,  // ALS gain of 20
    alsGain10,      // ALS gain of 10.32
    alsGain5,       // ALS gain of 5.21
    alsGain2_5,     // ALS gain of 2.60
    alsGain1_67,    // ALS gain of 1.72
    alsGain1_25,    // ALS gain of 1.28
    alsGain1_0,     // ALS gain of 1.01
    alsGain40       // ALS gain of 40

}ALSGain;

static ALSGain realalsGain,  alsGain = alsGain20;
static void getalsGain() {
    switch (alsGain)
    {
        // Possible ALS gains
        case alsGain20:
        realalsGain = 20.; // get actual ALS gain from nominal index
        break;
        case alsGain10:
        realalsGain = 10.32;
        break;
        case alsGain5:
        realalsGain = 5.21;
        break;
        case alsGain2_5:
        realalsGain = 2.60;
        break;
        case alsGain1_67:
        realalsGain = 1.72;
        break;
        case alsGain1_25:
        realalsGain = 1.28;
        break;
        case alsGain1_0:
        realalsGain = 1.01;
        break;
        case alsGain40:
        realalsGain = 40.;
        break;
    }
}

static bool internal_init(void) {
    uint8_t reset=0, timeOut;
    
    getalsGain();
    while (reset!=1 && timeOut<100) {
        readRegister(VL6180X_ADDRESS, VL6180X_SYSTEM_FRESH_OUT_OF_RESET, &reset);  // read fresh_out_of_reset bit
        
        if(reset == 1) {  // if if fresh_out_of_reset bit set, then device has been freshly initialized
            
            // SR03 settings AN4545 24/27 DocID026571 Rev 19 SR03 settings
            // http://www.st.com/st-web-ui/static/active/en/resource/technical/document/application_note/DM00122600.pdf
            // Below are the recommended settings required to be loaded onto the VL6180X during the
            // initialisation of the device (see Section 1.3).
            // Mandatory : private registers


            bool result = writeRegister_16Bit(VL6180X_ADDRESS, 0x0207, 0x01);
            result &= writeRegister_16Bit(VL6180X_ADDRESS, 0x0208, 0x01);
            result &= writeRegister_16Bit(VL6180X_ADDRESS, 0x0096, 0x00);
            result &= writeRegister_16Bit(VL6180X_ADDRESS, 0x0097, 0xFD);
            result &= writeRegister_16Bit(VL6180X_ADDRESS, 0x00e3, 0x00);
            result &= writeRegister_16Bit(VL6180X_ADDRESS, 0x00e4, 0x04);
            result &= writeRegister_16Bit(VL6180X_ADDRESS, 0x00e5, 0x02);
            result &= writeRegister_16Bit(VL6180X_ADDRESS, 0x00e6, 0x01);
            result &= writeRegister_16Bit(VL6180X_ADDRESS, 0x00e7, 0x03);
            result &= writeRegister_16Bit(VL6180X_ADDRESS, 0x00f5, 0x02);
            result &= writeRegister_16Bit(VL6180X_ADDRESS, 0x00d9, 0x05);
            result &= writeRegister_16Bit(VL6180X_ADDRESS, 0x00db, 0xce);
            result &= writeRegister_16Bit(VL6180X_ADDRESS, 0x00dc, 0x03);
            result &= writeRegister_16Bit(VL6180X_ADDRESS, 0x00dd, 0xf8);
            result &= writeRegister_16Bit(VL6180X_ADDRESS, 0x009f, 0x00);
            result &= writeRegister_16Bit(VL6180X_ADDRESS, 0x00a3, 0x3c);
            result &= writeRegister_16Bit(VL6180X_ADDRESS, 0x00b7, 0x00);
            result &= writeRegister_16Bit(VL6180X_ADDRESS, 0x00bb, 0x3c);
            result &= writeRegister_16Bit(VL6180X_ADDRESS, 0x00b2, 0x09);
            result &= writeRegister_16Bit(VL6180X_ADDRESS, 0x00ca, 0x09);
            result &= writeRegister_16Bit(VL6180X_ADDRESS, 0x0198, 0x01);
            result &= writeRegister_16Bit(VL6180X_ADDRESS, 0x01b0, 0x17);
            result &= writeRegister_16Bit(VL6180X_ADDRESS, 0x01ad, 0x00);
            result &= writeRegister_16Bit(VL6180X_ADDRESS, 0x00ff, 0x05);
            result &= writeRegister_16Bit(VL6180X_ADDRESS, 0x0100, 0x05);
            result &= writeRegister_16Bit(VL6180X_ADDRESS, 0x0199, 0x05);
            result &= writeRegister_16Bit(VL6180X_ADDRESS, 0x01a6, 0x1b);
            result &= writeRegister_16Bit(VL6180X_ADDRESS, 0x01ac, 0x3e);
            result &= writeRegister_16Bit(VL6180X_ADDRESS, 0x01a7, 0x1f);
            result &= writeRegister_16Bit(VL6180X_ADDRESS, 0x0030, 0x00);

            // onfigure range measurement for low power
            // Specify range measurement interval in units of 10 ms from 0 (= 10 ms) - 254 (= 2.55 s)
            
            writeRegister_16Bit(VL6180X_ADDRESS, VL6180X_SYSRANGE_INTERMEASUREMENT_PERIOD, 0x09); // 100 ms interval in steps of 10 ms
            writeRegister_16Bit(VL6180X_ADDRESS, VL6180X_SYSRANGE_VHV_REPEAT_RATE, 0xFF); // sets number of range measurements after which autocalibrate is performed
            writeRegister_16Bit(VL6180X_ADDRESS, VL6180X_SYSRANGE_VHV_RECALIBRATE, 0x01); // perform temperature calibration of the ranging sensor

            // Set Early Convergence Estimate for lower power consumption
            writeRegister_16Bit(VL6180X_ADDRESS, VL6180X_SYSRANGE_MAX_CONVERGENCE_TIME, 0x32); // set max convergence time to 50 ms (steps of 1 ms)
            writeRegister_16Bit(VL6180X_ADDRESS, VL6180X_SYSRANGE_RANGE_CHECK_ENABLES, 0x10 | 0x01);  // enable (0x01) early convergence estimate
            // This ECE is calculated as follows:
            // [(1 - % below threshold) x 0.5 x 15630]/ range max convergence time
            // This is ~123 ms for 50 ms max convergence time and 80% below threshold
            // This is a sixteen bit (2 byte) register with the first byte MSByte and the second LSByte
            writeRegister_16Bit(VL6180X_ADDRESS, VL6180X_SYSRANGE_EARLY_CONVERGENCE_ESTIMATE, 0x00); // set early convergence estimate to 5%
            writeRegister_16Bit(VL6180X_ADDRESS, VL6180X_SYSRANGE_EARLY_CONVERGENCE_ESTIMATE + 1, 0x7B); // set early convergence estimate to 5%

            // Configure ALS
            writeRegister_16Bit(VL6180X_ADDRESS, VL6180X_SYSALS_INTERMEASUREMENT_PERIOD, 0x0A);   // set to 100 ms
            // Following is a 16-bit register with the first MSByte reserved
            
            writeRegister_16Bit(VL6180X_ADDRESS, VL6180X_SYSALS_INTEGRATION_PERIOD, 0x00);
            writeRegister_16Bit(VL6180X_ADDRESS, VL6180X_SYSALS_INTEGRATION_PERIOD+1, 0x63);        // set ALS integration time to 100 ms in steps of 1 ms

            // The internal readout averaging sample period can be adjusted from 0 to 255. Increasing the sampling
            // period decreases noise but also reduces the effective max convergence time and increases power consumption:
            // Effective max convergence time = max convergence time - readout averaging period (see
            // Section 2.5: Range timing). Each unit sample period corresponds to around 64.5 ?s additional
            // processing time. The recommended setting is 48 which equates to around 4.3 ms
            writeRegister_16Bit(VL6180X_ADDRESS, VL6180X_READOUT_AVERAGING_SAMPLE_PERIOD, 0x30);  // compromise between low noise and increased execution time
            // Gain can be 0 = 20, 1 = 10, 2 = 5, 3 = 2.5, 4 = 1.67, 5 = 1.25, 6 = 1.0 and 7 = 40
            // These are value possible for the lower nibble. The upper nibble must be 4
            writeRegister_16Bit(VL6180X_ADDRESS, VL6180X_SYSALS_ANALOGUE_GAIN, 0x40 | alsGain);   // Sets light and dark gain (don't change upper nibble)
            // Scalar (1 - 32, bits 4:0) to multiply raw ALS count for additonal gain, if necessary
            writeRegister_16Bit(VL6180X_ADDRESS, VL6180X_FIRMWARE_RESULT_SCALER, 0x01);

            // Configure the interrupts
            writeRegister_16Bit(VL6180X_ADDRESS, VL6180X_SYSTEM_MODE_GPIO0, 0x00);                // set up GPIO 0 (set to high impedence for now)
            writeRegister_16Bit(VL6180X_ADDRESS, VL6180X_SYSTEM_MODE_GPIO1, 0x00);                // set up GPIO 1 (set to high impedence for now)
            writeRegister_16Bit(VL6180X_ADDRESS, VL6180X_SYSTEM_INTERRUPT_CONFIG_GPIO, 0x24);     // enable sample ready interrupt

            #if 0
            // enable continuous range mode
            if(VL6180XMode == contRangeMode) {
                writeByte(VL6180X_ADDRESS, VL6180X_SYSRANGE_START, 0x03);  // start auto range mode

            }
            // enable continuous ALS mode
            if(VL6180XMode == contALSMode) {
                // Configure ALS
                writeByte(VL6180X_ADDRESS, VL6180X_SYSALS_INTERMEASUREMENT_PERIOD, 0x32);   // set to 100 ms
                // Following is a 16-bit register with the first MSByte reserved
                writeByte(VL6180X_ADDRESS, VL6180X_SYSALS_INTEGRATION_PERIOD+1, 0x32);        // set ALS integration time to 50 ms in steps of 1 ms
                writeByte(VL6180X_ADDRESS, VL6180X_SYSALS_START, 0x03);                     // start auto range mode
            }
            #endif
            // Clear reset bit
            writeRegister_16Bit(VL6180X_ADDRESS, VL6180X_SYSTEM_FRESH_OUT_OF_RESET, 0x00);        // reset fresh_out_of_reset bit to zero
            return result;
            } else {
            timeOut++;
        }
    }
    reset=0;
    return false;
}

bool vl6180x_init(void) {
    uint8_t data=0;
    
    if (readRegister_16Bit(VL6180X_ADDRESS, VL6180X_IDENTIFICATION_MODEL_ID, &data)) {
        if (data == VL6180X_IDENTIFICATION_MODEL_ID_RETURN){
            return internal_init();
        }
    }
    
    return false;
}

/*
Single-shot range/ALS operation
single-shot ALS measurement is initiated by writing 0x01 to the
VL6180X_SYSALS_START register{0x38}.
• When the measurement is completed, bit 5 of
VL6180X_RESULT_INTERRUPT_STATUS_GPIO{0x4F} will be set.
Note that in both cases the start bit, (bit 0) auto-clears.
• The ALS result is read from VL6180X_SYSALS_INTEGRATION_PERIOD{0x50}.
• Interrupt status flags are cleared by writing a ‘1’ to the appropriate bit of
SYSTEM__INTERRUPT_CLEAR{0x15}.
• Bit 0 of RESULT__RANGE_STATUS{0x4D} and RESULT__ALS_STATUS{0x4E}
indicate when either sensor is ready for the next operation.
• Error codes are indicated in bits [7:4] of the status registers
A detailed description of all the user accessible registers is given in Section 6: Device
registers.
Note: Single-shot ALS and range operations cannot be performed simultaneously. Only one of
these operations should be performed at any one time and once started must be allowed to
complete before another measurement is started. This is because any current operation will
be aborted if another is started.
*/
    volatile uint8_t status;
    volatile uint16_t alsRaw;
    volatile uint32_t alsInt;
    volatile uint8_t rawData[2] = {0, 0};
    volatile float als;
bool vl6180x_get_light_polling(char *value) {


    writeRegister_16Bit(VL6180X_ADDRESS, VL6180X_SYSALS_START, START_SINGLE_MODE);

    if (readRegister_16Bit(VL6180X_ADDRESS, VL6180X_RESULT_INTERRUPT_STATUS_GPIO, &status)) {
        status = status & ALS_SINGLE_MODE_MASK;
        while (status != ALS_SINGLE_MODE_READY) {
            if (!readRegister_16Bit(VL6180X_ADDRESS, VL6180X_RESULT_INTERRUPT_STATUS_GPIO, &status)) {
                return false;
            }
            status = status & ALS_SINGLE_MODE_MASK;
        }
        
        if (readBufferRegister_16Bit(VL6180X_ADDRESS, VL6180X_RESULT_INTERRUPT_STATUS_GPIO, rawData, 2)) {
            writeRegister_16Bit(VL6180X_ADDRESS, VL6180X_SYSTEM_INTERRUPT_CLEAR, CLEAR_ALS_INT);
            
            uint16_t alsraw = (uint16_t) (((uint16_t) rawData[0] << 8) | rawData[1]); //get 16-bit als raw value
            als = 0.32f * ((float) alsraw / realalsGain) * (100.0f/100.0f);  // convert to absolute lux
            alsInt = als * 10000;
            value[0] = alsInt>>32 & 0xFF;
            value[1] = (alsInt>>16) & 0xFF;
            value[2] = alsInt>>8& 0xFF;
            value[3] = alsInt & 0xFF;
            
            return true;
        }
        
        return false; //should never arrive here
    }

    return false;
}

/*
Single-shot range/ALS operation
A single-shot range or ALS measurement is performed as follows:
• Write 0x01 to the VL6180X_SYSRANGE__TART register{0x18}.
• When the measurement is completed, bit 2 of
VL6180X_RESULT_INTERRUPT_STATUS_GPIO{0x4F} will be set.
• Similarly, a single-shot ALS measurement is initiated by writing 0x01 to the
VL6180X_SYSALS_START register{0x38}.
• When the measurement is completed, bit 5 of
VL6180X_RESULT_INTERRUPT_STATUS_GPIO{0x4F} will be set.
Note that in both cases the start bit, (bit 0) auto-clears.
• The range result is read from RESULT__RANGE_VAL{0x62}.
• The ALS result is read from VL6180X_SYSALS_INTEGRATION_PERIOD{0x50}.
• Interrupt status flags are cleared by writing a ‘1’ to the appropriate bit of
SYSTEM__INTERRUPT_CLEAR{0x15}.
• Bit 0 of RESULT__RANGE_STATUS{0x4D} and RESULT__ALS_STATUS{0x4E}
indicate when either sensor is ready for the next operation.
• Error codes are indicated in bits [7:4] of the status registers
A detailed description of all the user accessible registers is given in Section 6: Device
registers.
Note: Single-shot ALS and range operations cannot be performed simultaneously. Only one of
these operations should be performed at any one time and once started must be allowed to
complete before another measurement is started. This is because any current operation will
be aborted if another is started.
*/
bool vl6180x_get_range(char *value){
    uint8_t gain=0;
    uint16_t integration_t=0, ligth_value=0;
    
    if (readRegister_16Bit(VL6180X_ADDRESS, VL6180X_SYSALS_ANALOGUE_GAIN, &gain)) {
        if (readBufferRegister_16Bit(VL6180X_ADDRESS, VL6180X_SYSALS_INTEGRATION_PERIOD, &integration_t, 2)){
            if (readBufferRegister_16Bit(VL6180X_ADDRESS, VL6180X_SYSALS_INTEGRATION_PERIOD, &ligth_value, sizeof(integration_t))) {
                return true;
            }
        }
    }
    
    return false;
}