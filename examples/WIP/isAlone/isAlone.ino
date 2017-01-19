/*
 * This test file, tests isAlone function.
 * if the tile is alone color is blue, if it has some nieghbors is yellow
 */

void setup() {  
  setState(1);
}

void loop() {
  if(isAlone()){
    // blue if alone
    setColorRGB(0,0,255);    
  }else{
    // yellow w/ neighbors
    setColorRGB(255,255,0);
  }
}

// eventually move this to neighborsChanged...
