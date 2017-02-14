/*
 * AutomaTile.c
 *
 * Created: 1/6/2016 11:56:47
 *  Author: Joshua
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/cpufunc.h>
#include <avr/sleep.h>
#include <util/delay.h>

#include "Pins.h"
#include "Inits.h"
#include "AutomaTile.h"
#include "color.h"
#include "debug.h"

volatile int16_t holdoff = 50;//for temporarily preventing click outputs
volatile static uint8_t click = 0;//becomes non-zero when a click is detected
volatile static uint8_t sync = 0;//becomes non-zero when synchronization pulses need to be sent out
volatile static uint8_t state = 0;//current state of tile
volatile static uint32_t timer = 0;//.1 ms timer tick
volatile static uint32_t times[6][4];//ring buffer for holding leading  detection edge times for the phototransistors
volatile static uint8_t timeBuf[6];//ring buffer indices
volatile static uint8_t soundEn = 1; //if true, react to sound

// Pin mapping to arrange pins correctly on board
const uint8_t pinMap[6] = {0,1,2,5,4,3};

int32_t timeout = 30000; // s -> *1000ms
volatile static int32_t startTime = 0;
volatile static int32_t sleepTimer = 0;
volatile static uint32_t powerDownTimer = 0;
volatile uint8_t wake = 0;

volatile static uint16_t longPressTimer = 0;
volatile static uint16_t longPressTime = 1000;//1 second default

volatile static uint16_t clickDetectionTimer = 0;
volatile static uint16_t clickdetectionTime = 330;	//330ms default

volatile uint8_t progDir = 0;//direction to pay attention to during programming. Set to whichever side put the module into program mode.
volatile uint8_t* comBuf;//buffer for holding communicated messages when programming rules (oversized)
volatile uint8_t* datBuf;//buffer for holding verified messages to be accessed by the user
uint8_t datLen = 0;
volatile uint16_t bitsRcvd = 0;//tracking number of bits received for retransmission/avoiding overflow
volatile uint32_t modeStart = 0;

enum MODE {
	sleep,
	running,
	recieving,
	transmitting
};
enum MODE mode = running;

enum LEDMODE {
	stillMode,
	fadeMode,
	fadeRainbowMode,
	blinkMode,
	pulseMode
};
enum LEDMODE ledMode = stillMode;

enum COLORS{
	RED,
	GREEN,
	BLUE,
	COLOR_MAX
};

typedef struct {
	uint8_t currC; // Current color component
	uint8_t toC;  // color component to go to
	int8_t inc;	// Hue increment per transition
	int16_t error;	// Bressenham algorithm error to enable up to 32768ms delays
	int16_t dc;	// Red differential
	uint16_t dt;	// time differential is the amound of discrete time steps per fade transition
					// or the number of times that the LED will be refreshed for this transition	
} Bressenham;

struct Fading {
	uint16_t fadeCntr;	// Counter used to end the fade transitions
	Bressenham c[COLOR_MAX];
} fading;

struct fadingRainbow {
	hsv fromHSV;
	hsv currHSV;
	hsv toHSV;
	uint16_t fadeCntr;	// Counter used to end the fade transitions
	uint8_t inc;	// Hue increment per transition
	bool positiveIncrement;
	int16_t error;	// Bressenham algorithm error to enable up to 32768ms delays
	int16_t dh;	// hue differential and time differential
	int16_t dt;	// time differential is the amound of discrete time steps per fade transition
					// or the number of times that the LED will be refreshed for this transition
} fadingRainbow;

struct Blinking {
	bool status; // blink status OFF or ON
	uint16_t period;
	uint32_t next;	// Next time to switch blink status. current timer + period/2
} blinking;

struct Pulsing {
	uint8_t min;
	uint8_t max;
	hsv currHSV;
	uint8_t increment;	// Brightness increments between LED refreshing cycles
	bool rampUp;
} pulsing;


/* Uses the current state of the times ring buffer to determine the states of neighboring tiles
 * For each side, to have a non-zero state, a pulse must have been received in the last 100 ms and two of the
 * last three timing spaces must be equal.
 *
 * State is communicated as a period for the pulses. Differences are calculated between pulses and if a consistent
 * difference is found, that translates directly to a state
 * Accuracy is traded for number of states (i.e. 5 states can be communicated reliably, while 10 with less robustness)
*/
uint8_t oldData[] = {0,0,0,0,0,0};

