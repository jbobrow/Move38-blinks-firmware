#include <AutomaTile.h>
#include <Arduino.h>
#include "color.h"
#include "APA102C.h"

#include <util/delay.h>

uint32_t prevTimer;
const rgb black = {0x00, 0x00, 0x00};
const rgb transmitColor = {0xff, 0x55, 0x00};
const rgb recieveColor = {0x00, 0xff, 0x55};

static uint8_t seqNum = 0;//Sequence number used to prevent circular retransmission of data

int main(void) {
	tileSetup();

	// Power on sequence - 3 short blue blinks
	// so we can visually see when a reset happens

	static rgb powerupColor = {0,0,255};

	uint8_t i=3;
	while (i--) {
		sendColor(LEDCLK, LEDDAT, powerupColor );
		_delay_ms(200);
		sendColor(LEDCLK, LEDDAT, black );
		_delay_ms(200);
		
	}

	setup();

	prevTimer = getTimer();
	while(1){
		if(mode == running){
			// Difference between current time and last time the LED was updated
			uint32_t diff = getTimer()-prevTimer;
			if(holdoff<diff){
				holdoff = 0;
			}else{
				holdoff -= diff;
			}
			uint8_t t = prevTimer%64;
			if(t<=32 && t+diff>=32){
				updateLed();
			}
			prevTimer = getTimer();

			if(timeout>0){
				if(prevTimer-sleepTimer>1000*timeout){
					mode = sleep;
					disAD();
					DDRB &= ~IR;//Set direction in
					PORTB &= ~IR;//Set pin tristated
					sendColor(LEDCLK, LEDDAT, black);
					PORTA |= POWER;//Set LED and Mic power pin high (off)
					wake = 0;
				}
			}

			loop();		
		}
	}
}
