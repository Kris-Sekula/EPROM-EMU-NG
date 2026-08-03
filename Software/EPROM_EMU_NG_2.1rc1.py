#!/usr/bin/env python3
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
# ver 2.0rc9 - Apr 2021	    - lowered the size of transfer chunk from 2048 to 1024 for transfers with "spi -n" as 2048 was causing issues on macOS
# ver 2.0rc10- Jan 2024     - allow refreshing serial ports, group gui elements, add input validation, use FreeSimpleGui
#                           - use transfer chunks of 2048 on non-macOS and 1024 on macOS.
# ver 2.1rc1 - Aug 2026     - added a portable prompt_toolkit terminal application for Windows and macOS
#                           - terminal mode includes a tab-navigable form, COM/EPROM selectors, an ASCII file browser,
#                             classic blue installer styling, keyboard shortcuts, and GUI-equivalent upload options
#                           - terminal UI is now the default with no arguments; use --gui for the graphical interface

version = "2.1rc1"

import argparse
import serial,os
import time
import FreeSimpleGUI as sg
import sys
import platform
from serial.tools.list_ports import comports

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
		if platform.system() == "Darwin":
			buffer_size = 1024
		else:
			buffer_size = 2048

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

def get_com_ports():
	return [p.device for p in comports()]

# -----------------------------------------------------------------------

def clear_screen():
    if os.name == 'nt':
        os.system('cls')
    else:
        os.system('clear')


