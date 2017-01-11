// Color conversion
// color.h
// http://stackoverflow.com/questions/3018313/algorithm-to-convert-rgb-to-hsv-and-hsv-to-rgb-in-range-0-255-for-both

#ifndef COLOR_H_
#define COLOR_H_

#include <stdio.h>
#include <stdint.h>

// Delta error is the difference between and float and a rounded int value that is considered not valid
#define DELTA_ERROR 38
#define min(a,b) ((a)<(b)?(a):(b))
#define max(a,b) ((a)>(b)?(a):(b))
#define abs(x) ((x)>0?(x):-(x))

// Direct values for full and half rotation for a wheel with a range of 127 values
#define WHEEL_360	127
#define WHEEL_180	63


typedef struct {
    uint8_t r;       // percent
    uint8_t g;       // percent
    uint8_t b;       // percent
} rgb;

typedef struct {
	// hue needs to have sign to avoid overflows when the difference between one of RGB is negative
    // int16_t with delta error 2
    // int8_t delta error 9 

    int8_t h;       // angle in degrees
    uint8_t s;       // percent
    uint8_t v;       // percent
} hsv;

hsv rgb2hsv(rgb in);
rgb hsv2rgb(hsv in);

#endif /* COLOR_H_ */