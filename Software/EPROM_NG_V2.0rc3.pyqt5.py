#!/usr/bin/env python3
# kris@mygeekyhobby.com
#
# ver 1.0 - may 2020	- initial release
# ver 1.2 - june 2020 	- added save to SPI EEPROM functionality
#						- fixed the args parsing
#						- added auto load wait option
#						- added save SPI option
# ver 1.3 - july 2020	- removed some of the debug stuff
#						- removed address scrolling when loading
# ver 1.6g - Aug 2020	- Launches GUI when no parameters specified (code contributed by John Gerrard)... requires PySimpleGUI module "pip install PySimpleGUI"
#						- small fixes to the output
# ver 1.6e              - fixed bug with SPI==y 
#
# ver 2.0rc1 - Oct 2020   	- save parameters in GUI
#							- .bin and .hex file support
#							- binary mode upload with packets of 128-512 bytes at a time
#							- Save to SPI works with any type of data range (any source file less than 128 bytes gets "padded" with 0xFF)
# ver 2.0rc2 - Oct 2020   	- support other extensions like .rom .img etc... everything not .hex is treated as binary (thanks to Hubert for the feedback and contribution)
#                           - fixed bug where a 128Byte of "00h" was treated as "no data" (thanks to Hubert for the feedback and contribution)
# ver 2.0rc3 - Oct 2020     - code from rc1 that was missing in rc2 was added back, one typo fixed (thx again Hubert)   	
# ver 2.0rc3.pyqt - Mar 2025     - Changed from pysimplegui to pyqt5 -jrork (needs PyQt5 installed: pip install PyQt5) 

version = "2.0rc3.pyqt"

import argparse
import serial, os
import time
import sys
from serial.tools.list_ports import comports

# --------------------------
# Helper functions below
# --------------------------
def PrintHex(intList):
    row = 0
    for val in intList:
        print("%0.2X" % val, end=' ', flush=True)
        row += 1
        if row == 64:
            print("")
            row = 0

def chunkData(bfr):
    """
    Analyze the 64k buffer, find all 128Byte chunks that are in use (have data != to -1)
    Will return a list of 512 entries with "1" meaning block of 128Bytes had data changed and "0" no data changed
    """
    mapped = []
    for i in range(0, len(bfr), 128):
        chunk = bfr[i:i+128]
        used = False
        for cell in chunk:
            if cell >= 0:
                used = True
                break
        mapped.append(1 if used else 0)
    return mapped

def plotData(mapped):
    """
    Display map of data usage in the buffer, "*" data used and "_" not used
    """
    col = 0
    for idx in range(len(mapped)):
        if idx == 0:
            print("0x%0.4X" % idx, end=' ', flush=True)
        print("*" if mapped[idx] == 1 else "_", end=' ', flush=True)
        col += 1
        if col == 32:
            if (idx * 128 + 128) < 65535:
                print("")
                print("0x%0.4X" % int(idx * 128 + 128), end=' ', flush=True)
            col = 0
    return

def packetData(mapped):
    data = []
    n = 0
    start = -2
    for idx in range(len(mapped)):
        if mapped[idx] == 1:
            if n == 0:
                start = idx * 128
            n += 1
            if n == 4:
                data.append((start, 512))
                n = 0
        else:
            if n > 0:
                start = (idx - n) * 128
                data.append((start, n * 128))
            n = 0
            start = -2
    return data

def Decode(length, sum_val, addr, data):
    """
    Decode an Intel string.
    Returns a list of N bytes.
    Returns an empty list if an error has occurred.
    """
    global Errors
    checksum = length + (addr & 0xFF) + (addr >> 8)
    index = 0
    bytes_out = []
    if length <= 0 or length > 32:
        print("Illegal line length found.")
        Errors += 1
        return bytes_out
    while length > 0:
        decimal = int(data[index:index+2], 16)
        bytes_out.append(decimal)
        checksum += decimal
        index += 2
        length -= 1
    checksum = (checksum + int(sum_val, 16)) & 0xFF
    if checksum != 0:
        print("Checksum error.")
        Errors += 1
        return []
    return bytes_out

