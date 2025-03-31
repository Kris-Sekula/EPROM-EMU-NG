## Software (Python script) to control the EPROM emulator

### Currently recommended firmware and software:
* Firmware 2.0rc4 (EPROM_EMU_NG_FW_2.0rc4.ino - Arduino sketch in [Firmware](https://github.com/Kris-Sekula/EPROM-EMU-NG/tree/master/Firmware) folder)
* Software 2.0rc4 (EPROM_NG_v2.0rc4.py - Python script)

### If you want to try new features (increased speed of transfer, support for EEPROM), use the following:
* Firmware 2.0rc12 (EPROM_EMU_NG_FW_2.0rc12.ino - Arduino sketch in [Firmware](https://github.com/Kris-Sekula/EPROM-EMU-NG/tree/master/Firmware) folder)
* Software 2.0rc10 (EPROM_EMU_NG_2.0rc10.py - Python script)

To successfully use the EPROM Emulator, you must do the following:
1. Build or purchase the device
2. Install drivers
3. Install Python
4. Install required Python libraries
5. Run the script in command line mode (CLI) or in graphical interface mode (GUI)

### 1. Build or purchase the device
Follow the instructions in the Hardware section or purchase on eBay.

### 2. Install Drivers
If you're using a genuine Arduino Nano module, install the Arduino IDE (this will install the drivers for all modules).
Most people use the much cheaper "clones", those in most cases, use the CH340 chip.
When plugged to a computer, they are supposed to "show up" as a "serial port device", but in many cases, driver installation is required. 

Sparkfun created a dedicated page that will guide you through the installation of CH340 drivers:

https://learn.sparkfun.com/tutorials/how-to-install-ch340-drivers/all

### 3. Install Python
The base way to install Python is to download the installation file from https://www.python.org/downloads/

> [!TIP]
> The installer has an option called "Add python to PATH", make sure you select it, this makes executing scripts much easier.

### 4. Install required libraries
There are two essential libraries that you must install:

* **pyserial** - to enable serial communication in Python
* **FreeSimpleGUI** - to allow a Python script to display a graphical user interface (GUI)

In Python, we install the libraries by calling a "package manager" called PIP. In your command prompt, you need to run:

```python -m pip install pyserial```

and 

```python -m pip install FreeSimpleGUI```

### 5.1 Run the script - CLI mode
You can run the script in a command-line mode by supplying the required parameters. Here is an example of the minimum required parameters to run the script:

```python.exe EPROM_NG_v2.0rc4.py -mem 27512 blink.bin COM13```

Parameters are explained:

"-mem 27512" -> Type of emulated memory

"blink.bin" -> file getting transferred (we support binary and intel hex files)

"COM13" -> serial port the emulator is plugged in

Here is a full output of the "upload session":
```
C:\PyDev>python.exe EPROM_NG_v2.0rc4.py -mem 27512 blink.bin COM13

Running EPROM EMU NG python script version 2.0rc4

File used: blink.bin

Done processing bin

Memory map disabled


Using serial port COM13, emulating: 27512 EPROM

-- attempting to get sync --

-> HW: v2.2h @ 115200, FW: 2.0rc4, SPI: 0, Auto: 0, Last EPROM: 27512, mygeekyhobby.com 2020 :)

-- processing file --

0x7E00 ->in

Time taken to transfer: 4.79s

-- Setting EPROM Type --
ACK

-- Finished --
-> Bus set to 27512 EPROM Memory
```

## 5.2 Run the script - GUI mode
If you want to start the script in graphical mode, **simply start it without any parameters**:

```python.exe EPROM_NG_v2.0rc4.py```

The script will open a "window":

![GUIMODE](https://github.com/Kris-Sekula/EPROM-EMU-NG/blob/master/Pictures/running_GUI_mode.JPG)


> [!NOTE]
> Depending on how Python is installed on your system, you may be able to simply "double-click" on the script to start it in GUI mode.





