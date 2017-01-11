/*
 * This test file, tests setColorRGB function.
 * Sweeping colors components from 0 to 255
 */
                           
void setup() {  
   setButtonCallback(button); 
   setStepCallback(step);
   setColorRGB(0, 100, 200);
   blink(1000);
}

void loop() {
}

void button() {
}

void step() {
    // discrete time logic here
}


