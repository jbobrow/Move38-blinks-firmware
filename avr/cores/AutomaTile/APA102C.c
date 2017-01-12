/*
 * APA102C.c
 *
 * Created: 7/21/2015 14:51:49
 *  Author: Joshua
 */ 
#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "APA102C.h"

#include "Pins.h"

#define set(port,pin) (port |= pin) // set port pin
#define clear(port,pin) (port &= (~pin)) // clear port pin
#define bit_val(byte,bit) (byte & (1 << bit)) // test for bit set

uint8_t portSet = 0;
void setPort(volatile uint8_t* port){
	portSet = 1;
	SPI_PORT = port;
}

//bit bangs an SPI signal to the specified pins of the given data
static void sendBit( uint8_t data){
    
    uint8_t bitmask = 0b10000000;
    
    while (bitmask) {
        
        // Set the data pin
        
        if (data&bitmask) {
            LEDPORT |= LEDDAT;
            } else {
            LEDPORT &= ~LEDDAT;
        }
        
        // strobe the clock pin
        
        LEDPORT |= LEDCLK;
        LEDPORT &= ~LEDCLK;
        
        bitmask >>= 1;               // Walk to next bit
    }
 }    
 
//bit bangs an SPI signal to the specified pins of the given data
void sendByte(uint8_t data){
	sendBit( bit_val(data,7));
	sendBit( bit_val(data,6));
	sendBit( bit_val(data,5));
	sendBit( bit_val(data,4));
	sendBit( bit_val(data,3));
	sendBit( bit_val(data,2));
	sendBit( bit_val(data,1));
	sendBit( bit_val(data,0));
}
//bit bangs an SPI signal to the specified pins that generates the specified color 
//	formatted for the APA102, provided as a byte array of R,G,B
void sendColor(uint8_t clkPin, uint8_t datPin,const rgb c){
	if(!portSet){
		return;
	}
	//Start Frame
	sendByte( 0x00);
	sendByte( 0x00);
	sendByte( 0x00);
	sendByte( 0x00);
	//Data
	sendByte( 0xE1);//Set brightness to current to minimum TODO: Add setBrightness function (0xE1...0xFF)
	sendByte( c.b);
	sendByte( c.g);
	sendByte( c.r);
}