def terminal_interface():
    try:
        from prompt_toolkit.application import Application, in_terminal
        from prompt_toolkit.completion import PathCompleter, WordCompleter
        from prompt_toolkit.key_binding import KeyBindings
        from prompt_toolkit.layout import Layout, HSplit, VSplit
        from prompt_toolkit.layout.dimension import D
        from prompt_toolkit.shortcuts import radiolist_dialog
        from prompt_toolkit.styles import Style
        from prompt_toolkit.widgets import Button, Checkbox, Dialog, Label, RadioList, TextArea, Frame
    except ImportError:
        print("Terminal mode requires prompt_toolkit. Install it with:\n    python -m pip install prompt_toolkit")
        sys.exit(1)

    clear_screen()
    # Classic dialog/newt installer palette. prompt_toolkit maps these colors
    # to both the Windows console API and ANSI terminals on macOS/Linux.
    tui_style = Style.from_dict({
        '': 'bg:#0000aa #ffffff',
        'label': 'bg:#0000aa #ffffff',
        'frame.border': 'bg:#0000aa #ffffff',
        'frame.label': 'bg:#0000aa #ffff00 bold',
        'text-area': 'bg:#000088 #ffffff',
        'button': 'bg:#c0c0c0 #000000',
        'button.focused': 'bg:#aa0000 #ffffff bold',
        'checkbox': 'bg:#0000aa #ffffff',
        'checkbox-selected': 'bg:#00aaaa #000000 bold',
        'checkbox-checked': 'bg:#0000aa #ffff00 bold',
        'radio-list': 'bg:#0000aa #ffffff',
        'radio-selected': 'bg:#00aaaa #000000 bold',
        'dialog': 'bg:#0000aa #ffffff',
        'dialog.body': 'bg:#0000aa #ffffff',
        'dialog frame.label': 'bg:#0000aa #ffff00 bold',
        'dialog.body label': 'bg:#0000aa #ffffff',
        'dialog shadow': 'bg:#000044',
    })
    ports = get_com_ports()
    ports_display = ', '.join(ports) if ports else 'None detected'
    eprom_types = ['2716', '2732', '2764', '2864', '27128', '27256', '28256', '27512']

    # Flexible dimensions prevent a field from forcing its Frame wider than the
    # console. Completion menus provide terminal-friendly dropdown equivalents.
    file_field = TextArea(
        height=1, multiline=False, width=D(min=12, preferred=38),
        completer=PathCompleter(expanduser=True), complete_while_typing=False)
    port_field = TextArea(
        height=1, multiline=False, width=D(min=8, preferred=20),
        text=ports[0] if ports else '',
        completer=WordCompleter(ports, ignore_case=True), complete_while_typing=True)
    mem_field = TextArea(
        height=1, multiline=False, width=D.exact(8), text='27512',
        completer=WordCompleter(eprom_types), complete_while_typing=True)
    start_field = TextArea(height=1, multiline=False, width=D.exact(8), text='0x0000')
    spi_check = Checkbox(text='Save to SPI EEPROM', checked=False)
    auto_check = Checkbox(text='Auto start on power-up', checked=False)
    map_check = Checkbox(text='Show data map', checked=True)
    # prompt_toolkit 3.0.53 initializes Checkbox's inherited scrollbar even
    # though Checkbox declares that it should be hidden. Without this explicit
    # override, a pointless '^' scroll arrow appears after every one-row item.
    for checkbox in (spi_check, auto_check, map_check):
        checkbox.show_scrollbar = False
    status_label = Label('Use Tab to move around fields. Enter to press buttons.')
    ports_label = Label(ports_display, dont_extend_height=True)

    result = {}

    def refresh_ports():
        nonlocal ports, ports_display
        ports = get_com_ports()
        ports_display = ', '.join(ports) if ports else 'None detected'
        ports_label.text = ports_display
        port_field.buffer.completer = WordCompleter(ports, ignore_case=True)
        if ports:
            port_field.text = ports[0]
        status_label.text = 'COM ports refreshed.'

    def get_port_value():
        return port_field.text.strip()

    def browse_file():
        async def show_file_browser():
            from pathlib import Path
            import string

            supported_extensions = {'.hex', '.bin', '.rom', '.img'}
            entered = file_field.text.strip()
            entered_path = Path(entered).expanduser() if entered else None
            if entered_path and entered_path.is_file():
                current_dir = entered_path.parent
            elif entered_path and entered_path.is_dir():
                current_dir = entered_path
            else:
                current_dir = Path.cwd()

            async with in_terminal():
                while True:
                    values = []

                    # Make every mounted Windows drive directly reachable.
                    if os.name == 'nt':
                        for letter in string.ascii_uppercase:
                            drive = Path(letter + ':\\')
                            if drive.is_dir():
                                values.append((('dir', str(drive)), '[DRIVE] ' + str(drive)))

                    if current_dir.parent != current_dir:
                        values.append((('dir', str(current_dir.parent)), '[..] Parent directory'))

                    try:
                        entries = list(os.scandir(current_dir))
                        directories = sorted(
                            (entry for entry in entries if entry.is_dir()),
                            key=lambda entry: entry.name.casefold())
                        files = sorted(
                            (entry for entry in entries
                             if entry.is_file() and
                             Path(entry.name).suffix.lower() in supported_extensions),
                            key=lambda entry: entry.name.casefold())
                    except OSError as exc:
                        status_label.text = 'Cannot open ' + str(current_dir) + ': ' + str(exc)
                        current_dir = current_dir.parent
                        continue

                    values.extend(
                        (('dir', entry.path), '[DIR]   ' + entry.name)
                        for entry in directories)
                    values.extend(
                        (('file', entry.path), '        ' + entry.name)
                        for entry in files)

                    browser_list = RadioList(values=values)
                    browser_keys = KeyBindings()

                    def open_selected():
                        browser_app.exit(result=browser_list.current_value)

                    def cancel_browser():
                        browser_app.exit(result=None)

                    # RadioList normally uses Enter only to mark the current
                    # radio item. In a file browser, Enter conventionally
                    # opens the highlighted directory or chooses the file.
                    @browser_list.control.key_bindings.add('enter')
                    def _(event):
                        open_selected()

                    # DOS/Windows-style accelerators. Alt sends an Escape
                    # prefix in terminal protocols, hence the two-key forms.
                    @browser_keys.add('c-o')
                    @browser_keys.add('escape', 'o')
                    def _(event):
                        open_selected()

                    @browser_keys.add('escape')
                    @browser_keys.add('escape', 'c')
                    def _(event):
                        cancel_browser()

                    browser_dialog = Dialog(
                        title='EPROM File Browser',
                        body=HSplit([
                            Label(str(current_dir)),
                            Label('Folders first; supported image files only.'),
                            browser_list,
                        ]),
                        buttons=[
                            Button('Open/Select (Alt-O)', handler=open_selected),
                            Button('Cancel (Esc)', handler=cancel_browser),
                        ],
                        width=D(preferred=76),
                        modal=True)
                    browser_app = Application(
                        layout=Layout(browser_dialog, focused_element=browser_list),
                        key_bindings=browser_keys,
                        style=tui_style,
                        full_screen=True)
                    selected = await browser_app.run_async()

                    if selected is None:
                        break

                    entry_type, selected_path = selected
                    if entry_type == 'dir':
                        current_dir = Path(selected_path)
                    else:
                        selected_file = Path(selected_path)
                        file_field.text = str(selected_file)
                        status_label.text = 'File selected: ' + selected_file.name
                        break

            app.invalidate()

        app.create_background_task(show_file_browser())

    def select_eprom():
        async def show_eprom_dialog():
            current = mem_field.text.strip()
            values = [(eprom, eprom) for eprom in eprom_types]
            async with in_terminal():
                selected = await radiolist_dialog(
                    title='Select EPROM Type',
                    text='Use the arrow keys and press Enter:',
                    values=values,
                    default=current if current in eprom_types else '27512',
                    style=tui_style
                ).run_async()
            if selected:
                mem_field.text = selected
                status_label.text = 'EPROM type selected: ' + selected
            app.invalidate()

        app.create_background_task(show_eprom_dialog())

    def select_port():
        if not ports:
            status_label.text = 'No COM ports detected. Press Refresh or type a port manually.'
            app.invalidate()
            return

        async def show_port_dialog():
            current = port_field.text.strip()
            values = [(port_name, port_name) for port_name in ports]
            async with in_terminal():
                selected = await radiolist_dialog(
                    title='Select COM Port',
                    text='Use the arrow keys and press Enter:',
                    values=values,
                    default=current if current in ports else ports[0],
                    style=tui_style
                ).run_async()
            if selected:
                port_field.text = selected
                status_label.text = 'COM port selected: ' + selected
            app.invalidate()

        app.create_background_task(show_port_dialog())

    def submit_handler():
        port_value = get_port_value()
        if not file_field.text.strip():
            status_label.text = 'Error: File path is required.'
            return
        if not port_value:
            status_label.text = 'Error: Serial port is required.'
            return
        result['file'] = file_field.text.strip()
        result['port'] = port_value
        result['mem'] = mem_field.text.strip() or '27512'
        result['spi'] = 'y' if spi_check.checked else 'n'
        result['auto'] = 'y' if auto_check.checked else 'n'
        result['startaddrstr'] = start_field.text.strip() or '0x0000'
        result['mapmem'] = 'y' if map_check.checked else 'n'
        app.exit()

    def cancel_handler():
        app.exit()

    browse_button = Button('Browse', handler=browse_file)
    refresh_button = Button('Refresh', handler=refresh_ports)
    port_button = Button('Select', handler=select_port)
    eprom_button = Button('Select', handler=select_eprom)
    button_row = VSplit([
        Button('Submit', handler=submit_handler),
        Button('Cancel', handler=cancel_handler)
    ], padding=3)

    # Keep each Frame compact. The earlier vertical stacks consumed more than
    # 30 rows even though the terminal was wide enough, which made
    # prompt_toolkit replace the form with its "Window too small" message.
    file_box = Frame(
        VSplit([Label('File:', width=7), file_field, browse_button], padding=1),
        title='File Options')

    connection_box = Frame(
        HSplit([
            VSplit([Label('Detected:', width=10), ports_label], padding=1),
            VSplit([
                Label('Port:', width=10), port_field, port_button, refresh_button
            ], padding=1),
        ]),
        title='Connection Settings')

    eprom_box = Frame(
        VSplit([
            Label('EPROM:', width=8), mem_field, eprom_button,
            Label('Start:', width=7), start_field,
        ], padding=1),
        title='EPROM Settings')

    options_box = Frame(
        VSplit([spi_check, auto_check, map_check], padding=2),
        title='Options')

    # One stable 80-column arrangement is more predictable across cmd.exe,
    # Windows Terminal, and macOS Terminal than switching layouts at startup.
    content_container = HSplit(
        [file_box, connection_box, eprom_box, options_box], padding=1)

    root_container = HSplit([
        Label('EPROM Emulator Uploader', style='bold', dont_extend_height=True),
        content_container,
        status_label,
        button_row
    ], padding=1)

    kb = KeyBindings()

    @kb.add('tab')
    def _(event):
        event.app.layout.focus_next()

    @kb.add('s-tab')
    def _(event):
        event.app.layout.focus_previous()

    @kb.add('c-c')
    def _(event):
        status_label.text = 'Ctrl-C is disabled here. Use Ctrl-Q or Cancel to quit.'

    @kb.add('c-q')
    def _(event):
        cancel_handler()

    app = Application(
        layout=Layout(root_container, focused_element=file_field),
        key_bindings=kb,
        style=tui_style,
        full_screen=True)
    app.run()

    if not result:
        sys.exit(0)

    return result['mem'], result['port'], result['spi'], result['auto'], result['startaddrstr'], result['mapmem'], result['file']

