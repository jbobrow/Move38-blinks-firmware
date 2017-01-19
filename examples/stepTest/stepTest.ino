// colors arranged to travel the farthest in transition between colors
uint8_t colors[7][3] = {{204,0,0},        // Red
                        {0,204,204},      // Cyan
                        {204,0,204},      // Magenta
                        {204,204,0},      // Yellow
                        {0,0,255},        // Blue
                        {204,64,0},       // Orange
                        {0,204,0}};       // Green

uint8_t curColor = 0;
uint8_t numColors = 7;

void setup() {
  // put your setup code here, to run once:
  setButtonCallback(button);
  setStepCallback(onStep);
  setState(1);
  setColor(colors[curColor]);
  setTimeout(60);
}

void loop() {
  // put your main code here, to run repeatedly:

}

void onStep() {
  curColor = (curColor+1) % numColors;
  setColor(colors[curColor]);
}

void button() {
  sendStep();
}

