/*
 * APA102C.h
 *
 * Custom Library for writing values to the APA102C RGB LED
 *
 * Created: 7/21/2015 15:11:37
 *  Author: Joshua
 */ 


#ifndef APA102C_H_
#define APA102C_H_

#include "color.h"

void sendColor(uint8_t clkPin, uint8_t datPin,const rgb c);

void setPort(volatile uint8_t* port);           // TODO: THis is unused, but there are calls to it with no prototype that need to be dealt with

#endif /* APA102C_H_ */