# -----------------------------------------------------------------------

layout = [
	[sg.Text('EPROM Emulator Uploader', font=("Helvetica", 16), justification='center')],
	[sg.Column( [
	[sg.Frame(
		"EPROM Settings",
		[[sg.Text('EPROM Type:', size=(40, 1)), sg.Combo(
			['2716', '2732', '2764', '2864', '27128', '27256', '28256', '27512'],
			sg.user_settings_get_entry('mem', ''), key='mem', size=(10, 6))],
		 [sg.Text('Start Address:', size=(40, 1)), sg.InputText(
			 sg.user_settings_get_entry('startaddress', "0x0000"),
			 key='startaddress', size=(10, 1))]]
	)],
	[sg.Frame(
		"Connection Settings",
		[[sg.Text('COM Port:', size=(19, 1)), sg.Combo(
			get_com_ports(), sg.user_settings_get_entry('port', ''), key='port', size=(15, 1)),
		  sg.Button("Refresh Ports", key='refresh_ports')],
		 [sg.Checkbox('Save to SPI', sg.user_settings_get_entry('spi', False), key='spi'),
		  sg.Checkbox('Auto Start', sg.user_settings_get_entry('auto', False), key='auto')]]
	)],
	[sg.Frame(
		"File Options",
		[[sg.Text('Choose File:', size=(11, 1)), sg.InputText(
			sg.user_settings_get_entry('filename', ''), key='filename', size=(30, 1)),
		  sg.FileBrowse(file_types=(("Supported Files", ".hex .bin .rom .img"), ("All Files", "*.*")))]]
	)],
	[sg.Checkbox('Show Data Map', sg.user_settings_get_entry('map', True), key='map')],
	[sg.Button('Submit'), sg.Button('Cancel')]
	], pad=(0,0), scrollable=False
	)]
]

