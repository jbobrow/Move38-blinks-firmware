/*
 *  Multiple Side Detection
 *  
 *  Demonstrates 6 unique colors for 6 neighbors
 *  
 *  by Jonathan Bobrow
 *  01.19.2017
 */
 
void setup() {
   setColorRGB(32, 32, 32);  // dim white until clicked
   setTimeout(60);
   setState(1);
}

void loop() {
  for(int i=0; i<6; i++) {
    if(getNeighbor(i) != 0) {
      switch(i) {
        case 0: setColorRGB(255,0,0); break;
        case 1: setColorRGB(255,0,255); break;
        case 2: setColorRGB(0,0,255); break;
        case 3: setColorRGB(0,255,255); break;
        case 4: setColorRGB(0,255,0); break;
        case 5: setColorRGB(255,255,0); break;
      }
    }
  }
}

void buttonClicked(){
  setColorRGB(64,64,64);  // white on single click
}

void buttonDoubleClicked(){
  setColorRGB(128,128,128);  // brighter white on double click
}

void buttonTripleClicked(){
  setColorRGB(255, 255, 255); // brighter white on triple click
}

void buttonPressed() {
}

void buttonReleased() {
}

void buttonLongPressed(){
}
