#include <AutomaTile.h>
#include <Arduino.h>
#include "color.h"
#include "APA102C.h"
#include "debug.h"

uint32_t prevTimer;

static uint8_t seqNum = 0;//Sequence number used to prevent circular retransmission of data

int main(void) {
	tileSetup();

	// Power on sequence - 3 short blue blinks
	// so we can visually see when a reset happens

	debugBlinkBlue();

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
				int32_t gt = getTimer();
				int32_t gst = getSleepTimer();
				int32_t timeoutDiff = 0;
				// WATCHOUT!! HACK. Its is not clear to me how is it possible that at any point
				// timer value is smaller, than sleep timer, my guess is that at some "point"
				// someone is reading one of both variables while an interrupt is fired without precaution
				// which messes one of the values. This fixes the issue here, and for the moment
				if (gt > gst) {
					timeoutDiff = gt - gst;
				}

				if(timeoutDiff>timeout){
					if(gt > gst)
						debugBlinkGreen();
					else
						debugBlinkRed();
					debugBlinkBlue();
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