void getNeighborStates(uint8_t * result){
	uint8_t interrupts = SREG&1<<7;

	if(interrupts)cli();//Disable interrupts to safely grab consistent timer value
	uint32_t curTime = timer;
	uint32_t diffs[3];
	uint8_t i;
	for(i = 0; i < 6; i++){

		if((curTime-times[i][timeBuf[i]])>100){//More than .1 sec since last pulse, too long
			result[pinMap[i]] = 0;
		}else{//received pulses recently
			uint8_t buf = timeBuf[i];//All bit-masking is to ensure the numbers are between 0 and 3
			diffs[0] = times[i][buf] - times[i][(buf-1)&0x03];
			diffs[1] = times[i][(buf-1)&0x03] - times[i][(buf-2)&0x03];
			diffs[2] = times[i][(buf-2)&0x03] - times[i][(buf-3)&0x03];
			if(diffs[0]>100 || diffs[1]>100 || diffs[2] > 100){//Not enough pulses recently
				result[pinMap[i]] = 0;
				oldData[i] = 0;
			}else{//received enough pulses recently
				//rounding
				diffs[0] >>= 3;
				diffs[1] >>= 3;
				diffs[2] >>= 3;
				//checking if any two of the differences are equal and using a value from the equal pair
				if(diffs[0] == diffs[1] &&diffs[0] == diffs[2]){
					result[pinMap[i]] = (uint8_t) diffs[0];
					oldData[i]=result[i];
				}else{//too much variation reuse old value
					result[pinMap[i]] = oldData[i];
				}
			}
		}
	}
	if(interrupts)sei();//Re-enable interrupts
}

/*
 * Get a single neighbor state using a tile side neigbor id.
 */
uint8_t getNeighbor(const uint8_t neighbor){
	uint8_t neighbors[TILE_SIDES];
	getNeighborStates(neighbors);
	return neighbors[neighbor];
}

bool isAlone(void){
	uint8_t neighbors[TILE_SIDES];
	bool alone = true;
	getNeighborStates(neighbors);
	uint8_t i;
	for(i=0; i<TILE_SIDES; i++){
		if (neighbors[TILE_SIDES]){
			alone =  false;
			break;
		}
	}
	return alone;
}

int32_t getTimer(){
	uint8_t interrupts = SREG&1<<7;
	if(interrupts)cli();
	int32_t t = timer;
	if(interrupts)sei();
	return t;
}

int32_t getSleepTimer(){
	uint8_t interrupts = SREG&1<<7;
	if(interrupts)cli();
	int32_t t = sleepTimer;
	if(interrupts)sei();
	return t;
}
void setButtonLongPressed(uint16_t ms){
	uint8_t interrupts = SREG&1<<7;
	if(interrupts)cli();
	longPressTime = ms;
	if(interrupts)sei();
}

void setButtonClickThreshold(uint16_t ms){
	uint8_t interrupts = SREG&1<<7;
	if(interrupts)cli();
	clickdetectionTime = ms;
	if(interrupts)sei();
}
void setTimeout(uint32_t seconds){
	timeout = seconds*1000;  // Convert seconds into ms

}

void setState(uint8_t newState){
	if (newState<16)
	{
		state = newState;
	}

}

uint8_t getState(){
	return state;
}
/*
void setMicOn(){
	soundEn = 1;
}

void  setMicOff(){
	soundEn = 0;
}*/

void tileSetup(void){
	//Initialization routines
	initIO();
	setPort(&PORTB);
	sendColor(LEDCLK,LEDDAT,black);
	sei();
	//initAD();
	initTimer();
	set_sleep_mode(SLEEP_MODE_PWR_DOWN);
	sleep_enable();
	//Set up timing ring buffers
	uint8_t i;
	for(i = 0; i<6; i++){
		timeBuf[i]=0;
	}
	mode = running;
}

void emptyCB(void){
	return;
}

cb_func clickCB = emptyCB;
cb_func timerCB = emptyCB;

volatile uint16_t timerCBcount = 0;
volatile uint16_t timerCBtime = UINT16_MAX;

void setColor(const uint8_t color[3]){
	setColorRGB(outColor.r, outColor.g, outColor.b);
}

