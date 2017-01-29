uint32_t lastUpdateTime = 0;
uint16_t updateFrequency = 30;  //milliseconds

uint8_t infectionCounter = 0;

// State Index
uint8_t empty     = 1; // Ball can move here
uint8_t hasBall   = 2; // Ball is here
uint8_t passing   = 3; // Has ball, ready to move it
uint8_t hadBall   = 4; // Can't get ball next
uint8_t dead      = 5; // Ball can't move here, infects neighbors
uint8_t blank     = 6; // Nothing going on here

uint8_t stateColors[ 6 ][3] = {  
  {0, 200, 20},    // empty    : 1,
  {255, 255, 255}, // hasBall  : 2,
  {100, 100, 200}, // passing  : 3,
  {50, 50, 50},    // hadBall  : 4,
  {250, 0, 0},     // dead     : 5,
  {0, 0, 0}        // dark     : 6
};

// Click locks
boolean longPress = false;

uint16_t tickDown = 0;

void set_state_color() {
  uint8_t r = stateColors[ getState() - 1 ][ 0 ];
  uint8_t g = stateColors[ getState() - 1 ][ 1 ];
  uint8_t b = stateColors[ getState() - 1 ][ 2 ];
  setColorRGB(r, g, b);
}

void my_set_state( uint8_t stateIndex ) {
  setState( stateIndex );
  set_state_color();
  if ( stateIndex == dead ) {
    tickDown = 150;
  } else if ( stateIndex == passing ) {
    tickDown = 200;
  } else if ( stateIndex == hadBall ) {
    tickDown = 100;
  } else if ( stateIndex == hasBall ) {
    tickDown = 100;
  }      
}

void setup() {
  setTimeout( 60000 );
  my_set_state( empty );   
}

uint8_t neighborhoodValue = 0; 
boolean wasAlone = false;

void loop() {
  uint32_t curTime = getTimer();
  if(curTime - lastUpdateTime > updateFrequency) {
    srand( curTime - lastUpdateTime );
    tickDown--;
    neighborhoodValue = 0;
    for ( uint8_t i = 0; i < 6; i++ ) {
      neighborhoodValue = neighborhoodValue + getNeighbor( i );
      if ( getNeighbor( i ) == dead) {
        infectionCounter++;
        if ( infectionCounter > 60 ) {
          my_set_state( dead );       
          infectionCounter = 0;
          return;
        }        
      }
    }
    if ( neighborhoodValue == 0 ) {
      wasAlone = true;      
    } else { // This random factor makes the game unplayable
      wasAlone = false;
      if ( rand() % 100 < 10 ) {
        my_set_state( dead );
      }
    }    
    
    uint8_t state = getState();
    if( state == hasBall ) {
      if ( tickDown == 0 ) {
        my_set_state( dead );
        return;
      }
      if ( tickDown < 75 ) {
        for ( uint8_t i = 0; i < 6; i++ ) {
          if ( getNeighbor( i ) == empty ) {
            my_set_state( passing );          
            return;
          }
        }
      }
    }  
    else if ( state == passing ) {
      if ( tickDown == 0 ) {
        my_set_state( dead );
        return;
      }
      for ( uint8_t i = 0; i < 6; i++ ) {
        if ( getNeighbor( i ) == hasBall ) {
          my_set_state( hadBall );          
          return;
        }
      }
    }
    else if ( state == hadBall && tickDown == 0 ) {
      my_set_state( empty );
    }
    else if ( state == dead && tickDown == 0 ) {
      my_set_state( empty );
    }
    else if ( state == empty ) {
      for ( uint8_t i = 0; i < 6; i++ ) {
        if ( getNeighbor( i ) == passing ) {
          my_set_state( hasBall );          
          return;
        }
      }
    }
    lastUpdateTime = curTime;
  }
}

void buttonDoubleClicked(){}
void buttonTripleClicked(){}
void buttonPressed() {}
void buttonReleased() {} // Fires before buttonClicked

// Use long press to rest the tile
void buttonLongPressed() {
  my_set_state( dead );
  longPress = true;
}
  
void buttonClicked() {
  if ( longPress ) {
    longPress = false;
  }
  add_ball();
}

void add_ball() {
  my_set_state( hasBall );
}
