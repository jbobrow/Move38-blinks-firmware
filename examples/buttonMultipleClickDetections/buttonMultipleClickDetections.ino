/*
 *  Multiple Click Detection
 *  
 *  Demonstrates 3 functions
 *  1) buttonClicked() - which triggers after a button has been clicked once
 *  2) buttonDoubleClicked() - which triggers after a button has been clicked twice
 *  3) buttonTripleClicked() - which triggers after a button has been clicked 3 times
 *  
 *  by Jonathan Bobrow & Luis Rodriguez
 *  01.19.2017
 */
 
void setup() {
   setColorRGB(100, 0, 100);  // magenta until clicked   
}

void loop() {
}

void buttonClicked(){
  setColorRGB(255, 0, 0);  // red on single click
}

void buttonDoubleClicked(){
  setColorRGB(0, 255, 0);  // green on double click
}

void buttonTripleClicked(){
  setColorRGB(0, 0, 255); // blue on triple click
}

void buttonPressed() {
}

void buttonReleased() {
}

void buttonLongPressed(){
}
