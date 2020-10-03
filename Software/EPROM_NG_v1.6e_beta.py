# kris@mygeekyhobby.com
#
# ver 1.0 - may 2020	- initial release
# ver 1.2 - june 2020 	- added save to SPI EEPROM functionality
# 						- fixed the args parsing
#						- added auto load wait option
#						- added save SPI option
# ver 1.3 - july 2020	- removed some of the debug stuff
#						- removed address scrolling when loading
# ver 1.6g - Aug 2020	- Lunches GUI when no parameters specified (code contributed by John Gerrard)... requires PySimpleGUI module "pip install PySimpleGUI"
#						- small fixes to the output
# ver 1.6e              - fixed bug with SPI==y 

version = "1.6e"

import argparse
import serial,os
import time
import PySimpleGUI as sg
import sys

sg.theme('DarkAmber')   # Add a touch of color
form = sg.FlexForm('Eprom emulator Uploader V3')
from serial.tools.list_ports import comports
com = [p.device for p in comports()]

layout = [  [sg.Text('Eprom Type'), sg.InputCombo(('2716', '2732','2764', '27128','27256', '27512'), size=(10, 6))],
			[sg.Text('COM port   '), sg.InputCombo(com,size=(10, 1))],
			[sg.Checkbox('Save to SPI', default=False)],
			[sg.Checkbox('Auto Start', default=False)],
			[sg.Text('Choose A File', size=(35, 1))],
			[sg.In() ,sg.FileBrowse(file_types=(("Hex files", "*.hex"),))],
			[sg.Submit(), sg.Cancel()],
			]


if len(sys.argv) >= 3:
	parser = argparse.ArgumentParser(description='This script sends data to EPROM Emulator over serial port, requires python 3.8 and pyserial "pip install pyserial"')
	parser.add_argument('file_arg', metavar='<file>', type=str, help='Data file in 8 bit InteHEX format')
	parser.add_argument('port_arg', metavar='<port>', type=str, help='Serial port to use, eg COM1 in Windows or "/dev/ttyUSB0" in Linux')
	parser.add_argument('-mem', metavar='<type>', type=str, choices=['2716', '2732','2764', '27128', '27256', '27512'], default='27512', help='Emulated EPROM memory type, eg.: 2716..27512. Defaults to 27512')
	parser.add_argument('-spi', metavar='y/n', choices=['y','n'], default=False, help='Enable/Disable emulator saves data to SPI EEPROM. This only works on "full images" with data in blocks of 128 bytes, starting from 0000h')
	parser.add_argument('-auto', metavar='y/n', choices=['y','n'], default=False, help='Enable/Disable emulator automatically load last saved image and EPROM configuraiton on start.')
	args = parser.parse_args()
	mem = args.mem
	port = args.port_arg
	spi = args.spi
	auto = args.auto
	file = args.file_arg
	gui = False
else:
	window = sg.Window('EPROM EMU NG GUI Uploader '+version, layout)
	gui = True
# Create the Window
# Event Loop to process "events" and get the "values" of the inputs
	while True:
		event, values = window.read()
		mem = values[0]
		port = values[1]
		spi = values[2]
		auto = values[3]
        
		if values[2]: spi="y"
		else: spi= "n"
		if values[3]: auto="y"
		else: auto = "n"

		file = values[4]
		if event == sg.WIN_CLOSED or event == 'Cancel': # if user closes window or clicks cancel
			exit()
			break
		if event == 'Submit': # if user closes window or clicks cancel
			break

if gui:
	window.close()

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
        ser = serial.Serial(port=port,baudrate=115200,timeout=0.1,writeTimeout=0)
except:
        print("Failed to open port, verify port name")
        exit()

print ("\nRunning EPROM EMU NG python script version",version)
print("\n")
print("Using serial port {}, emulating: {} EPROM".format(port,mem))

time.sleep(2)	# on nano, opening the port will trigger reset of the arduino, so need to wait

try:
	for x in range(4):
		data_tx = ("\r\n").encode()
		ser.write(data_tx)
	
	ser.flushInput() # ignore anything waiting in the input buffer.
	
	print("\n-- attempting to get sync --\n")
	
	data_tx = (":dml\r\n").encode()
	ser.write(data_tx)

	response = ser.readline()

	if "HW: " in response.decode():	# Emulator will respond with version number like "HW: v1.0"
		print(response.decode())
	elif "...." in response.decode():
		print("Waiting for autoupload to finish... Note: you can disable autoupload by long pressing pushbutton")
		while "..." in ser.readline().decode():
			print(".", end=' ', flush=True)	
		
		print("")
		time.sleep(2)
		ser.flushInput() # ignore anything waiting in the input buffer.

		data_tx = (":dml\r\n").encode()
		ser.write(data_tx)
		response = ser.readline()
		if "HW: " in response.decode():
			print(response.decode())
		else:
			print("Failed to connect after autoload - exiting")
			exit()
	else:
		print("Failed to connect - exiting")
		exit()

	#
	# when sending entire "image" of and EPROM, for example 32k of data to emulate 27256 EPROM, you can ask the EMULATOR to save the data to SPI EEPROM
	# this will only work if you send full blocks of 128 bytes, in consecutive range starting from address 0000h up to the size of emulated EPROM
	# the emulator can reload this data when the button is pressed, can take up to 1min for 27512 - LED will quickly flash during loading.
	#

	if spi == "y":
		print('-- Setting "Save to SPI EEPROM" option to enable --')
		data_tx = (":iniSPI1\r\n").encode()
		ser.write(data_tx)
		response = ser.readline()
		print(response.decode())
	if spi == "n":
		print('-- Setting "Save to SPI EEPROM" option to disable --')
		data_tx = (":iniSPI0\r\n").encode()
		ser.write(data_tx)
		response = ser.readline()
		print(response.decode())

	#
	# Set the Auto load from SPI EEPROM option
	#
	if auto == "y":
		print('-- Setting "Auto load from SPI EEPROM" option to enable --')
		data_tx = (":iniAuto1\r\n").encode()
		ser.write(data_tx)
		response = ser.readline()
		print(response.decode())
	if auto == "n":
		print('-- Setting "Auto load from SPI EEPROM" option to disable --')
		data_tx = (":iniAuto0\r\n").encode()
		ser.write(data_tx)
		response = ser.readline()
		print(response.decode())

	print("-- prcessing file --")

	start = time.time()
	bytesdata = 0

	with open(file) as fp:
		while True:
			line = fp.readline()
			ser.write(line.encode())
			response = ser.readline()
			print(response[:-2].decode("utf-8"), end='\r', flush=True)
			if line[7:9] == "00": # count only data records
				bytesdata += len(line[9:-3])//2
			if not line:
				break
	
	print("\n")
    
	taken = time.time() - start

	print("Sent {} bytes in {} seconds\n".format(str(bytesdata),round(taken,2)))

	if bytesdata > max_size:
		print("Warninig!!! Sent {} bytes, but the selected {} EPROM can only hold {} bytes...\n".format(str(bytesdata),str(mem),str(max_size)))

	print("-- Setting EPROM Type --")

	data_tx = (mem_type + "\r\n").encode()
	ser.write(data_tx)
	response = ser.readline()
	print(response.decode())

	print("-- Finished -- ")
	
	#while(True):
	#	if ser.in_waiting > 0:
	#		print(ser.readline())

except Exception as e:
	print("Failed to send",e)
	
ser.close()
if gui:
	input("Press Enter to continue...")
exit()