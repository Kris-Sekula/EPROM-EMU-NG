## Software (Python script) to control the EPROM emulator

### Currently recommended firmware and software:
* Firmware 2.0rc4 (EPROM_EMU_NG_FW_2.0rc4.ino - Arduino sketch in [Firmware](https://github.com/Kris-Sekula/EPROM-EMU-NG/tree/master/Firmware) folder)
* Software 2.0rc3 (EPROM_NG_v2.0rc3.py - Python script)

### If you want to try new features (increased speed of transfer, support for EEPROM) use the following:
* Firmware 2.0rc12 (EPROM_EMU_NG_FW_2.0rc12.ino - Arduino sketch in [Firmware](https://github.com/Kris-Sekula/EPROM-EMU-NG/tree/master/Firmware) folder)
* Software 2.0rc10 (EPROM_EMU_NG_2.0rc10.py - Python script)

Python script to control the EPROM EMULATOR NG.

The emulator will show up as a serial port on your computer, depending on the OS type and version, you may need to install drivers, follow these instructions:

https://learn.sparkfun.com/tutorials/how-to-install-ch340-drivers/all

Requires Python 3.8, and the following libraries:
* pySerial (install by running the following command on your computer ```pip install pyserial```)
* pySimpleGui ~~(```pip install pysimplegui```)~~ 4.30.0 ~~or above~~

NOTE: this has recently changed, the current PySimpleGUI library as installed via pip will not work, please follow the new instructions:
1. Download older version of library from [mygeekyhobby](https://mygeekyhobby.com/wp-content/uploads/2025/03/pysimplegui4.zip)
2. Unzip to a local folder.
3. Open command line in the folder where you downloaded the file - you should see a single file "PySimpleGUI-4.60.5.tar.gz"
4. run the following command ```pip install PySimpleGUI-4.60.5.tar.gz```


parameters like type of memory, hex file etc, should be provided via command line.

For **GUI mode**, just run the script **without parameters** and GUI will show up (of course only works in GUI environments :))

Example of parameters and output:
```
#python.exe EPROM_NG_v1.5h.py -mem 27256 -spi y -auto n 27256.HEX com4
Running EPROM EMU NG python script version 1.5h

 Using serial port COM4, emulating: 27512 EPROM

-- attempting to get sync --

-> HW: 1.4 @ 115200, FW: 1.6, SPI: 0, Auto: 0, Last EPROM: 6, mygeekyhobby.com 2020 :)

-- processing file --
-> Emulator Running.
```
