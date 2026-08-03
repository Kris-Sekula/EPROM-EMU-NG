# EPROM EMU NG software

Python software for loading binary and Intel HEX images into the EPROM EMU NG hardware.

## Recommended versions

- Firmware 2.0rc12 (`EPROM_EMU_NG_FW_2.0rc12.ino`)
- Software 2.1rc1 (`EPROM_EMU_NG_2.1rc1.py`)

Software 2.1rc1 adds a portable, full-screen terminal interface while retaining the graphical interface and direct command-line mode.

## Requirements

- Windows or macOS (Linux terminals should also work)
- Python 3.8 or newer
- A serial driver for the emulator hardware
- `pyserial` for serial communication
- `prompt_toolkit` for the terminal interface
- `FreeSimpleGUI` for the optional graphical interface

Install the Python packages with:

```console
python -m pip install pyserial prompt_toolkit FreeSimpleGUI
```

On macOS, use `python3` and `pip3` if `python` is not available:

```console
python3 -m pip install pyserial prompt_toolkit FreeSimpleGUI
```

### Serial drivers

A genuine Arduino Nano normally receives its driver with the Arduino IDE. Many Nano-compatible boards use a CH340 USB serial chip and may require a separate driver. See the [SparkFun CH340 driver guide](https://learn.sparkfun.com/tutorials/how-to-install-ch340-drivers/all).

When installing Python on Windows, enable **Add Python to PATH**.

## Terminal interface (default)

Launch the portable terminal application with no arguments:

```console
python EPROM_EMU_NG_2.1rc1.py
```

It can also be requested explicitly:

```console
python EPROM_EMU_NG_2.1rc1.py --terminal
```

The terminal interface uses a classic blue-and-white installer-style screen and is designed to fit an 80-by-25-character terminal. It provides the same upload settings as the GUI:

- EPROM image file
- Serial/COM port selection and refresh
- EPROM type
- Binary-file start address
- Save to SPI EEPROM
- Auto start on power-up
- Show data map
- Submit and Cancel

### Terminal controls

- `Tab` / `Shift+Tab`: move forward/backward between controls
- Arrow keys: move within selectors and the file browser
- `Space` or `Enter`: toggle an option or activate a focused button
- `Ctrl+Q`: quit the main terminal application
- `Ctrl+C`: intentionally disabled in the main form to prevent accidental exit

### ASCII file browser

The Browse button opens an in-terminal filesystem browser without switching to a graphical window.

- Windows drive roots are shown as `[DRIVE] C:\`, `[DRIVE] D:\`, and so on
- Directories are shown as `[DIR]`
- `[..]` moves to the parent directory
- `.hex`, `.bin`, `.rom`, and `.img` files are displayed
- `Enter`, `Alt+O`, or `Ctrl+O` opens the highlighted directory or selects the highlighted file
- `Esc` or `Alt+C` cancels the browser

On macOS, volumes and directories use normal POSIX paths. Serial devices commonly appear as `/dev/cu.usbserial-*` or `/dev/cu.usbmodem-*`.

## Graphical interface

Launch the graphical interface explicitly with:

```console
python EPROM_EMU_NG_2.1rc1.py --gui
```

The GUI provides file selection, COM-port refresh, EPROM and start-address settings, SPI save, auto start, data-map output, and Submit/Cancel controls. GUI preferences are saved between runs.

## Direct command-line mode

Supply both an image file and serial port to upload without opening either interface:

```console
python EPROM_EMU_NG_2.1rc1.py image.bin COM4 -mem 27512 -spi n -auto n -start 0x0000 -map y
```

On macOS, an equivalent command may look like:

```console
python3 EPROM_EMU_NG_2.1rc1.py image.bin /dev/cu.usbserial-110 -mem 27512
```

Run the built-in help for every option:

```console
python EPROM_EMU_NG_2.1rc1.py --help
```

## Supported image formats

- Intel HEX (`.hex`)
- Raw binary images (`.bin`, `.rom`, `.img`, and other non-HEX extensions in direct entry mode)

## Tests

The terminal UI and startup-mode unit tests do not require EPROM hardware:

```console
python -m unittest discover -s tests -v
```

An actual upload remains the final integration test for serial communication and hardware behavior.
