##Blinks API##

A simple API for programming Blink tiles to perform needed tasks
**A default blank sketch**
```c
void setup() {  
}

void loop() {
	// continuous time logic here
}

void neighborChanged() {
	// handles when a neighbor is changed
	// (should also know which neighbors have been changed)
}

void buttonPressed() {
	// handle button down here
}

void onStep() {
	// discrete time logic here
}

```

###Blink global variables###

**neighbors**
```c
int neighbors[6];
// publicly accessible neighbor array, allows for quick simple access to any of the neighbors at all times
```

###Blink methods###
**setState**
```c
void setState(int n);
// sets the local state of the tile, which is communicated to other tiles ~30fps
```

**setColor**
```c
void setColor(int r, int g, int b);
// instantly changes the color of the RGB LED to the values passed
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

###Blink display manager methods###
These functions should not be used in `loop` since they will handle animation on their own.
Use them in a callback, for example, when the button is pressed, `fadeToAndReturn(255, 0, 0, 500);` will turn red over the course of half a second and then return to its previous color.

**fadeTo**
```c
void fadeTo(int r, int g, int b, int ms);  // timed change to color

void fadeToAndReturn(int r, int g, int b, int ms);  // timed change to color and back
```

**blink**
```c
void blink(int ms); // defaults to on/off of current color

void blink(int ms, int min, int max); // TODO: low and high levels for blinking and the time between them

void blink(int ms, int[n][3] c); // TODO: (low priority) send array of colors to blink between
```

**pulse**
```c
void pulse(int ms); // phase

void pulse(int ms, int min, int max); // TODO: phase w/ low and high brightness

void pulse(int ms, int[n][3] c); // TODO: phased pulse between colors (depends on fadeTo)
```

###Blink callbacks###

**neighborChanged**
```c
void neighborChanged();
// handles when a neighbor is changed
// (should also know which neighbors have been changed)
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
void buttonPressed();
void buttonReleased();

void setButtonClickThreshold(int ms); // defaulted to 200ms, but function available to make slower or faster clicking part of the game

void buttonClicked();
void buttonDoubleClicked();
void buttonTripleClicked();

void buttonLongPressed();
```
