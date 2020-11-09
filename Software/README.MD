Python script to control the EPROM EMULATOR NG.

Requires python 3.8, and the following libraries:
* pySerial (pip install pyserial)
* pySimpleGui (pip install pysimplegui) 4.30.0 or above

parameters like type of memory, hex file etc should be provided via command line.

For GUI, just run the script without parameters and GUI will show up (of course only works in GUI environments :))

Example of parameters and output:
```
#python.exe EPROM_NG_v1.5h.py -mem 27256 -spi y -auto n 27256.HEX com4
Running EPROM EMU NG python script version 1.5h

 Using serial port COM4, emulating: 27512 EPROM

-- attempting to get sync --

-> HW: 1.4 @ 115200, FW: 1.6, SPI: 0, Auto: 0, Last EPROM: 6, mygeekyhobby.com 2020 :)

-- prcessing file --
-> Emulator Running.
```

As of Nov 8 2020, it is recommended to use the 2.0rc1 version of the Python script (and a matching 2.0rc1 version of the Arduino sketch) for the emulator. Please read the release notes.
