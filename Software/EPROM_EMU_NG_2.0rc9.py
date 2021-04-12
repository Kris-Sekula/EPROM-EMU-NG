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
#
# ver 2.0rc1 - Oct 2020   	- save parameters in GUI
#							- .bin and .hex file support
#							- binary mode upload with packets of 128-512 bytes at a time
#							- Save to SPI works with any type of data range (any source file less than 128 bytes gets "padded" with 0xFF)
# ver 2.0rc2 - Oct 2020   	- support other extenstions like .rom .img etc... everything not .hex is treated as binary (thanks to Hubert for the feedback and contribution)
#                           - fixed bug where a 128Byte of "00h" was treated as "no data" (thanks to Hubert for the feedback and contribution)
# ver 2.0rc3 - Oct 2020     - code from rc1 that was missing in rc2 was added back, one typo fixed (thx again Hubert)   	
# ver 2.0rc7 - Apr 2021		- added support for 28C64 and 28C256 EEPROM memory (experimental)
# ver 2.0rc8 - Apr 2021	    - added direct to SRAM upload bypassing local buffer					
# ver 2.0rc9 - Apr 2021	    - lowered the size of transfer chunk from 2048 to 1024 for transfers with "spi -n" as 2048 was casuing issues on macOS


version = "2.0rc9"

import argparse
import serial,os
import time
import PySimpleGUI as sg
import sys

# All routines here

# Print int list in Hex format (for debugging only)
def PrintHex(intList):
	row = 0
	for val in intList:
		print("%0.2X"%val, end=' ', flush=True)
		row += 1
		if row == 64:
			print("")
			row=0
# -----------------------------------------------------------------------
def chunkData(bfr):
	"""
	Analyze the 64k buffer, find all 128Byte chunks that are in use (have data != to -1)
	Will return a list of 512 entries with "1" meaning block of 128Bytes had data chaned and "0" no data changed
	"""
	mapped=[]

	for i in range(0,len(bfr),128):
		chunk = bfr[i:i+128]			# split buffer into 128 Bytes chunks
		
		used = False

		for cell in chunk:				# if any byte changed, mark chunk for upload
			if cell >= 0:
				used = True
		
		if used:
			mapped.append(1)
		else:
			mapped.append(0)

	return mapped
# -----------------------------------------------------------------------
def plotData(mapped):
	"""
	Display map of data usage in the buffer, "*" data used and "_" not used
	"""
	col=0
	for idx in range(len(mapped)):
		if idx == 0:		# first address special case
			print("0x%0.4X"%idx, end=' ', flush=True)

		if mapped[idx] == 1:
			print("*", end=' ', flush=True)
		else:
			print("-", end=' ', flush=True)
		col+=1
		if col == 32:
			if (idx*128+128) < 65535:
				print("")
				print("0x%0.4X"%int(idx*128+128),end=' ', flush=True)
				col=0
	return


# -----------------------------------------------------------------------
def packetData(mapped):
	global spi

	data=[]
	"""
	create a list of tuples [(start_address,size),(start_address,size),...,(start_address,size)] ... size can be 128,256,384,512
	in other words find continues ranges of 128Byte data that has changed, group into packets of 128-512 bytes, create a list for later processing
	"""
	if spi == "y":
		buffer_size = 512
	else:
		buffer_size = 1024

	size=0
	start=0
	for idx in range(len(mapped)):
		if mapped[idx] == 1:                  # found used sector
			if size == 0:                     # if chunk empty (new chunk)
				start = idx * 128             # init start address of new chunk
			size += 128                       # one sector more to load
			# test for full chunk or partial filled chunk at end of buffer
			if (size >= buffer_size) or ((size > 0) and (idx >= (len(mapped) - 1))):
				data.append((start, size))    # save chunk
				size = 0                      # reinit size for next chunk
		else:                                 # found unused or cached sector
			if size > 0:                      # sectors in chunk?
				data.append((start, size))    # save chunk, if exists
			size = 0                          # reinit size for next chunk
	return data

# -----------------------------------------------------------------------
def Decode(len, sum, addr, data):

	"""
	Borrowed this from www.sbprojects.net, credits to San Bergmans

	Decode an Intel string.

	Returns a list of N bytes.
	Returns an empty list if an error has occurred.
	"""

	global Errors

	checksum = len + (addr & 0xFF) + (addr >> 8)
	index = 0
	bytes = []

	if len <= 0 or len > 32:
		# Illegal length
		print("Illegal line length found.")
		Errors += 1
		return bytes

	while len > 0:
		# Iterate over all data bytes
		decimal = int(data[index:index+2], 16)
		bytes.append(decimal)
		checksum = checksum + decimal
		index += 2
		len -= 1

	checksum = (checksum + int(sum, 16)) & 0xFF

	if checksum != 0:
		# Checksum didn't add up
		print("Checksum error.")
		bytes = []
		Errors += 1
		return bytes

	return bytes
