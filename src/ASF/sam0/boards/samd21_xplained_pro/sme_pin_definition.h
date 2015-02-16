/*
 * sme_pin_definition.h
 *
 * Created: 2/14/2015 7:09:37 PM
 *  Author: mfontane
 */ 


#ifndef SME_PIN_DEFINITION_H_
#define SME_PIN_DEFINITION_H_

/** \name Embedded debugger I2C interface definitions
 * @{
 */
#ifdef SMARTEVERYTHING
#define SME_I2C_MODULE              SERCOM3
#define SME_I2C_SERCOM_PINMUX_PAD0  PINMUX_PA22C_SERCOM3_PAD0
#define SME_I2C_SERCOM_PINMUX_PAD1  PINMUX_PA23C_SERCOM3_PAD1
#define SME_I2C_SERCOM_DMAC_ID_TX   SERCOM3_DMAC_ID_TX
#define SME_I2C_SERCOM_DMAC_ID_RX   SERCOM3_DMAC_ID_RX
#else
#define SME_I2C_MODULE              SERCOM5
#define SME_I2C_SERCOM_PINMUX_PAD0  PINMUX_PB16C_SERCOM5_PAD0
#define SME_I2C_SERCOM_PINMUX_PAD1  PINMUX_PB17C_SERCOM5_PAD1
#define SME_I2C_SERCOM_DMAC_ID_TX   SERCOM5_DMAC_ID_TX
#define SME_I2C_SERCOM_DMAC_ID_RX   SERCOM5_DMAC_ID_RX
#endif
/** @} */


/** \name Embedded debugger CDC Gateway USART interface definitions
 * @{
 */
#ifdef SMARTEVERYTHING
#define SME_CDC_MODULE              SERCOM0
#define SME_CDC_SERCOM_MUX_SETTING  USART_RX_1_TX_0_XCK_1
#define SME_CDC_SERCOM_PINMUX_PAD0  PINMUX_PA10C_SERCOM0_PAD2
#define SME_CDC_SERCOM_PINMUX_PAD1  PINMUX_PA11C_SERCOM0_PAD3
#define SME_CDC_SERCOM_PINMUX_PAD2  PINMUX_UNUSED
#define SME_CDC_SERCOM_PINMUX_PAD3  PINMUX_UNUSED
#define SME_CDC_SERCOM_DMAC_ID_TX   SERCOM0_DMAC_ID_TX
#define SME_CDC_SERCOM_DMAC_ID_RX   SERCOM0_DMAC_ID_RX
#else
#define SME_CDC_MODULE              SERCOM3
#define SME_CDC_SERCOM_MUX_SETTING  USART_RX_1_TX_0_XCK_1
#define SME_CDC_SERCOM_PINMUX_PAD0  PINMUX_PA22C_SERCOM3_PAD0
#define SME_CDC_SERCOM_PINMUX_PAD1  PINMUX_PA23C_SERCOM3_PAD1
#define SME_CDC_SERCOM_PINMUX_PAD2  PINMUX_UNUSED
#define SME_CDC_SERCOM_PINMUX_PAD3  PINMUX_UNUSED
#define SME_CDC_SERCOM_DMAC_ID_TX   SERCOM3_DMAC_ID_TX
#define SME_CDC_SERCOM_DMAC_ID_RX   SERCOM3_DMAC_ID_RX
#endif
#define SME_CDC_SIGFOX_BAUDRATE 115200
/** @} */

/** \name Embedded SIGFOX UART interface definitions
 * @{
 */
#define SME_SIGFOX_SERCOM_MUX_SETTING  USART_RX_1_TX_0_XCK_1
#define SME_SIGFOX_MODULE              SERCOM4
#define SME_SIGFOX_SERCOM_PINMUX_PAD0  PINMUX_PB12C_SERCOM4_PAD0
#define SME_SIGFOX_SERCOM_PINMUX_PAD1  PINMUX_PB13C_SERCOM4_PAD1
#define SME_SIGFOX_SERCOM_PINMUX_PAD2  PINMUX_PB14C_SERCOM4_PAD2
#define SME_SIGFOX_SERCOM_PINMUX_PAD3  PINMUX_PB15C_SERCOM4_PAD3
#define SME_SIGFOX_SERCOM_DMAC_ID_TX   SERCOM4_DMAC_ID_TX
#define SME_SIGFOX_SERCOM_DMAC_ID_RX   SERCOM4_DMAC_ID_RX

#define SME_SIGFOX_BAUDRATE			   19200
/** @} */


/** \name Embedded GPS UART interface definitions
 * @{
 *
#define SME_GPS_MODULE              SERCOM1
#define SME_GPS_SERCOM_MUX_SETTING  USART_RX_1_TX_0_XCK_1
#define SME_GPS_SERCOM_PINMUX_PAD0  PINMUX_PA16C_SERCOM1_PAD0
#define SME_GPS_SERCOM_PINMUX_PAD1  PINMUX_PA17C_SERCOM1_PAD1
#define SME_GPS_SERCOM_PINMUX_PAD2  PINMUX_UNUSED
#define SME_GPS_SERCOM_PINMUX_PAD3  PINMUX_UNUSED
#define SME_GPS_SERCOM_DMAC_ID_TX   SERCOM1_DMAC_ID_TX
#define SME_GPS_SERCOM_DMAC_ID_RX   SERCOM1_DMAC_ID_RX
#define SME_GPS_BAUDRATE		    9600
/ @} */



/** \name Embedded RGB I/O interface definitions
 * @{
 */
 #define SME_PWM_RGB_MODULE  TCC0

#define SME_PWM_R_OUT_PIN PIN_PB10F_TCC0_WO4
#define SME_PWM_R_OUT_MUX MUX_PB10F_TCC0_WO4
#define SME_CC_R_REGISTER 0
#define SME_WO_R_REGISTER 4

#define SME_PWM_G_OUT_PIN PIN_PB11F_TCC0_WO5
#define SME_PWM_G_OUT_MUX MUX_PB11F_TCC0_WO5
#define SME_CC_G_REGISTER 1
#define SME_WO_G_REGISTER 5

#define SME_PWM_B_OUT_PIN PIN_PA12F_TCC0_WO6
#define SME_PWM_B_OUT_MUX MUX_PA12F_TCC0_WO6
#define SME_CC_B_REGISTER 2
#define SME_WO_B_REGISTER 6
/* @} */


#endif /* SME_PIN_DEFINITION_H_ */