void setColorRGB(const uint8_t r, const uint8_t g, const uint8_t b){
	// stop blinking or fadingRainbow...
	ledMode = stillMode;
	// set color
	outColor.r = r;
	outColor.g = g;
	outColor.b = b;
}

/*
 * Fade from current RGB color to RGB parameter, ms is the duration for the fade transition
 *
 */
void fadeTo(const uint8_t r, const uint8_t g, const uint8_t b, const uint16_t ms){
	ledMode = fadeMode;
	//rgb toRGB = {r, g, b};
	// Transform current and next color to HSV
	// fadingRainbow.fromHSV = rgb2hsv(outColor);
	fading.c[RED].currC = outColor.r;
	fading.c[GREEN].currC = outColor.g;
	fading.c[BLUE].currC = outColor.b;
	fading.c[RED].toC = r;
	fading.c[GREEN].toC = g;
	fading.c[BLUE].toC = b;
	// fadingRainbow.toHSV = rgb2hsv(toRGB);

	//printf("Fade from R = %d, G=%d, B= %d\n", outColor.r, outColor.g, outColor.b);
	//printf("Fade to R = %d, G=%d, B= %d\n", r, g, b);

	uint8_t i;
	for (i = 0; i < COLOR_MAX; i++)	{
		fading.c[i].dt = ms/LED_REFRESHING_PERIOD;
		fading.fadeCntr = fading.c[i].dt;
		// printf("Led Updates Per Period = %d\n", fading.fadeCntr);
		fading.c[i].error = 0;
		fading.c[i].dc = fading.c[i].toC - fading.c[i].currC;
		if(fading.c[i].dc >= 0) {
			debugBlinkGreen();
		} else {
			debugBlinkRed();
		}

		fading.c[i].inc = fading.c[i].dc / fading.fadeCntr;

		// printf("Color Diff = %d\n", fading.c[i].dc);	
		// printf("Color Increment = %d\n", fading.c[i].inc);
	}
}


void fadeUpdateRGBComponent(uint8_t color_index) {
	// Make sure we never go beyon minimum (0) and maximum (255) limits
	if ( (fading.c[color_index].currC + fading.c[color_index].inc) < 0 ) {
		fading.c[color_index].currC = 0;
	} else if ( (fading.c[color_index].currC + fading.c[color_index].inc) > 255 ) {
		fading.c[color_index].currC = 255;
	} else {
		fading.c[color_index].currC = fading.c[color_index].currC + fading.c[color_index].inc;  // Increment current Hue value				
		// Discretization double to int correction
		// This solves casting and rounding issues using uint8_t.Based on Bressenham's algorithm
		//printf("currR=%d < %d, toR=%d, cnt=%d, inc=%d \n", fading.currRGB.r,fading.toRGB.r - fading.fadeCntr * fading.inc,fading.toRGB.r, fading.fadeCntr, fading.inc);
		if (fading.c[color_index].dc > 0) {
			// Making sure we never pass over the RGB to value
			if (fading.c[color_index].currC >= fading.c[color_index].toC) {
				fading.c[color_index].currC = fading.c[color_index].toC;
			} else if (!fading.c[color_index].inc) { // Increment equal to 0
				// Bressenham's algorithm, incremental scan conversion algorithm
			 	fading.c[color_index].error += 2*fading.c[color_index].dc;
			 	if (fading.c[color_index].error > fading.c[color_index].dt) {
			 		fading.c[color_index].currC = fading.c[color_index].currC + 1;
			 		fading.c[color_index].error -= 2* fading.c[color_index].dt;
			 	}
			// We look at the current value using rounded increment, and compared to the same equivalent increment from our 
			// target hue value, if smaller increment by 1.
			} else if (fading.c[color_index].currC < (fading.c[color_index].toC - fading.fadeCntr * fading.c[color_index].inc)){
				fading.c[color_index].currC = fading.c[color_index].currC + fading.c[color_index].inc;
			}
		} else { // dc >0
			// Making sure we never pass over the RGB to value
			if (fading.c[color_index].currC <= fading.c[color_index].toC) {
				fading.c[color_index].currC = fading.c[color_index].toC;
			} else if (!fading.c[color_index].inc) { // Increment equal to 0
				// Bressenham's algorithm, incremental scan conversion algorithm
			 	fading.c[color_index].error += 2*abs(fading.c[color_index].dc);
			 	if (fading.c[color_index].error > fading.c[color_index].dt) {
			 		fading.c[color_index].currC = fading.c[color_index].currC - 1;
			 		fading.c[color_index].error-= 2* fading.c[color_index].dt;
			 	}

			} else if (fading.c[color_index].currC > (fading.c[color_index].toC - fading.fadeCntr * fading.c[color_index].inc)){
				fading.c[color_index].currC = fading.c[color_index].currC + fading.c[color_index].inc;
			}
		}
	}
}