# -----------------------------------------------------------------------
def ParseIntelLine(line):

	"""
	Borrowed this from www.sbprojects.net, credits to San Bergmans

	Parse an IntelHex line.
	Record types 02, 03, 04 and 05 are ignored.
	Maximum payload length is 32 bytes.
	Everything after the checksum is ignored.

	Returns True if EOF record found, or when an error is found.
	"""

	global Errors

	# Remove EOL character and leading and trailing white space
	line = line.strip()

	if len(line) == 0:
		# Ignore empty lines
		return False

	# Pad the line with plenty of spaces to make parsing easier
	# We don't have to worry about the line being too short
	line = line + ' '*74

	if line[0] in ('#', ';'):
		# Ignore lines starting with a comment symbol
		return False

	if line[0] != ":":
		# Line must begin with a colon
		print("Illegal start of formatted line.")
		Errors += 1
		return True

	if line[7:9] in ('02', '03', '04', '05'):
		# These record types are simply ignored
		return False

	if line[0:11] == ":00000001FF":
		# End of file marker found
		return True

	datalength = int(line[1:3], 16)
	checksum_index = (datalength + 4) * 2 + 1
	checksum = line[checksum_index:checksum_index + 2]
	address = int(line[3:7], 16)
	data = line[9:checksum_index]

	# Decode the dataline
	bytes = Decode(datalength, checksum, address, data)
	if len(bytes) == 0:
		# Decoding the line resulted in an error
		return True

	#print(address, bytes)
	for x in range (0,len(bytes)):
		buff64k[x+address] = bytes[x]
# -----------------------------------------------------------------------

sg.theme('DarkAmber')   # Add a touch of color



form = sg.FlexForm('Eprom emulator Uploader 2.0')
from serial.tools.list_ports import comports
com = [p.device for p in comports()]

layout = [  [sg.Text('Eprom Type'), sg.InputCombo(('2716', '2732','2764', '2864', '27128','27256', '28256','27512'),sg.user_settings_get_entry('mem',''), key='mem', size=(10, 6))],		
			[sg.Text('COM port   '), sg.InputCombo(com,sg.user_settings_get_entry('port',''),key='port',size=(10, 1))],														
			[sg.Checkbox('Save to SPI',sg.user_settings_get_entry('spi',False),key='spi'),sg.Checkbox('Show Data Map',sg.user_settings_get_entry('map',True),key='map')],																	
			[sg.Checkbox('Auto Start',sg.user_settings_get_entry('auto',False),key='auto')],																		
			[sg.Text('Start Address (for binary files)'),sg.Input(sg.user_settings_get_entry('startaddress',"0x0000"), key='startaddress', size=(10,1))],									# 4	
			[sg.Text('Choose A File', size=(35, 1))],																		
			[sg.In(sg.user_settings_get_entry('filename', ''),key='filename'),sg.FileBrowse(file_types=(("Supported", ".hex .bin .rom .img"),("All files","*.*")))],													# 6
			[sg.Submit(), sg.Cancel()],
			]


if len(sys.argv) >= 2:
	parser = argparse.ArgumentParser(description='This script sends data to EPROM Emulator over serial port, requires python 3.8, pyserial and pysimplegui (use "pip3 install"')
	parser.add_argument('file_arg', metavar='<file>', type=str, help='Data file in 8 bit iHEX (*.hex) or binary format (*.*, all other extensions except *.hex)')
	parser.add_argument('port_arg', metavar='<port>', type=str, help='Serial port to use, eg COM1 in Windows or "/dev/ttyUSB0" in Linux')
	parser.add_argument('-mem', metavar='<type>', type=str, choices=['2716', '2732','2764', '2864','27128', '27256', '28256','27512'], default='27512', help='Emulated EPROM memory type, eg.: 2716..27512. Defaults to 27512')
	parser.add_argument('-spi', metavar='y/n', choices=['y','n'], default=False, help='Enable/Disable emulator saves data to SPI EEPROM')
	parser.add_argument('-auto', metavar='y/n', choices=['y','n'], default=False, help='Enable/Disable emulator automatically load last saved image and EPROM configuraiton on start.')
	parser.add_argument('-start', metavar='<start>', type=str, default="0x0000", help='start address for loading binary files, defaults to 0x0000 if none provided')
	parser.add_argument('-map', metavar='y/n', choices=['y','n'], default=True, help='Show a map of memory usage after loading files')
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
	window = sg.Window('EPROM EMU NG GUI Uploader '+version, layout)
	gui = True
