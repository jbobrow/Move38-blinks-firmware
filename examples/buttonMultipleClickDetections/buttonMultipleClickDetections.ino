/*
 *  Multiple Click Detection
 *
 *  Demonstrates 3 functions
 *  1) buttonClicked() - which triggers after a button has been clicked once
 *  2) buttonDoubleClicked() - which triggers after a button has been clicked twice
 *  3) buttonTripleClicked() - which triggers after a button has been clicked 3 times
 *
 *  Note: states to communicate colors to neighboring tiles are chosen to match
 *        the color gossip set of colors: red(1), cyan(2), magenta(3), yellow(4),...
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
  setState(1);             // communicates to another tile that it is red
}

void buttonDoubleClicked(){
  setColorRGB(0, 255, 0);  // green on double click
  setState(7);             // communicates to another tile that it is green
}

void buttonTripleClicked(){
  setColorRGB(0, 0, 255); // blue on triple click
  setState(5);            // communicates to another tile that it is blue
}

void buttonPressed() {
}

void buttonReleased() {
}

void buttonLongPressed(){
}
