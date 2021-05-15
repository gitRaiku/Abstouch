# Abstouch

Linux utility to let you turn your touchpad into an absolute input device like a graphical tablet. 

You can find the TOUCHPAD\_INPUT\_ID by running [evtest](https://cgit.freedesktop.org/evtest/) [mirror](https://github.com/freedesktop-unofficial-mirror/evtest) and by finding the id of your touchpad. After this just edit the line
```
#define TOUCHPAD_INPUT_ID "<id>"
``` 
by replacing \<id\> with the id

You can then change SXMAX and SYMAX by looking at the initial output of evtest.

After running evtest and selecting your touchpad you shuold see something like this
```
    Event code 53 (ABS_MT_POSITION_X)
      Value      0
      Min        0
      Max     1236
      Resolution      12
    Event code 54 (ABS_MT_POSITION_Y)
      Value      0
      Min        0
      Max      896
      Resolution      12
```

Simply copy the value from ABS\_MT\_POSITION\_X -> max to SXMAX and ABS\_MT\_POSITION\_Y -> max to SYMAX.

Your monitor width and height can be found by running xrandr and changing
```
#define MONITOR_WIDTH 1920
#define MONITOR_HEIGHT 1080
```
accordingly.

While i could make everything dynamic and spare you the trouble of finding all these values yourself i'm too lazy so i won't.
