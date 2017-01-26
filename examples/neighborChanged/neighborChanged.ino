/*
 *  Neighbors Changed
 *  
 *  This examples shows how to discretely call a function when the neighbors are changed
 *  
 *  by Jonathan Bobrow
 *  01.26.2017
 */

uint8_t neighbors[6];
uint8_t prevNeighbors[6];

void setup() {
   //setButtonPressedCallback(buttonPressed);
   setColorRGB(255, 0, 0);
   setState(1);
   setTimeout(60);
}

void loop() {
  
  boolean didChange = false;

  getNeighborStates(neighbors);
  
  for(int i=0; i<6; i++) {
    if(neighbors[i] != prevNeighbors[i]) {
      didChange = true;
      break;
    }
  }
  
  for(int i=0; i<6; i++) {
    prevNeighbors[i] = neighbors[i];
  }
  
  if(didChange) {
    neighborChanged();
  }
}

void neighborChanged(){
  // act on the neighbor changing... blink
  setColorRGB(255, 255, 0);
  blink(200);
}

void buttonClicked(){
}

void buttonDoubleClicked(){
}

void buttonTripleClicked(){
}

void buttonPressed() {
  // reset the tile to not blink
  setColorRGB(0, 255, 0);
}

void buttonReleased() {
}

void buttonLongPressed(){
}