parser = argparse.ArgumentParser(
	description='EPROM EMU NG uploader with terminal, graphical, and direct command-line modes; requires Python 3.8+ and pyserial')
parser.add_argument('--version', action='version', version='%(prog)s '+version)
parser.add_argument('--terminal', action='store_true', help='Run the interactive terminal UI')
parser.add_argument('--gui', action='store_true', help='Run the GUI instead of the terminal UI')
parser.add_argument('file_arg', metavar='<file>', type=str, nargs='?', help='Data file in 8 bit iHEX (*.hex) or binary format (*.*, all other extensions except *.hex)')
parser.add_argument('port_arg', metavar='<port>', type=str, nargs='?', help='Serial port to use, eg COM1 in Windows or "/dev/ttyUSB0" in Linux')
parser.add_argument('-mem', metavar='<type>', type=str, choices=['2716', '2732','2764', '2864','27128', '27256', '28256','27512'], default='27512', help='Emulated EPROM memory type, eg.: 2716..27512. Defaults to 27512')
parser.add_argument('-spi', metavar='y/n', choices=['y','n'], default='n', help='Enable/Disable emulator saves data to SPI EEPROM')
parser.add_argument('-auto', metavar='y/n', choices=['y','n'], default='n', help='Enable/Disable emulator automatically load last saved image and EPROM configuraiton on start.')
parser.add_argument('-start', metavar='<start>', type=str, default="0x0000", help='start address for loading binary files, defaults to 0x0000 if none provided')
parser.add_argument('-map', metavar='y/n', choices=['y','n'], default='y', help='Show a map of memory usage after loading files')
args = parser.parse_args()

