/*
 * Color RGB to/from HSV
 */

#include "color.h"

rgb hsv2rgb(hsv in){
    rgb out;
    uint8_t region, p, q, t;
    uint16_t h, s, v, remainder;

    if (in.s == 0) {
        out.r = in.v;
        out.g = in.v;
        out.b = in.v;
        return out;
    }

    // 2* to transform our hue range from 127 to 255
    // converting to 16 bit to prevent overflow
    h = 2*in.h;
    s = in.s;
    v = in.v;
    // in what region of the hue wheel this color is (there are 6 regions red, yellow, green, cyan, blue, and magenta)
    // since our value goes from 0 to 255, each reason will be equal 255/6=43
    // https://en.wikipedia.org/wiki/HSL_and_HSV#Hue_and_chroma
    region = (uint8_t)(h) / 43;
    remainder = (h - (region * 43)) * 6; 

    p = (v * (255 - s)) >> 8;
    q = (v * (255 - ((s * remainder) >> 8))) >> 8;
    t = (v * (255 - ((s * (255 - remainder)) >> 8))) >> 8;

    // printf("region = %d, remainder = %d\n", region, remainder);
    // printf("p = %d, q = %d, t = %d \n", p, q, t);

    switch (region) {
        case 0:
            out.r = v; out.g = t; out.b = p;
            break;
        case 1:
            out.r = q; out.g = v; out.b = p;
            break;
        case 2:
            out.r = p; out.g = v; out.b = t;
            break;
        case 3:
            out.r = p; out.g = q; out.b = v;
            break;
        case 4:
            out.r = t; out.g = p; out.b = v;
            break;
        default:
            out.r = v; out.g = p; out.b = q;
            break;
    }

    return out;
}

hsv rgb2hsv(rgb in) {
    hsv out;
    uint8_t rgbMin, rgbMax;
    rgbMax = max(max(in.r, in.g), in.b);
    rgbMin = min(min(in.r, in.g), in.b);
    //printf("rgbMin = %d, rgbMax = %d\n", rgbMin, rgbMax);

    out.v = rgbMax;
    if (out.v == 0) {
        out.h = 0;
        out.s = 0;
        return out;
    }

    out.s = 255 * (long)(rgbMax - rgbMin) / out.v;
    if (out.s == 0) {
        out.h = 0;
        return out;
    }
    // 0, 43, 86, 23 are the equivalent values to hue values formulas using a wheel of 127 values
    // http://www.rapidtables.com/convert/color/rgb-to-hsv.htm
    if (rgbMax == in.r) {
        out.h = 0 + 23 * (in.g - in.b) / (rgbMax - rgbMin);
        //printf("(out.g - out.b) / (rgbMax - rgbMin) %d\n", (out.g - out.b) / (rgbMax - rgbMin));
    } else if (rgbMax == in.g) {
        out.h = 43 + 23 * (in.b - in.r) / (rgbMax - rgbMin);
    } else {
        out.h = 86 + 23 * (in.r - in.g) / (rgbMax - rgbMin);
    }

    // if hue value is negative add a complete hue wheel turn (in our case 127)
    if (out.h < 0) {
      out.h += 127;
    }
    
    return out;
}

