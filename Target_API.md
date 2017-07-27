## Blinks API

A simple API for programming Blink tiles to perform needed tasks
**A default blank sketch**
```c
void setup() {  
}

void loop() {
	// continuous time logic here
}

void buttonClicked(){
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

void onStep() {
	// (Work In Progress)
	// discrete time logic here
}

void neighborChanged() {
	// (Work In Progress)
	// handles when a neighbor is changed
	// (should also know which neighbors have been changed)
}


```

### Blink global variables

**neighbors**
```c
int neighbors[6];
// publicly accessible neighbor array, allows for quick simple access to any of the neighbors at all times
// (Work In Progress)
// for the time being, call getNeighbor(i) to get a neighbor's state
```

### Blink methods
**setState**
```c
void setState(int n);
// sets the local state of the tile, which is communicated to other tiles
```

**setSideState [EXPERIMENTAL]**
```c
void setSideState(int sideID, int n);
// sets the state on a specific side
// this could be used for telling different neighbors different things (too much possibility here)
```

**getState**
```c
int getState();
// gets the local state of the tile, which is communicated to other tiles
```

**setColor**
```c
void setColor(int r, int g, int b);
// instantly changes the color of the RGB LED to the values passed
```

**setSideColor**
```c
void setColorSide(int sideID, int r, int g, int b);
// instantly changes the color of the RGB LED on a single side to the values passed
```

**getSideColor**
```c
int[3] getColorSide(int sideID);
// returns the current RGB values of the specific LED in an array
```

**sendStep**
```c
void sendStep();
// communicates for all connected neighbors to step forward in discrete time
// used for games that are not based on real-time and need to globally update the board at "the same time"
```

**isAlone**
```c
boolean isAlone();
// returns true when the tile has no neighbors
// same as checking all 6 sides and seeing that they return 0
```

### Blink display manager methods
These functions should not be used in `loop` since they will handle animation on their own.
Use them in a callback, for example, when the button is pressed, `fade(255, 0, 0, 500);` will turn red over the course of half a second. Calling two fade functions in a row will add them to the queue and animations will happen in sequence. setColor automatically clears the queue.

**clear**
```c
void clear(); // clear stops any animation and is basically a setColor() on each LED to pause where it is at and to empty the queue of animations
```

**fadeTo**
```c
void fade(int r, int g, int b, int ms);  // timed change to color

void fadeSide(int sideID, int r, int g, int b, int ms);  // timed change to color for a single side
```

**blink**
```c
void blink(int ms); // defaults to on/off of current color

void blinkTo(int r, int g, int b, int ms); // blinks between current color and color passed as parameter (i.e. red and blue flashing)

void blinkSide(int sideID, int ms); // defaults to on/off of current color

void blinkSideTo(int sideID, int r, int g, int b, int ms); // blinks between current color and color passed for a specific side

```

**pulse**
```c
void pulse(int ms); // pulses between on/off of current color over a period(ms)

void pulseTo(int r, int g, int b, int ms); // pulses between on/off of current color over a period(ms)

void pulseSide(int sideID, int ms); // pulses between on/off of current color over a period(ms) for a specific side

void pulseSideTo(int sideID, int r, int g, int b, int ms); // pulses between on/off of current color over a period(ms) for a specific side
```

### Blink callbacks

**neighborChanged**
```c
void neighborChanged(int[] neighborID);
// handles when a neighbor is changed
// (should also know which neighbors have been changed)
// array of neighborIDs that changed
// another option could be to call this function for each neighbor as it changes... but that could set up weird race conditions
```

**onStep**
```c
void onStep();
// handles a discrete step
// (i.e. received a sent step from a neighbor, or called a step was invoked locally)
```

**onWake**
```c
void onWake();
// called once when woken (either by button press or neighbor)
```

**onSleep**
```c
void onSleep();
// called once just before going to sleep (could be a good time to store information if in memory and needed later...)
```

**button**
```c
void buttonDown();
void buttonUp();

void buttonClicked();
void buttonDoubleClicked();
void buttonTripleClicked();

void setButtonClickThreshold(int ms); // defaulted to 330ms, but function available to make slower or faster clicking part of the game

void buttonLongPressed();
void setButtonLongThreshold(int ms); // defaulted to 1000ms

```
