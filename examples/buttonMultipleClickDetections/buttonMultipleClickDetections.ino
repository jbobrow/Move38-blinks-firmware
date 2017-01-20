/*
 *  FadeToCycle Example
 *  
 *  This sketch fades color from red to cyan on the button press over the course of 5 seconds
 *  
 *  
 *  by Jonathan Bobrow
 *  01.10.2017
 */
 
// colors arranged to travel the farthest in transition between colors
int colors[7][3] = {{204,0,0},        // Red
                    {0,204,204},      // Cyan
                    {204,0,204},      // Magenta
                    {204,204,0},      // Yellow
                    {0,0,255},        // Blue
                    {204,64,0},       // Orange
                    {0,204,0}};       // Green

int numColors = 7;
int index = 0;

void setup() {
   setColorRGB(100, 0, 100);
}

void loop() {
}

void buttonPressed() {
//  setColorRGB(0, 0, 255); 
}

void buttonReleased() {
}

void buttonLongPressed(){
}

void buttonClicked(void){
  setColorRGB(255, 0, 0);
}
void buttonDoubleClicked(void){
  setColorRGB(0, 255, 0);  
}
void buttonTripleClicked(void){
  setColorRGB(0, 0, 255); 
}

