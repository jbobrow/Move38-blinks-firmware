/*
 * This test file, tests isAlone function.
 * if the tile is alone color is blue, if it has some nieghbors is yellow
 */

uint8_t blue[3] = {0,0,255};
uint8_t yellow[3] = {204,204,0};
                           
void setup() {  
   setButtonCallback(button); 
   setStepCallback(step);
}

void loop() {
  if(isAlone()){
    setColor(blue);
  }else{
    setColor(yellow);
  }
}

void button() {
}

void step() {
    // discrete time logic here
}