def ParseIntelLine(line):
    """
    Parse an IntelHex line.
    Record types 02, 03, 04 and 05 are ignored.
    Maximum payload length is 32 bytes.
    Everything after the checksum is ignored.
    Returns True if EOF record found, or when an error is found.
    """
    global Errors
    line = line.strip()
    if len(line) == 0:
        return False
    line = line + ' ' * 74
    if line[0] in ('#', ';'):
        return False
    if line[0] != ":":
        print("Illegal start of formatted line.")
        Errors += 1
        return True
    if line[7:9] in ('02', '03', '04', '05'):
        return False
    if line[0:11] == ":00000001FF":
        return True
    datalen = int(line[1:3], 16)
    checksum_index = (datalen + 4) * 2 + 1
    checksum = line[checksum_index:checksum_index + 2]
    address = int(line[3:7], 16)
    data = line[9:checksum_index]
    bytes_line = Decode(datalen, checksum, address, data)
    if len(bytes_line) == 0:
        return True
    for x in range(0, len(bytes_line)):
        buff64k[x + address] = bytes_line[x]
    return False

# --------------------------
# PyQt GUI function
# --------------------------
# This function creates a PyQt5 dialog to collect user inputs.
def get_pyqt_values():
    from PyQt5.QtWidgets import (
        QApplication, QDialog, QLabel, QLineEdit, QComboBox, QCheckBox,
        QPushButton, QFileDialog, QMessageBox, QGridLayout
    )
    
    class InputDialog(QDialog):
        def __init__(self):
            super().__init__()
            self.setWindowTitle("EPROM EMU NG GUI Uploader " + version)
            self.initUI()
            
        def initUI(self):
            layout = QGridLayout(self)
            
            # EPROM Type
            layout.addWidget(QLabel("Eprom Type:"), 0, 0)
            self.mem_cb = QComboBox(self)
            self.mem_cb.addItems(['2716', '2732', '2764', '27128', '27256', '27512'])
            self.mem_cb.setCurrentText("27512")
            layout.addWidget(self.mem_cb, 0, 1)
            
            # COM Port
            layout.addWidget(QLabel("COM Port:"), 1, 0)
            self.port_cb = QComboBox(self)
            ports = [p.device for p in comports()]
            self.port_cb.addItems(ports)
            if ports:
                self.port_cb.setCurrentIndex(0)
            layout.addWidget(self.port_cb, 1, 1)
            
            # Checkboxes: Save to SPI and Show Data Map
            self.spi_cb = QCheckBox("Save to SPI", self)
            layout.addWidget(self.spi_cb, 2, 0)
            self.map_cb = QCheckBox("Show Data Map", self)
            self.map_cb.setChecked(True)
            layout.addWidget(self.map_cb, 2, 1)
            
            # Checkbox: Auto Start
            self.auto_cb = QCheckBox("Auto Start", self)
            layout.addWidget(self.auto_cb, 3, 0)
            
            # Start Address for binary files
            layout.addWidget(QLabel("Start Address (for binary files):"), 4, 0)
            self.startaddr_le = QLineEdit(self)
            self.startaddr_le.setText("0x0000")
            layout.addWidget(self.startaddr_le, 4, 1)
            
            # File selection
            layout.addWidget(QLabel("Choose A File:"), 5, 0)
            self.filename_le = QLineEdit(self)
            layout.addWidget(self.filename_le, 5, 1)
            self.browse_btn = QPushButton("Browse", self)
            layout.addWidget(self.browse_btn, 5, 2)
            self.browse_btn.clicked.connect(self.browse_file)
            
            # Submit and Cancel buttons
            self.submit_btn = QPushButton("Submit", self)
            self.cancel_btn = QPushButton("Cancel", self)
            layout.addWidget(self.submit_btn, 6, 0)
            layout.addWidget(self.cancel_btn, 6, 1)
            
            self.submit_btn.clicked.connect(self.on_submit)
            self.cancel_btn.clicked.connect(self.reject)
            
        def browse_file(self):
            file_path, _ = QFileDialog.getOpenFileName(
                self, "Select a file",
                "", "iHex files (*.hex);;Binary (*.bin);;Image (*.img);;All files (*.*)"
            )
            if file_path:
                self.filename_le.setText(file_path)
        
        def on_submit(self):
            if not self.filename_le.text():
                QMessageBox.critical(self, "Error", "Please select a file")
                return
            self.accept()
    
    app = QApplication.instance()
    if not app:
        app = QApplication(sys.argv)
    dialog = InputDialog()
    if dialog.exec_() == QDialog.Accepted:
        return {
            'mem': dialog.mem_cb.currentText(),
            'port': dialog.port_cb.currentText(),
            'spi': dialog.spi_cb.isChecked(),
            'map': dialog.map_cb.isChecked(),
            'auto': dialog.auto_cb.isChecked(),
            'startaddress': dialog.startaddr_le.text(),
            'filename': dialog.filename_le.text()
        }
    else:
        sys.exit(0)

