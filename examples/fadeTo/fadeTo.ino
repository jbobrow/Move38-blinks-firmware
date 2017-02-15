/*
 *  FadeTo Example
 *  
 *  This sketch fades color from red to cyan on the button press over the course of 5 seconds
 *  
 *  by Jonathan Bobrow
 *  01.10.2017
 */
 
void setup() {
   setColorRGB(0,255,0);
   int i = 0;
   fadeTo(0,0,255, 4000);
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
   setColorRGB(255, 123, 0);
   fadeTo(0,0,255, 4000);

}

void buttonReleased() {
}

void buttonLongPressed(){
}

