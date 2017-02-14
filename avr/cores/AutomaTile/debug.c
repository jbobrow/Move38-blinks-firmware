#include "color.h"
#include "Pins.h"
#include "avr/delay.h"

void debugBlinkRed(void){
	debugBlinkColor(red);
}

void debugBlinkGreen(void){
	debugBlinkColor(green);
}

void debugBlinkBlue(void){
	debugBlinkColor(blue);
}

void debugBlinkColor(const rgb c){
	uint8_t i=4;
	while (i--) {
		sendColor(LEDCLK, LEDDAT, c);
		_delay_ms(100);
		sendColor(LEDCLK, LEDDAT, black);
		_delay_ms(100);	
	}
}