void fadeUpdate(void) {
	// Output current color 
	//sendColor(LEDCLK, LEDDAT, outColor);
	// Terminal bar
	//printf("R = %d, G = %d, B = %d, cntr = %d \n", fading.c[RED].currC, fading.c[GREEN].currC, fading.c[BLUE].currC, fading.fadeCntr);
	outColor.r = fading.c[RED].currC;
	outColor.g = fading.c[GREEN].currC;
	outColor.b = fading.c[BLUE].currC;
	sendColor(LEDCLK, LEDDAT, outColor);
	//for (int i = 0; i < fading.currRGB.r; ++i) {
	//	printf("#");
	//}
	//printf("\n");
	
	// Only fade if the number of led fade refreshes is bigger than 0!
	if (fading.fadeCntr--) {
		uint8_t i;
		for (i = 0; i < COLOR_MAX; i++)	{
			fadeUpdateRGBComponent(i);
		}
	} else {  // End of the fade to transition, return to send colors
		ledMode = stillMode;
		//printf("Fade ending :)\n");
	}
}

	
/*
 * Fade from current RGB color to RGB parameter, ms is the duration for the fade transition
 *
 */
void fadeToRainbow(const uint8_t r, const uint8_t g, const uint8_t b, const uint16_t ms){
	ledMode = fadeRainbowMode;

	rgb toRGB = {r, g, b};
	// Transform current and next color to HSV
	fadingRainbow.fromHSV = rgb2hsv(outColor);
	fadingRainbow.currHSV = fadingRainbow.fromHSV;
	fadingRainbow.toHSV = rgb2hsv(toRGB);

	fadingRainbow.dt = ms/LED_REFRESHING_PERIOD;
	fadingRainbow.fadeCntr = fadingRainbow.dt;
	fadingRainbow.error = 0;

	fadingRainbow.dh = abs(fadingRainbow.fromHSV.h - fadingRainbow.toHSV.h);

	// Looking for the fastest route to reach the next Color
	// if the increment is smaller than 180, that meeans that the shortest route is within a full circle (0 to 360 hue degrees)
	// although if the increment is bigger than 180, that means that the shorter route will be over 360 or under 0 degrees
	if (fadingRainbow.dh < WHEEL_180) {
		if (fadingRainbow.fromHSV.h < fadingRainbow.toHSV.h) {
			fadingRainbow.positiveIncrement = true;
		} else {
			fadingRainbow.positiveIncrement = false;
		}
	} else { // Hue increment per update period is bigger than 180
		if (fadingRainbow.fromHSV.h < fadingRainbow.toHSV.h) {
			fadingRainbow.positiveIncrement = false;
		} else {
			fadingRainbow.positiveIncrement = true;
		}
		// adjust increment for transitions that overflow the wheel
		fadingRainbow.dh = WHEEL_360 - fadingRainbow.dh;
	}
	fadingRainbow.inc = fadingRainbow.dh / fadingRainbow.fadeCntr;

}

