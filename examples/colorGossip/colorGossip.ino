/*  Color Gossip (Visualization)
 *  
 *  Pressing on an AutomaTile assigns it a new color and it infects its neighbors
 *  with the color as well. Colors should animate through the HSV spectrum to arrive
 *  at their new color.
 *
 *  --------------------------------------------------------------------------------------------------
 *  IMPORTANT: To use this code in Arduino's IDE, first move the AutomaTiles folder
 *  into the right directory i.e. <user home directory>/Documents/Arduino/hardware/AutomaTiles
 *  Then open the Arduino IDE and select Tools > Board > "AutomaTile"
 *  Now you should be good to go :) (thanks to the hard work of Joshua Sloane / Josh Levine / Luis Rodriguez)
 *  -------------------------------------------------------------------------------------------------- 
 *     
 *  by Jonathan Bobrow
 *  01.21.2017
 */
 
// colors arranged to travel the farthest in transition between colors
int colors[7][3] = {{204,0,0},        // Red
                    {0,204,204},      // Cyan
                    {204,0,204},      // Magenta
                    {204,204,0},      // Yellow
                    {0,0,255},        // Blue
                    {204,64,0},       // Orange
                    {0,204,0}};       // Green

int numColors = 7;
int index = 0;

int waitTimeForStateChange = 2000;
int timeSinceLastStateChange = 0;
int curState = 1;
int prevState = 1;
int curTime = 0;

int aloneCount = 0;
int aloneThreshold = 10;
int bAlone = 0;

void setup() {
   //setButtonPressedCallback(buttonPressed);
   setColorRGB(255, 0, 0);
   setState(1);
   setTimeout(300); // 5 minutes before sleep
}

void loop() {
    curTime = getTimer();

    if(aloneCount < aloneThreshold)
      aloneCount++;

    for(int i=0; i<6; i++) {
      if(getNeighbor(i) != 0) {
        aloneCount = 0;
        if(curTime - timeSinceLastStateChange > waitTimeForStateChange) {
          if(getNeighbor(i) != curState ) {
            curState = getNeighbor(i);
            setState(curState);
          }
        }
      }
    }
    
    if(aloneCount >= aloneThreshold) {
      if(!bAlone) {
        bAlone = 1;  // we are confirmed alone... but we enjoy the solidarity, pulse calmly in contemplation :)
        //pulse white
        setColorRGB(255,255,255);
        pulse(2000);
      }
    }
    else {
      bAlone = 0;  // no longer alone

      if( prevState != curState) {
        // new state, fade to new color
        // can't fade from white... yet, so lets start from a hue
        // start color                                          
        setColorRGB(colors[prevState-1][0],   // R value
                    colors[prevState-1][1],   // G value
                    colors[prevState-1][2]);  // B value
        
        // finish color         
        fadeTo(colors[curState-1][0],   // R value
               colors[curState-1][1],   // G value
               colors[curState-1][2],   // B value
               2000);                   // duration (ms)
        prevState = curState;
      }
    }
    

}

void buttonClicked(){
   prevState = getState();
   setState(getState() % numColors + 1);
   curState = getState();
   timeSinceLastStateChange = curTime;
}

void buttonDoubleClicked(){
}

void buttonTripleClicked(){
}

void buttonPressed() {
}

void buttonReleased() {
}

void buttonLongPressed(){
}
