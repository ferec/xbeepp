# xbeepp
xbee handling library in C++, used in andam iControl. For details see www.andam-systems.com

features:
- basic device discovery - must be already on the same network PAN ID
- discovery through callback
- setup port function
- basic GPIO and analog input handling: read input and write output latches
- basic information about local and remote xbee devices
- starts its own thread for each local device (serial port)

requirements:
- 1 local xbee plugged in USB (default /dev/ttyUSB0, see test.cpp)
- c++11 compiler
- code:blocks for compilation (I use it for development) or create a custom makefile (please share it with others)

If you have any question, or need some feature, feel free to ask.
