/*
 * sme_sigfox_execute.c
 *
 * Created: 12/01/2015 20:43:25
 *  Author: mfontane
 */ 
#include "sme_sigfox_execute.h"
#include "port.h"
#include "sme\Devices\Uart\sigFox\sme_sigfox_usart.h"
#include "samd21_xplained_pro.h"


// here just for quick Test 
static uint8_t confModeMsg[] = "+++";
static uint8_t sigFoxModeMsg[] = "ATX";


static void sendSigFoxCommand(uint8_t *msg, uint8_t len) {
	port_pin_toggle_output_level(LED_0_PIN);
	sigfoxSendMessage(msg, len);
}

static void sendSigFoxMode(uint8_t *msg, uint8_t len) {
	port_pin_toggle_output_level(LED_0_PIN);
	sigfoxSendMessage(msg, len);
}

bool executeSigFox(sigFoxT *msg) {
	switch (msg->messageType){
		
		case confMessage:
		sendSigFoxCommand(confModeMsg,  sizeof(confModeMsg)-1);
		break;
		
		case dataMessage:
		sendSigFoxMode(sigFoxModeMsg, sizeof(sigFoxModeMsg)-1);
		break;
		
		default:
		//error print Help
		break;
	}
	
	return true;
}