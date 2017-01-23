##Move38 Blinks Firmware – AutomaTile##

###Instructions for using the Arduino IDE w/ AutomaTiles###
1. Clone this repo into your Arduino directory `~/Documents/Arduino/hardware/`
2. Launch or quit and relaunch Arduino IDE
3. In the top menu, go to `Tools > Board` and select `AutomaTile`
4. Open an example and press the checkmark to make sure it compiles
5. Use an ISP of your choice and program your AutomaTiles!

If you want to work w/ this BETA library and keep the legacy library handy do the following:
1. Navigate to `~/Documents/Arduino/hardware`
2. Remove the old AutomaTiles folder (if present)
3. Get the Legacy AutomaTiles library
    1. clone https://github.com/jbobrow/AutomaTiles
    2. copy the library titled `AutomaTiles Legacy` inside of the `Arduino` folder to `~/Documents/Arduino/hardware/`
4. Launch or quit and relaunch Arduino IDE
5. In the top menu, go to `Tools > Board` and now you should see `AutomaTile` and `AutomaTile Legacy`
6. Use the appropriate API for each library, check by compiling

###API###

[Here is a link to the API](API.md) (Work in Progress)

###Examples###

The following examples are written using the AutomaTiles API for the Arduino IDE.

**blink**
An example to show how to blink with a duration `blink(duration)`

**pulse**
An example to show how to pulse with a duration `pulse(duration)`

**fadeTo**
An example to show how colors can transition smoothly between two colors `fadeTo(r, g, b, duration)`

**fadeToCycle**
An example to show how colors can transition smoothly between two colors `fadeTo(r, g, b, duration)`

**setColorRGB**
An example to show how to set the color w/ 3 values `setColorRGB(r, g, b)`