void fadeRainbowUpdate(void) {
	// Output current color
	outColor = hsv2rgb(fadingRainbow.currHSV);
	sendColor(LEDCLK, LEDDAT, outColor);
	// Terminal bar
	/*for (int i = 0; i < fadingRainbow.currHSV.h; ++i) {
		printf("#");
	}
	printf("\n");*/

	// Only fade if the number of led fade refreshes is bigger than 0!
	if (fadingRainbow.fadeCntr--) {
		if (fadingRainbow.positiveIncrement) {  // Positive increment moving clockwise along the Hue wheel
			// Detect and correct an color overflow hue value situation
			if ((fadingRainbow.currHSV.h + fadingRainbow.inc) >= WHEEL_360) {
				fadingRainbow.currHSV.h = fadingRainbow.currHSV.h + fadingRainbow.inc - WHEEL_360;
			} else {
				fadingRainbow.currHSV.h += fadingRainbow.inc;  // Increment current Hue value
				// Discretization double to int correction
				// This solves casting and rounding issues using uint8_t.Based on Bressenham's algorithm
				if (!fadingRainbow.inc) { // Increment equal to 0
					// Bressenham's algorithm, incremental scan conversion algorithm
				 	fadingRainbow.error += 2*fadingRainbow.dh;
				 	if (fadingRainbow.error > fadingRainbow.dt) {
				 		fadingRainbow.currHSV.h++;
				 		fadingRainbow.error-= 2* fadingRainbow.dt;
				 	}
				// We look at the current value using rounded increment, and compared to the same equivalent increment from our
				// target hue value, if smaller increment by 1.
				 } else if (fadingRainbow.currHSV.h < (fadingRainbow.toHSV.h - fadingRainbow.fadeCntr * fadingRainbow.inc)){
					fadingRainbow.currHSV.h++;
				}
			}
		} else { // Negative increment moving counterclockwise along the Hue wheel
			// Detect and correct negative overflows for hue values
			if ((fadingRainbow.currHSV.h - fadingRainbow.inc) <= 0) {
				fadingRainbow.currHSV.h = fadingRainbow.currHSV.h - fadingRainbow.inc + WHEEL_360;
			} else {
				fadingRainbow.currHSV.h -= fadingRainbow.inc;
				// This solves casting and rounding issues using uint8_t.Based on Bressenham's algorithm
				if (!fadingRainbow.inc) { // Increment equal to 0
					// Bressenham's algorithm
				 	fadingRainbow.error += 2*fadingRainbow.dh;
				 	if (fadingRainbow.error > fadingRainbow.dt)	{
				 		fadingRainbow.currHSV.h--;
				 		fadingRainbow.error-= 2* fadingRainbow.dt;
				 	}
				} else if(fadingRainbow.currHSV.h > ( (fadingRainbow.toHSV.h + fadingRainbow.fadeCntr * fadingRainbow.inc) % WHEEL_360 ) ){
					fadingRainbow.currHSV.h--;
				}
			}
		}
		//printf("Hue = %d, Saturation = %d, Value = %d \n", wheelTo360(fadingRainbow.currHSV.h), fadingRainbow.currHSV.s, fadingRainbow.currHSV.v);
	} else {  // End of the fade to transition, return to send colors
		ledMode = stillMode;
		outColor = hsv2rgb(fadingRainbow.toHSV);
		//printf("Fade ending :)\n");
	}
}

/*
 * This sets up a basic blink animation, ms is the blink period in ms
 */
void blink(const uint16_t ms){
	//only update blink if the mode has changed or the timing has changed
	if(ledMode != blinkMode || blinking.period != ms) {
		ledMode = blinkMode;
		blinking.status = false;
		blinking.period = ms;
		blinking.next = ms + getTimer();
	}
}

void blinkUpdate(void) {
	if ((blinking.next-getTimer()) > blinking.period) {
		if (blinking.status) { // On to Off
			//printf("OFF\n" );
			sendColor(LEDCLK, LEDDAT, black);
			blinking.status = false;
		} else {  // Off to On
			//printf("ON\n");
			sendColor(LEDCLK, LEDDAT, outColor);
			blinking.status = true;
		}
		blinking.next += blinking.period;  // Updating the next blinking time incrementing it by the blinking period in ms
	}
}

/*
 * This sets up a pulse animation, pulsation works incrementing and decrementing the Value (hsv)
 * @ms; is the blink period in ms
 */
void pulse(const uint16_t ms){
	ledMode = pulseMode;
	// Initializing pulsing variables
	pulsing.currHSV = rgb2hsv(outColor);
	//printf("Pulse h = %d, s=%d, v= %d\n", pulsing.currHSV.h, pulsing.currHSV.s, pulsing.currHSV.v);
	// Start pulsing from current value
	pulsing.rampUp = false;
	pulsing.min = 0;
	pulsing.max = 255;

	uint16_t ledUpdatesPerPeriod = ms/LED_REFRESHING_PERIOD;  // This is how many times the LED will be refreshed by pulsing period
	//printf("ledUpdatesPerPeriod = %d\n", ledUpdatesPerPeriod);
	// Lets round up the ledUpdatePerPeriod if its a odd number
	// We do this to make sure that there will be always a led output cycle to reach the pulsing.max value
	// Note, we are rounding division, this means that the period will be a +- 32ms difference.
	if(ledUpdatesPerPeriod%2) {
		ledUpdatesPerPeriod++;
	}
	//printf("ledUpdatesPerPeriod = %d\n", ledUpdatesPerPeriod);
	// A period contains a ramp up cycle and a ramp down cycle. That is why we dived by two
	ledUpdatesPerPeriod >>= 1;
	//printf("ledUpdatesPerPeriod = %d\n", ledUpdatesPerPeriod);
	// TODO: fine a better place to conver
	pulsing.increment = (uint8_t)(pulsing.currHSV.v) / ledUpdatesPerPeriod;
	//printf("Increment = %d\n", pulsing.increment);

	// Check max and mins of increment to avoid weird situation
	if (pulsing.increment > pulsing.max) {
		pulsing.increment = pulsing.max;
	} else if (pulsing.increment < pulsing.min) {
		pulsing.increment = pulsing.min;
	}
}

