/*
 * AutomaTiles.h
 *
 * Created: 1/6/2016 11:56:26
 *  Author: Joshua
 */ 


#ifndef AUTOMATILES_H_
#define AUTOMATILES_H_

#include <stdbool.h>
#include <stdint.h>

#define TILE_SIDES	6
#define LED_REFRESHING_PERIOD  32  	// Led refreshing period is 32ms (check main.c)

typedef void (*cb_func)(void);

void tileSetup(void);

void getNeighborStates(uint8_t * result);
uint8_t getNeighbor(const uint8_t neighbor);
bool isAlone(void);

void sendStep(void);
uint32_t getTimer(void);

void setColor(const uint8_t color[3]);
void setColorRGB(const uint8_t r, const uint8_t g, const uint8_t b);

void fadeTo(const uint8_t r, const uint8_t g, const uint8_t b, const uint16_t ms);
//void fadeTo(const Color c, uint8_t ms);
//void fadeTo(const Color c, uint8_t ms);

void blink(const uint16_t ms);
//void blink(int ms, Color c); // defaults to on/off of this color
//void blink(int ms, Color[n] c); // send array of colors to blink between
//void blink(int ms, int min, int max); // low and high levels for blinking and the time between them
//void blink(int ms, Color c, int min, int max); // low and high levels for blinking and the time between them

void pulse(const uint16_t ms); // phase
//void pulse(int ms, int min, int max); // phase w/ low and high brightness
//void pulse(int ms, Colors[n] c); // phased pulse between colors (depends on fadeTo)

void setState(uint8_t state);
uint8_t getState(void);
void setStepCallback(cb_func cb);
void setLongButtonCallback(cb_func cb, uint16_t);
void setLongButtonCallbackTimer(uint16_t);
void setButtonCallback(cb_func cb);
void setTimerCallback(cb_func cb, uint16_t t);
void setTimerCallbackTime(uint16_t t);
void setTimeout(uint16_t seconds);
void setMicOn();
void setMicOff();

#define PULSE_WIDTH 8
extern volatile int16_t holdoff;
//extern uint8_t outColor[3];
extern volatile uint8_t wake;

extern volatile uint8_t progDir;
extern volatile uint8_t* comBuf;//buffer for holding communicated messages when programming rules (oversized)
extern volatile uint8_t* datBuf;//buffer for holding verified messages to be accessed by the user
extern uint8_t datLen;
extern volatile uint8_t msgNum;
extern volatile uint16_t bitsRcvd;
extern volatile uint32_t modeStart;

#endif /* AUTOMATILES_H_ */