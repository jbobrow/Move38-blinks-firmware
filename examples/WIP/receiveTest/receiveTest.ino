/*  Color Sides
 *  
 *  Show a color based on the side a neighbor is sensed
 *  
 *  Pulse white if alone
 *
 *  --------------------------------------------------------------------------------------------------
 *  IMPORTANT: To use this code in Arduino's IDE, first move the AutomaTiles folder
 *  into the right directory i.e. <user home directory>/Documents/Arduino/hardware/AutomaTiles
 *  Then open the Arduino IDE and select Tools > Board > "AutomaTile"
 *  Now you should be good to go :) (thanks to the hard work of Joshua Sloane)
 *  -------------------------------------------------------------------------------------------------- 
 *     
 *  by Jonathan Bobrow
 *  01.2017
 */

// colors arranged to travel the farthest in transition between colors
uint8_t colors[6][3] = {{204,0,0},        // Red
                        {0,204,204},      // Cyan
                        {204,0,204},      // Magenta
                        {204,204,0},      // Yellow
                        {0,0,255},        // Blue
                        {204,64,0}};      // Orange
                        
void setup() {
  setState(1);
  setMicOff();
  setTimeout(60);  // 1 minute
  setColorRGB(255,255,255);
  pulse(2000);
}

void loop() {
}

void neighborsChanged() {
  
}