void pulsingUpdate(void) {
	outColor = hsv2rgb(pulsing.currHSV);
	sendColor(LEDCLK, LEDDAT, outColor);

	if (pulsing.rampUp) { // pulsing is ramping up
		if ((pulsing.currHSV.v + pulsing.increment) >= pulsing.max) {  // If HSV value next cycle will be bigger than the max value time to ramp down
			pulsing.rampUp = false;
			pulsing.currHSV.v = pulsing.max;  // Make sure we never overflow
		} else {
			pulsing.currHSV.v += pulsing.increment;  // Increase HSV value for the next light update cycle (ledOutputControl () )
		}
	} else { // pulsing is ramping down
		if ((pulsing.currHSV.v - pulsing.increment) <= pulsing.min) {  // are we under our minimum value?
			pulsing.rampUp = true;
			pulsing.currHSV.v = pulsing.min;
		} else {
			pulsing.currHSV.v -= pulsing.increment;
		}
	}
	//printf("Is ramping up? = %d\n", pulsing.rampUp);
}

/*
 * This controls the led output mode and its logic
 */
void updateLed(void) {
	switch(ledMode){
		case stillMode:
			sendColor(LEDCLK, LEDDAT, outColor);
			break;
		case fadeMode:
			fadeUpdate();
			break;
		case fadeRainbowMode:
			fadeRainbowUpdate();
			break;
		case blinkMode:
			blinkUpdate();
			break;
		case pulseMode:
			pulsingUpdate();
			break;
	}
}

void setTimerCallback(cb_func cb, uint16_t t){
	timerCB = cb;
	timerCBcount = 0;
	timerCBtime = t;
}

void setTimerCallbackTime(uint16_t t){
	timerCBcount = 0;
	timerCBtime = t;
}

void sendStep(){
	uint8_t interrupts = SREG&1<<7;

	if(interrupts)	cli();
	uint32_t t = timer;
	if(interrupts)sei();
	uint32_t st = t;
	uint8_t done = 0;
	sync = 3;
	holdoff = 200;

	while(!done){
		cli();
		t = timer;
		sei();
		if(t-st>100){
			done = 1;
		}
	}
	clickCB();
}

void setSharedDataBuffer(uint8_t* comb,uint8_t* datb , uint8_t len){
	comBuf = comb;
	datBuf = datb;
	datLen = len;
}

uint8_t getSharedData(uint8_t i){
	if(i>=datLen){
		return 0;
	}

	return datBuf[i];
}

