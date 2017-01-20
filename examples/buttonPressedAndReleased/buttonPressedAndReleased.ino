/*
 *  Button Pressed and Released
 *    
 *  Demonstrates 3 functions
 *  1) buttonPressed() - which triggers on the down action of the button
 *  2) buttonReleased() - which triggers on the down action of the button 
 *  3) buttonLongPressed() - which triggers after a button has been held down for 1000ms 
 *  
 *  by Jonathan Bobrow
 *  01.10.2017
 */
 
void setup() {
   setColorRGB(255, 0, 0);  // red until clicked
}

void loop() {
}

void buttonPressed() {
  setColorRGB(0, 255, 0);   // green on press
}

void buttonReleased() {
  setColorRGB(0, 0, 255);   // blue on button released
}

void buttonLongPressed(){
  setColorRGB(255, 0, 0);   // red on button pressed for a "long" time
}

void buttonClicked(){
}

void buttonDoubleClicked(){
}

void buttonTripleClicked(){
}