if args.file_arg and args.port_arg:
	mem = args.mem
	port = args.port_arg
	spi = args.spi
	auto = args.auto
	startaddrstr = args.start
	mapmem = args.map
	file = args.file_arg
	gui = False
elif args.file_arg or args.port_arg:
	parser.error('both <file> and <port> are required for direct CLI mode')
elif args.gui:
	# Default window position
	default_location = (100,100)
	# Retrieve last saved window position or use the default
	last_position = sg.user_settings_get_entry('window_location', default_location)
	# Validate position
	if last_position is None or not isinstance(last_position, tuple) or len(last_position) != 2:
		last_position = default_location
	window = sg.Window('EPROM EMU NG Uploader '+version, layout, location=last_position)
	gui = True
else:
	mem, port, spi, auto, startaddrstr, mapmem, file = terminal_interface()
	gui = False

# Create the Window
# Event Loop to process "events" and get the "values" of the inputs
if gui:
	while True:
		event, values = window.read()

		if event in (sg.WINDOW_CLOSED, 'Cancel'):
			try:
				sg.user_settings_set_entry('window_location', window.current_location())
			except Exception:
				pass
			sys.exit(0)		

		if event == 'refresh_ports':
			window['port'].update(values=get_com_ports())

		if event == 'Submit':
			# Validate inputs
			if not values['filename']:
				sg.popup_error('Error: Please select a file to upload!')
				continue
			if not values['port']:
				sg.popup_error('Errror: Please select a COM port!')
				continue
			if not values['mem']:
				sg.popup_error('Errror: Please select an EPROM type!')
				continue

			# Save user preferences
			sg.user_settings_set_entry('filename', values['filename'])
			sg.user_settings_set_entry('startaddress', values['startaddress'])
			sg.user_settings_set_entry('mem', values['mem'])
			sg.user_settings_set_entry('spi', values['spi'])
			sg.user_settings_set_entry('auto', values['auto'])
			sg.user_settings_set_entry('port', values['port'])
			sg.user_settings_set_entry('map', values['map'])
			sg.user_settings_set_entry('window_location', window.current_location())

			if values['map']: mapmem="y"
			else: mapmem = "n"
			if values['spi']: spi="y"
			else: spi= "n"
			if values['auto']: auto="y"
			else: auto = "n"
			mem = values['mem']
			port = values['port']
			startaddrstr = values['startaddress']
			file = values['filename']
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
print ("EPROM EMU NG tool v{}\n".format(version))
print("Processing {}... ".format(os.path.basename(file)), end="")

filename, file_extension = os.path.splitext(file)

if file_extension.lower() == ".hex":
	isbin = False
else:
	isbin = True


if isbin:	# process bin

	startaddr = int(startaddrstr, 16)
	binfile = open(file,"rb").read()
	file_size = len(binfile)

	# Validate file size against maximum buffer size
	if startaddr + file_size > len(buff64k):
		print("Error: File size exceeds buffer capacity.")
		sys.exit(1)

	# load the binary file into the buffer at start location
	for x in range (0,file_size):
		buff64k[x+startaddr] = binfile[x]
	print("done (bin)")

else:	# process hex file
	Bytes_Written = 0
	Errors = 0

	with open(file) as fp:
		while True:
			line = fp.readline()
			if not line:
				break
			if ParseIntelLine(line):
				break
		if Errors == 0:
			print("done (hex)")
	fp.close()