# Create the Window
# Event Loop to process "events" and get the "values" of the inputs
	while True:
		event, values = window.read()
		mem = values['mem']
		port = values['port']
		spi = values['spi']
		auto = values['auto']
		startaddrstr = values['startaddress']
		mapmem = values['map']

		if values['spi']: spi="y"
		else: spi= "n"
        
		if values['auto']: auto="y"
		else: auto = "n"
        
		if values['map']: mapmem="y"
		else: mapmem = "n"
        
		file = values['filename']
		if event == sg.WIN_CLOSED or event == 'Cancel': # if user closes window or clicks cancel
			exit()
			break
		if event == 'Submit': # if user closes window or clicks cancel
			sg.user_settings_set_entry('filename', values['filename'])
			sg.user_settings_set_entry('startaddress', values['startaddress'])
			sg.user_settings_set_entry('mem', values['mem'])
			sg.user_settings_set_entry('spi', values['spi'])
			sg.user_settings_set_entry('auto', values['auto'])
			sg.user_settings_set_entry('port', values['port'])
			sg.user_settings_set_entry('map', values['map'])
			break

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
elif mem == "2864":
	mem_type = ':iniE64'	# EEPROM 28C256 support, new as of 2.0rc7
	max_size = 8192
elif mem == "27128":
	mem_type = ':ini128'
	max_size = 16384
elif mem == "27256":
	mem_type = ':ini256'
	max_size = 32768
elif mem == "28256":
	mem_type = ':iniE256'	# EEPROM 28C256 support, new as of 2.0rc7
	max_size = 32768
elif mem == "27512":
	mem_type = ':ini512'
	max_size = 65536

try:
        ser = serial.Serial(port=port,baudrate=115200,timeout=0.5,writeTimeout=0)
		#ser = serial.Serial(port=port,baudrate=1000000,timeout=0.5,writeTimeout=0)
except:
        print("Failed to open port, verify port name")
        exit()

# ------------------------------------------------------------------------
buff64k = [-1]*65536
# -------------------------------------------------------------------------
print ("\nRunning EPROM EMU NG python script version",version)
print("")
print("File used: {}\n".format(os.path.basename(file)))

filename, file_extension = os.path.splitext(file)

if file_extension.lower() == ".hex":
	isbin = False
else:
	isbin = True


if isbin:	# process bin

	startaddr = int(startaddrstr, 16)
	binfile = open(file,"rb").read()
	file_size = len(binfile)

	# load the binary file into the buffer at start location
	for x in range (0,file_size):
		buff64k[x+startaddr] = binfile[x]
	print("Done processing bin\n")

else:	# process hex file
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

# generate data map (list of 128Byte chunks that has data)
datamap = chunkData(buff64k)
# display map of data in buffer
if mapmem == "y":
	print('Each symbol represents 128Bytes "*" with data, "-" with no data\n')
	plotData(datamap)
else:
	print("Memory map disabled")

# at this point we have raw data in the buffer, 

# To support 28C256 we are using a software trick, pinout of 28C256 matches 27C256 with the exception of line A14, that ends up connected to A15, so we re-shuffle the data to match.
if mem == "28256":
	temp_buff64k = [-1]*65536
	for i in range (0,16384):
		temp_buff64k[i] = buff64k[i]	# copy data 0000h-3FFFh to the new buffer (as we will fix A14 to logic 0 inside the emulator)
	for i in range (16384, 32768):
		temp_buff64k[i+16384] = buff64k[i]	# shift data 4000h-7FFFh to 8000h-BFFFh (as pin 1 of EPROM 27512(A15) is now connected to A14 of EEPROM 28C256)
	buff64k = temp_buff64k
	# now our new buffer has data shifted for EEPROM, we don't want to show it anymore on the map but we need to process it anyways
	# generate data map (list of 128Byte chunks that has data)
	datamap = chunkData(buff64k)

# process data map into a list of 128Byte chunks that can be sent in 1,2,3 or 4 chunks of 128Bytes to the emulator

data = packetData(datamap)

# ----------------------------------------------------------------------------------

