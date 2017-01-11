##Move38 Blinks Firmware – AutomaTile##

###Instructions for using the Arduino IDE w/ AutomaTiles###
1. Clone this repo into a folder named `AutomaTile` in your Arduino directory i.e. `~/Documents/Arduino/hardware/`
2. launch or quit and relaunch Arduino IDE (should be hot swappable)
3. in the top menu, go to `Tools > Board` and select `AutomaTile`
4. open an example and press the checkmark to make sure it compiles
5. Use an ISP of your choice and program your AutomaTiles!

###API###

[Here is a link to the API](API.md) (Work in Progress)

###Examples###

The following examples are written using the AutomaTiles API for the Arduino IDE.

**blink**
An example to show how to blink with a duration `blink(duration)`

**pulse**
An example to show how to pulse with a duration `pulse(duration)`

**fadeTo**
An example to show how colors can transition smoothly between two colors `fadeTo(r, g, b, duration)` **-WIP-**

**getNeighbor**

**isAlone**

**setColor**

**setColorRGB**