//Timer interrupt occurs every 1 ms
//Increments timer and controls IR LEDs to keep their timing consistent
ISR(TIM0_COMPA_vect){
	volatile static uint8_t IRcount = 0;//Tracks cycles for accurate IR LED timing
	volatile static uint8_t sendState = 0;//State currently being sent. only updates on pulse to ensure accurate states are sent
	volatile static bool pressed = false; // used to differenciate between button pressed and released states
	volatile static bool multipleClicks = false; // used to trigger multi clicks detection
	volatile static uint8_t numClicks = 0; // Counter for how many clicks have been pressed
	timer++;

	timerCBcount++;
	if(timerCBcount >= timerCBtime){
		timerCB();
		timerCBcount = 0;
	}

	if(mode==running){
		//check if a click has happened and call appropriate function
		if(click){
			clickCB();
			holdoff = 100;
			click = 0;
		}

		IRcount++;
		if(IRcount>=(uint8_t)(sendState*8+4)){//State timings are off by 4 from a multiple of 8 to help with detection
			IRcount = 0;
			if(sync==0){
				sendState = state;
			}
		}
		if(IRcount==5){
			PORTB |= IR;
			DDRB |= IR;
		}else if(IRcount==7&&sync>1){
			PORTB |= IR;
			DDRB |= IR;
			sync = 1;
		}else if(IRcount==9&&sync==1){
			PORTB |= IR;
			DDRB |= IR;
			sync = 0;
		}else if(sendState==0&&sync>0){//0 case is special
			if((IRcount&0x01)!=0){
				PORTB |= IR;
				DDRB |= IR;
				sync -= 1;
			}else{
				DDRB &= ~IR;//Set direction in
				PORTB &= ~IR;
			}
		}else{
			DDRB &= ~IR;//Set direction in
			PORTB &= ~IR;//Set pin tristated

			// if the button has been pressed increment longpresstimer
			// outside of the holdoff statement to make sure that we increase
			// during debouncing cycles too
			if(pressed){
				longPressTimer++;
			}

			// Multiclick detection
			if(multipleClicks){
				clickDetectionTimer++;
				if(clickDetectionTimer >= clickdetectionTime) {
					switch(numClicks){
						case 1:
							buttonClicked();
							break;
						case 2:
							buttonDoubleClicked();
							break;
						case 3:
							buttonTripleClicked();
							break;
						default:
							break;
					}
					// Reset multiple clicks detections after "detecting"
					multipleClicks = false;
					clickDetectionTimer = 0;
					numClicks = 0;
				}
			}

			if(IRcount<5){

			if(!holdoff){ // This is a good detection we are not debouncing!
				if(PINB & BUTTON){// Button pressed (Button active high)
					if(!pressed){  // Making sure buttonPressed is not called over and over while the button is pressed
						buttonPressed();
					}
					pressed = true;
					multipleClicks = true;  // Activate multiple cicks detections
					sleepTimer = timer;
					powerDownTimer = timer;

					if(longPressTimer>=longPressTime){
						// This will keep triggering the button long pressed function every time
						// the button keeps being pressed over long press time
						buttonLongPressed();
						longPressTimer = 0;
					}
				// Getting read of the holdoff fixed multiple clicks issues
				// Legacy library had 200ms debounce, which is a really high value, my guess
				// to help mask the main sleep detection issue.
				// Its hard to tell for me (Luis) what is the real debounce time when holdoff = 0,
				// at least 2ms but also its dependant on IRcount
				holdoff = 0;

				} else {  // Button not pressed
					if(pressed){
						buttonReleased();  // Button Released callback
						numClicks++;
						clickDetectionTimer = 0;
						sleepTimer = timer;
						powerDownTimer = timer;
						pressed = false;
						longPressTimer = 0;  // Reset lpt counter after button has been released
					}
				}
			} else {

			}

				/*if(PINB & BUTTON){//Button active high
					if(!holdoff){//initial press
						buttonPressed();
						sleepTimer = timer;
						powerDownTimer = timer;
						longPressTimer = 0;
						pressed = true;
					}else{//during long press wait
						if(longPressTimer>=longPressTime){
							buttonLongPressed();
						}
					}
					holdoff = 3;//debounce and hold state until released
				}else{//Button not down
					if(pressed && !holdoff){
						buttonReleased();  // Button Released callback
						sleepTimer = timer;
						powerDownTimer = timer;
						pressed = false;
						holdoff = 30;//debounce and hold state until released
					}
					longPressTimer = 0;
				}*/


			}
		}
	}else if(mode==sleep){
		uint32_t diff = timer-powerDownTimer;
		uint32_t startDiff = timer-startTime;
		if(diff>500 && wake==0){
			sei();
			sleep_cpu();
			cli();
			wake = 1;
			sleepTimer = timer;
		}
		if(wake == 1){
			startTime = timer;
			PORTA &= ~POWER;
			wake = 2;
		}else if (wake == 2){
			if(startDiff>250){
				wake=3;
			}
		}else if (wake == 3){
			PORTB |= IR;//Set pin on - Always set high before switching to output or will will short out Vcc to ground!
			DDRB |= IR;//Set direction out
			sendColor(LEDCLK, LEDDAT, wakeColor);
			startTime = timer;
			wake = 4;
		}else if(wake == 4){
			PORTB &= !IR;
			wake = 5;
		}else if(wake == 5){
			PORTB |= IR;
			wake = 6;
		}else if(wake == 6){
			if(startDiff>500){
				wake=7;
			}
		}else if(wake == 7){
			enAD();
			powerDownTimer = timer;
			sleepTimer = timer;
			holdoff=500;
			mode = running;
		}
	}
}


