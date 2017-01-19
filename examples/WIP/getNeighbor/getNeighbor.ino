/*
 * This test file, tests getNighbor function.
 * changes tile state by pressing the button, and copies colors of neighbor tiles that change their state
 */

#define MAX_STATE  7
#define SIDE_TILES  6

// colors arranged to travel the farthest in transition between colors
uint8_t colors[MAX_STATE][3] = {{204,0,0},        // Red
                                {0,204,204},      // Cyan
                                {204,0,204},      // Magenta
                                {204,204,0},      // Yellow
                                {0,0,255},        // Blue
                                {204,64,0},       // Orange
                                {0,204,0}};       // Green

uint8_t currState = 1;  // Current state for the tile
uint8_t currNieghborState[MAX_STATE] = {0,0,0,0,0,0,0}; // Current state for neighbor tiles
uint8_t prevNieghborState[MAX_STATE] = {0,0,0,0,0,0,0}; // Previous state for nieghbor tiles

void setup() {  
   setButtonCallback(button); 
   setStepCallback(step);
}

void loop() {

  for(uint8_t i=0; i< SIDE_TILES; i++){
    currNieghborState[i] = getNeighbor(i);

    // If a neighbor has changed state, copy color, state and update nighbor state
    if(prevNieghborState[i] != currNieghborState[i]) {
      setColor(colors[currNieghborState[i]]);
      currState = currNieghborState[i];
      prevNieghborState[i] = currNieghborState[i];
    }
  }

}

void button() {
    // Increate or reset tile current state after button has been pressed
    currState++;
    if (currState < MAX_STATE){
      currState=0;
    }
    // Update tile color
    setColor(colors[currState]);
}

void step() {
    // discrete time logic here
}


