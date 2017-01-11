/*
 * This test file, tests setColorRGB function.
 * Sweeping colors components from 0 to 255
 */

uint8_t blue[3] = {0,0,255};
uint8_t yellow[3] = {204,204,0};
                           
void setup() {  
   setButtonCallback(button); 
   setStepCallback(step);
}

void loop() {
  for(uint8_t i =0; i<255; i++){
    for(uint8_t j =0; j<255; j++){
      for(uint8_t k =0; k<255; k++){
        setColorRGB(i, j, k);
        // Not sure why delay has been commented from Arduino.h
        //delay(10);
      }
    }
  }
}

void button() {
}

void step() {
    // discrete time logic here
}


