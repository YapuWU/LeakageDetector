# LeakageDetector
Using ATTINY13A to detect water leakage

FUSE should be set to Fast Rising Power, this will cosume around 4ms to bring the MCU back from power down mode

PB3  LED  -  High to turn on LED  
PB4  BUZZ -  High to turn on BUZZ  
PB1  Connects to Groud  
PB0  Connect to PB1 via a resistor, Also it is a probe pin for the detector  
POWER  This is another probe pin for the detector   


If a water spill in two probe pins, PB0's voltage will be higher than PB1, then it will trigger the buzz.  

can connect multiple probe pins to the two pins to detect multiple area.   