# --------------------------
# Main script starts here
# --------------------------
if len(sys.argv) >= 2:
    parser = argparse.ArgumentParser(
        description='This script sends data to EPROM Emulator over serial port, requires python 3.8, pyserial and PyQt5'
    )
    parser.add_argument('file_arg', metavar='<file>', type=str,
                        help='Data file in 8 bit iHEX (*.hex) or binary format (*.*, all other extensions except *.hex)')
    parser.add_argument('port_arg', metavar='<port>', type=str,
                        help='Serial port to use, eg COM1 in Windows or "/dev/ttyUSB0" in Linux')
    parser.add_argument('-mem', metavar='<type>', type=str, choices=['2716', '2732', '2764', '27128', '27256', '27512'],
                        default='27512', help='Emulated EPROM memory type, eg.: 2716..27512. Defaults to 27512')
    parser.add_argument('-spi', metavar='y/n', choices=['y','n'], default=False,
                        help='Enable/Disable emulator saves data to SPI EEPROM')
    parser.add_argument('-auto', metavar='y/n', choices=['y','n'], default=False,
                        help='Enable/Disable emulator automatically load last saved image and EPROM configuration on start.')
    parser.add_argument('-start', metavar='<start>', type=str, default="0x0000",
                        help='start address for loading binary files, defaults to 0x0000 if none provided')
    parser.add_argument('-map', metavar='y/n', choices=['y','n'], default=True,
                        help='Show a map of memory usage after loading files')
    args = parser.parse_args()
    mem = args.mem
    port = args.port_arg
    spi = args.spi
    auto = args.auto
    startaddrstr = args.start
    mapmem = args.map
    file = args.file_arg
    gui = False
else:
    values = get_pyqt_values()
    mem = values['mem']
    port = values['port']
    spi = "y" if values['spi'] else "n"
    auto = "y" if values['auto'] else "n"
    mapmem = "y" if values['map'] else "n"
    startaddrstr = values['startaddress']
    file = values['filename']
    gui = True

# Set EPROM memory parameters based on selected type
mem_type = ""
if mem == "2716":
    mem_type = ':ini16'
    max_size = 2048
elif mem == "2732":
    mem_type = ':ini32'
    max_size = 4096
elif mem == "2764":
    mem_type = ':ini64'
    max_size = 8192
elif mem == "27128":
    mem_type = ':ini128'
    max_size = 16384
elif mem == "27256":
    mem_type = ':ini256'
    max_size = 32768
elif mem == "27512":
    mem_type = ':ini512'
    max_size = 65536

try:
    ser = serial.Serial(port=port, baudrate=115200, timeout=0.5, writeTimeout=0)
except Exception as e:
    print("Failed to open port, verify port name")
    exit()

# Initialize 64k buffer
buff64k = [-1] * 65536

print("\nRunning EPROM EMU NG python script version", version)
print("")
print("File used: {}\n".format(os.path.basename(file)))