//INT0 interrupt triggered when the pushbutton is pressed
ISR(PCINT1_vect){
	if((DDRB & BUTTON)==0)
		wake = 1;
}

//Pin Change 0 interrupt triggered when any of the phototransistors change level
//Checks what pins are newly on and updates their buffers with the current time
static volatile uint32_t oldTime = 0;
volatile uint8_t msgNum = 0;

ISR(PCINT0_vect){
	static uint8_t prevVals = 0; //stores the previous state so that only what pins are newly on are checked
	static uint8_t pulseCount[6]; //stores counted pulses for various actions
	uint8_t vals = PINA & 0x3f; //mask out phototransistors
	uint8_t newOn = vals & ~prevVals; //mask out previously on pins

	if(mode == running){
		powerDownTimer = timer;

		uint8_t i;
		for(i = 0; i < 6; i++){
			if(newOn & 1<<i){ //if an element is newly on,
				if(timer-times[i][timeBuf[i]]<10){//This is a rapid pulse. treat like a click
					pulseCount[i]++;
					wake = 1;
					if(pulseCount[i]==2){
						if(holdoff==0){
							click = 1;
							sync = 3;
							sleepTimer = timer;
						}
					}
					if(pulseCount[i]>=4){//There have been 4 quick pulses. Enter programming mode.
						click = 0;
						sync = 0;
						mode = recieving;
						progDir = i;
						int j;
						for(j = 0; j < datLen; j++){//zero out buffer
							comBuf[j]=0;
						}
						msgNum = 0;
					}
				}else{//Normally timed pulse, process normally
					pulseCount[i]=0;
					timeBuf[i]++;
					timeBuf[i] &= 0x03;
					times[i][timeBuf[i]] = timer;
				}
			}
		}
	}else if(mode == recieving){
		modeStart = timer;
		if(((prevVals^vals)&(1<<progDir))){//programming pin has changed
			if(timer-oldTime > (3*PULSE_WIDTH)/2){//an edge we care about
				if(timer-oldTime > 4*PULSE_WIDTH){//first bit. use for sync
					bitsRcvd = 0;
				}
				oldTime = timer;
				if(bitsRcvd<8){
					uint8_t bit = ((vals&(1<<progDir))>>progDir);
					msgNum |= bit<<(bitsRcvd%8);
					bitsRcvd++;
				}else	if(bitsRcvd<datLen*8+8){
					uint8_t bit = ((vals&(1<<progDir))>>progDir);
					comBuf[bitsRcvd/8-1] |= bit<<(bitsRcvd%8);
					bitsRcvd++;
				}
			}
		}
	}

	prevVals = vals;
}
//ADC conversion complete interrupt
//Calculates a running median for zeroing out signal
//Then calculates a running median of deltas from the median to check for exceptional events
//If a delta is very high compared to the median, a click is detected and click is set to non-0
ISR(ADC_vect){
	//Values saved for derivative calculation
	static uint16_t median = 1<<15;
	static uint16_t medDelta = 1<<5;

	uint8_t adc;

	adc = ADCH;// Record ADC value

	//update running median. Error on high side.
	//note that due to comparison, the median is scaled up by 2^8
	if((adc<<8)<median){
		median--;
		}else{
		median++;
	}
	uint16_t delta;
	if(median > (adc<<8)){// Calculate delta
		delta = (median>>8)-adc;
		}else{
		delta = adc-(median>>8);
	}

	//Update running delta median. Error on high side.
	//note that due to comparison, the median is scaled up by 2^4=16
	if((delta<<4)<medDelta && medDelta > 10){
		medDelta--;
		}else{
		medDelta++;
	}

	if(holdoff == 0){//holdoff can be set elsewhere to disable click being set for a period of time
		if(medDelta < delta){//check for click. as the median delta is scaled up by 16, an exceptional event is needed.
			if(soundEn){
				click = delta;//Board triggered click as soon as it could (double steps)
				sync = 3;
				sleepTimer = timer;
			}
		}
	}
}
