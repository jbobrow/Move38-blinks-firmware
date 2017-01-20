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
   //setButtonPressedCallback(buttonPressed);
   setColorRGB(255, 0, 0);
}

void loop() {
}

void buttonClicked(){
}

void buttonDoubleClicked(){
}

void buttonTripleClicked(){
}

void buttonPressed() {
   index = (index+1) % numColors;
   
   fadeTo(colors[index][0], // R value
          colors[index][1], // G value
          colors[index][2], // B value
          1000);            // 1 second
}

void buttonReleased() {
}

void buttonLongPressed(){
}