# generate data map (list of 128Byte chunks that has data)
datamap = chunkData(buff64k)
# display map of data in buffer
if mapmem == "y":
	print('Each symbol represents 128Bytes "*" with data, "-" with no data\n')
	plotData(datamap)

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

print("Using serial port {}, emulating: {} EPROM".format(port,mem))

time.sleep(2)	# on nano, opening the port will trigger reset of the arduino, so need to wait

try:
	for x in range(8):
		response = ser.readline()
		#print(str(x),response.decode().strip(),str(x))
	
	ser.flushInput() # ignore anything waiting in the input buffer.
	
	print("<- attempting to get sync")

	data_tx = b":EMUOFF\r\n"
	ser.write(data_tx)
	response = ser.readline()

	if b"HW: " in response:	# Emulator will respond with version number like "HW: v1.0"
		print(response.decode(errors="ignore"), end="")
	elif b".." in response:
		print("Waiting for autoupload to finish...")
		while b".." in ser.readline():
			print(".", end=' ', flush=True)	
		
		time.sleep(2)
		ser.flushInput() # ignore anything waiting in the input buffer.

		data_tx = b":EMUOFF\r\n"
		ser.write(data_tx)
		response = ser.readline()
		if b"HW: " in response:
			print(response.decode(errors="ignore"))
		else:
			print("Failed to connect after autoload - exiting")
			print("Debug info:", response.decode(errors="ignore"))
			exit()
	else:
		# try to see if emulator running old FW is connected
		data_tx = b":dml\r\n"
		ser.write(data_tx)
		response = ser.readline()
		if b"HW: " in response:
			print(response.decode(errors="ignore"))
			print("-"*80)
			print("    !!! Looks like you are using an old version of firmware on your emulator !!!")
			print("        Please upgrade using Arduino platform and compatible .ino file")
			print("-"*80)
		else:
			print("Failed to connect - exiting")
		# give ppl chance to read the failed connection reason even in GUI mode
		exit()

	# Connected, now we can send settings

	#
	#  Enable saving to SPI EEPROM
	#
	if spi == "y":
		print('<- Setting "Save to SPI EEPROM" option to enable')
		data_tx = b":iniSPI1\r\n"
		ser.write(data_tx)
		response = ser.readline()
		print(response.decode(), end="")
	if spi == "n":
		print('<- Setting "Save to SPI EEPROM" option to disable')
		data_tx = b":iniSPI0\r\n"
		ser.write(data_tx)
		response = ser.readline()
		print(response.decode(), end="")

	#
	# Set the Auto load from SPI EEPROM option
	#
	if auto == "y":
		print('<- Setting "Auto load from SPI EEPROM" option to enable')
		data_tx = b":iniAuto1\r\n"
		ser.write(data_tx)
		response = ser.readline()
		print(response.decode(), end="")
	if auto == "n":
		print('<- Setting "Auto load from SPI EEPROM" option to disable')
		data_tx = b":iniAuto0\r\n"
		ser.write(data_tx)
		response = ser.readline()
		print(response.decode(), end="")

	print("<- processing file:")
	start = time.time()

	for StartAddr,FrameSize in data:
		#print("Sending {}".format(StartAddr))
		
		# request byte frame processing
		if spi == "y":
			data_tx = b":SBN\r\n"
		else:
			data_tx = b":DIR\r\n"
		
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
		ser.write(bytes(buff64k[StartAddr:StartAddr+FrameSize]))
		response = ser.readline()
		
		print("0x%0.4X"%StartAddr,response.decode().strip(), end='\r', flush=True)
		

	taken = time.time() - start
	print("\nTime taken to transfer: {}s".format(str(round(taken,2))))

	print("<- Setting EPROM Type")
	data_tx = (mem_type + "\r\n").encode()
	ser.write(data_tx)
	response = ser.readline()
	print(response.decode(), end="")

	print("<- Enable Emulation")
	# enable emulation
	data_tx = b":EMUON\r\n"
	ser.write(data_tx)
	response = ser.readline()
	print(response.decode())
	print("Success, Finished.")

except Exception as e:
	print("Failed to send",e)
	
ser.close()

sys.exit(0)