print("\n\nUsing serial port {}, emulating: {} EPROM".format(port,mem))

time.sleep(2)	# on nano, opening the port will trigger reset of the arduino, so need to wait

try:
	for x in range(8):
		response = ser.readline()
		#print(str(x),response.decode().strip(),str(x))
	
	ser.flushInput() # ignore anything waiting in the input buffer.
	
	print("\n<- attempting to get sync")

	data_tx = (":EMUOFF\r\n").encode()
	ser.write(data_tx)
	response = ser.readline()

	if "HW: " in response.decode():	# Emulator will respond with version number like "HW: v1.0"
		print(response.decode())
	elif ".." in response.decode():
		print("Waiting for autoupload to finish...")
		while ".." in ser.readline().decode():
			print(".", end=' ', flush=True)	
		
		print("")
		time.sleep(2)
		ser.flushInput() # ignore anything waiting in the input buffer.

		data_tx = (":EMUOFF\r\n").encode()
		ser.write(data_tx)
		response = ser.readline()
		if "HW: " in response.decode():
			print(response.decode())
		else:
			print("Failed to connect after autoload - exiting")
			print("Debug info:", response.decode())
			exit()
	else:
		# try to see if emulator running old FW is connected
		data_tx = (":dml\r\n").encode()
		ser.write(data_tx)
		response = ser.readline()
		print("\n")
		if "HW: " in response.decode():
			print(response.decode())
			print("-"*80)
			print("    !!! Looks like you are using an old version of firmware on your emulator !!!")
			print("        Please upgrade using Arduino platform and compatible .ino file")
			print("-"*80)
		else:
			print("Failed to connect - exiting")
		# give ppl chance to read the failed connection reason even in GUI mode
		input("Press Enter to continue...")	
		exit()

	# Connected, now we can send settings

	#
	#  Enable saving to SPI EEPROM
	#
	if spi == "y":
		print('<- Setting "Save to SPI EEPROM" option to enable')
		data_tx = (":iniSPI1\r\n").encode()
		ser.write(data_tx)
		response = ser.readline()
		print(response.decode())
	if spi == "n":
		print('<- Setting "Save to SPI EEPROM" option to disable')
		data_tx = (":iniSPI0\r\n").encode()
		ser.write(data_tx)
		response = ser.readline()
		print(response.decode())

	#
	# Set the Auto load from SPI EEPROM option
	#
	if auto == "y":
		print('<- Setting "Auto load from SPI EEPROM" option to enable')
		data_tx = (":iniAuto1\r\n").encode()
		ser.write(data_tx)
		response = ser.readline()
		print(response.decode())
	if auto == "n":
		print('<- Setting "Auto load from SPI EEPROM" option to disable')
		data_tx = (":iniAuto0\r\n").encode()
		ser.write(data_tx)
		response = ser.readline()
		print(response.decode())

	print("<- processing file\n")
	start = time.time()

	for StartAddr,FrameSize in data:
		#print("Sending {}".format(StartAddr))
		
		# request byte frame processing
		if spi == "y":
			data_tx = (":SBN\r\n").encode()
		else:
			data_tx = (":DIR\r\n").encode()
		
		ser.write(data_tx)
		# Wait for ACK
		response = ser.readline()	# this is a form of sync before the array gets sent

		# Send address and no of bytes incomming

		ser.write((str(StartAddr)+"\n").encode())	
		ser.write((str(FrameSize)+"\n").encode())


		# set un-used bytes "-1" to "0xffh"
		for x in range (StartAddr,StartAddr+FrameSize):
			if buff64k[x] < 0:
				buff64k[x] = 0xff
		
		# send to emulator
		ser.write(buff64k[StartAddr:StartAddr+FrameSize])
		response = ser.readline()
		
		print("0x%0.4X"%StartAddr,response.decode().strip(), end='\r', flush=True)
		

	taken = time.time() - start
	print("\n\nTime taken to transfer: {}s \n".format(str(round(taken,2))))
	print("<- Setting EPROM Type")

	data_tx = (mem_type + "\r\n").encode()
	ser.write(data_tx)
	response = ser.readline()
	print(response.decode())

	print("<- Enable Emulation")
	
	# enable emulation
	data_tx = (":EMUON\r\n").encode()
	ser.write(data_tx)

	response = ser.readline()
	print(response.decode())

	print("\nSuccess, Finished.")

except Exception as e:
	print("Failed to send",e)
	input("Press Enter to continue...")
	
ser.close()

if gui:
	input("Press Enter to continue...")
sys.exit(0)