filename, file_extension = os.path.splitext(file)
if file_extension.lower() == ".hex":
    isbin = False
else:
    isbin = True

if isbin:  # process binary file
    startaddr = int(startaddrstr, 16)
    binfile = open(file, "rb").read()
    file_size = len(binfile)
    for x in range(0, file_size):
        buff64k[x + startaddr] = binfile[x]
    print("Done processing bin\n")
else:  # process hex file
    Bytes_Written = 0
    Errors = 0
    with open(file) as fp:
        while True:
            line = fp.readline()
            if len(line) > 0:
                if ParseIntelLine(line):
                    break
            else:
                break
        if Errors == 0:
            print("Done processing hex\n")
    fp.close()

# generate data map and display if requested
datamap = chunkData(buff64k)
if mapmem == "y":
    print('Each symbol represents 128Bytes ("*" with data, "-" with no data)\n')
    plotData(datamap)
else:
    print("Memory map disabled")
data = packetData(datamap)

print("\n\nUsing serial port {}, emulating: {} EPROM".format(port, mem))
time.sleep(2)  # wait for Arduino reset after opening the port

try:
    # Flush serial input and attempt synchronization
    for x in range(4):
        ser.write(("\r\n").encode())
    ser.flushInput()
    
    print("\n-- attempting to get sync --\n")
    ser.write((":dml\r\n").encode())
    response = ser.readline()
    
    if "HW: " in response.decode():
        print(response.decode())
    elif "...." in response.decode():
        print("Waiting for autoupload to finish... Note: you can disable autoupload by long pressing pushbutton")
        while "..." in ser.readline().decode():
            print(".", end=' ', flush=True)
        print("")
        time.sleep(2)
        ser.flushInput()
        ser.write((":dml\r\n").encode())
        response = ser.readline()
        if "HW: " in response.decode():
            print(response.decode())
        else:
            print("Failed to connect after autoload - exiting")
            exit()
    else:
        print("Failed to connect - exiting")
        exit()
    
    # Configure SPI EEPROM option
    if spi == "y":
        print('-- Setting "Save to SPI EEPROM" option to enable --')
        ser.write((":iniSPI1\r\n").encode())
        response = ser.readline()
        print(response.decode())
    else:
        print('-- Setting "Save to SPI EEPROM" option to disable --')
        ser.write((":iniSPI0\r\n").encode())
        response = ser.readline()
        print(response.decode())
    
    # Configure auto load option
    if auto == "y":
        print('-- Setting "Auto load from SPI EEPROM" option to enable --')
        ser.write((":iniAuto1\r\n").encode())
        response = ser.readline()
        print(response.decode())
    else:
        print('-- Setting "Auto load from SPI EEPROM" option to disable --')
        ser.write((":iniAuto0\r\n").encode())
        response = ser.readline()
        print(response.decode())
    
    print("-- processing file --\n")
    start_time = time.time()
    for StartAddr, FrameSize in data:
        ser.write((":SBN\r\n").encode())
        response = ser.readline()  # sync before sending data
        ser.write((str(StartAddr) + "\n").encode())
        ser.write((str(FrameSize) + "\n").encode())
        for x in range(StartAddr, StartAddr + FrameSize):
            if buff64k[x] < 0:
                buff64k[x] = 0xff
        ser.write(bytearray(buff64k[StartAddr:StartAddr + FrameSize]))
        response = ser.readline()
        print("0x%0.4X" % StartAddr, response.decode().strip(), end='\r', flush=True)
    taken = time.time() - start_time
    print("\n\nTime taken to transfer: {}s \n".format(str(round(taken, 2))))
    print("-- Setting EPROM Type --")
    ser.write((mem_type + "\r\n").encode())
    response = ser.readline()
    print(response.decode())
    
    print("-- Finished -- ")
    ser.write((":EMUON\r\n").encode())
    response = ser.readline()
    print(response.decode())
    
except Exception as e:
    print("Failed to send", e)
    
ser.close()
if gui:
    input("Press Enter to continue...")
sys.exit(0)
