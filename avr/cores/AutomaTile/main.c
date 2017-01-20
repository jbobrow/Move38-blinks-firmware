#include <AutomaTile.h>
#include <Arduino.h>
#include "color.h"
#include "APA102C.h"

#include <util/delay.h>

uint32_t prevTimer;
const rgb black = {0x00, 0x00, 0x00};
const rgb transmitColor = {0xff, 0x55, 0x00};
const rgb recieveColor = {0x00, 0xff, 0x55};
const rgb red = {0xff, 0, 0};
const rgb green = {0, 0xff, 0};
const rgb blue = {0, 0, 0xff};

static uint8_t seqNum = 0;//Sequence number used to prevent circular retransmission of data

void debugBlinkColor(const rgb c){
	uint8_t i=4;
	while (i--) {
		sendColor(LEDCLK, LEDDAT, c);
		_delay_ms(100);
		sendColor(LEDCLK, LEDDAT, black);
		_delay_ms(100);	
	}
}

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
						debugBlinkColor(green);
					else
						debugBlinkColor(red);
					debugBlinkColor(blue);
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
		}else if(mode==recieving){
			//disable A/D
			disAD();
			//set photo transistor interrupt to only trigger on specific direction
			setDir(progDir);
			//set recieving color
			sendColor(LEDCLK, LEDDAT, recieveColor);
			//record time entering the mode for timeout
			modeStart = getTimer();
			while(mode==recieving){//stay in this mode until instructed to leave or timeout
				uint32_t diff = getTimer()-modeStart;
				if(diff>20*PULSE_WIDTH){//Been too long without any new data*/
					mode = transmitting;
				}
			}
		}else if(mode==transmitting){
			//disable Phototransistor Interrupt
			setDirNone();
			//set LED to output
			DDRB |= IR;//Set direction out
			//send 5 pulses
			uint32_t startTime = getTimer();
			if(bitsRcvd>=8 && msgNum!=seqNum){
				seqNum = msgNum;
				int i;
				for(i=0; i<5; i++){
					while(getTimer()==startTime){
						PORTB &= ~IR;
					}
					startTime = getTimer();
					while(getTimer()==startTime){
						PORTB |= IR;
					}
					startTime = getTimer();
				}

				for(i=0;i<bitsRcvd/8-1;i++){
					datBuf[i]=comBuf[i];
				}
			}else{
				bitsRcvd = 0;
			}

			startTime = getTimer();
			//sendColor(LEDCLK, LEDDAT, transmitColor);//update color while waiting
			while(getTimer()<startTime+5*PULSE_WIDTH);//pause for mode change
			startTime = getTimer();
			uint16_t timeDiff;
			uint16_t bitNum;
			while(bitsRcvd>0){
				timeDiff = (getTimer()-startTime)/PULSE_WIDTH;
				bitNum = timeDiff/2;
				if(timeDiff%2==0){//first half
					if(comBuf[bitNum/8]&(1<<bitNum%8)){//bit high
						PORTB &=  ~IR;
					}else{//bit low
						PORTB |=  IR;
					}
				}else{//second half
					if(comBuf[bitNum/8]&(1<<bitNum%8)){//bit high
						PORTB |=  IR;
					}else{//bit low
						PORTB &=  ~IR;
					}
				}
				if(bitNum>=bitsRcvd){
					bitsRcvd = 0;
				}
			}
			while(getTimer()<startTime+2000);//pause for effect

			//done transmitting
			//re-enable A/D
			enAD();
			//re-enable all phototransistors
			setDirAll();
			setState(0);

			mode = running;
		}
	}
}
