//
// this sketch controls the EPROM memory Emulator hardware described on mygeekyhobby.com
//
// 1.8rc1 Oct-2020
//  - optimized for speed (now loading from SPI via a button or "auto" takes only 3.5s for 27512)
//  - HW version now in EEPROM of the Arduino, so doesn't change every time you upload a new sketch version (update the default HW version below before you run the first time)
//  - only update "last EPROM" if Save to SPI was on... this way when you reload from SPI EEPROM you will also get the last used EPROM type configured on the bus (this was a bug before)
//  - includes the fix as per ERRATA for PCB ver 1.6 and below
//
// 2.0rc1 Oct-2020
//  - support for binary transfer
//  - support for SPI EEPROM addressed transfers
//
// 2.0rc1ultrafast2 2020-12-10
//  - Fast/Ultrafast version by Hubert H.
//  - some fixes
//  - double SPI clock (f/2)
//  - Speed optimizations im SRAM loading
//  - Compile option for unfixed older boards
//
// 2.0rc1ultrafast3 2020-12-14
//  - Ultrafast version by Hubert H.
//  - Ultrafst only, removed Fast Option
//
// 2.0rc3
//  - change the "long-press" LOAD button function from "disable auto-load" to more useful "reset target" function
//

// SPI library not used, SPI programmed directly to registers
//#include <SPI.h>
#include <EEPROM.h>
#include <util/atomic.h>

// The ultra fast code is only valid for ATmega328 based modules (typically Arduino NANO 3.x or UNO)

// Software Fix for boards with SRAM /OE=GND (<= v1.6 without fix)
// Undefine it, if board is v1.6 with fix or higher as v1.6)
#undef UNFIXED
//#define UNFIXED

// Signals to shift refister 74HC595
#define DATA     (3) // data pin for shift register handling D0-D7
#define ADLO     (4) // data pin for shift register handling A0-A7
#define ADHI     (5) // data pin for shift register handling A8-A15
#define SCLK     (7) // clock ping for all shift registers
#define DAT_LD   (6) // latch pin for all shift registers 

// Bits for direct port access to shift register
#define DATAPORT PORTD
#define DATABIT  (3) //PORTD:3
#define ADLOPORT PORTD
#define ADLOBIT  (4) //PORTD:4
#define ADHIPORT PORTD
#define ADHIBIT  (5) //PORTD:5
#define SCLKPORT PORTD
#define SCLKBIT  (7) //PORTD:7
#define DATLDPORT PORTD
#define DATLDBIT (6) //PORTD:6

// Signals to SRAM
#define WE       (8) // SRAM Write Enable Pin

// Bits for direct port access to SRAM
#define WEPORT PORTB
#define WEBIT    (0) //PORTB:0

// Other signals
#define EN_RST (2)   //  internal / external bus control pin
#define LD_BTN (9)   // load data from SPI button
#define LED_G (A0)   // RED LED

// Bits for direct port access to LED_G
#define LEDPIN PINC
#define LEDBIT    (0) // PORTC:0

// Gating signals - ignores selected A11 - A15 address pins besed on EPROM Type
#define EN_A11 (A1)  // A11 data pin gating signal 
#define EN_A12 (A2)  // A12 data pin gating signal 
#define EN_A13 (A3)  // A13 data pin gating signal 
#define EN_A14 (A4)  // A14 data pin gating signal 
#define EN_A15 (A5)  // A15 data pin gating signal 

// SPI EEPROM stuff (using default SPI pins)
#define DATAOUT   MOSI  // 11
#define DATAIN    MISO  // 12
#define SPICLOCK   SCK  // 13
#define SLAVESELECT SS  // 10

// SPI opcodes - taken from Arduino SPI examples
#define WREN  (6)
#define WRDI  (4)
#define RDSR  (5)
#define WRSR  (1)
#define READ  (3)
#define WRITE (2)
#define CE    (0xc7)  // SPI_EEPROM Chip erase (1100 0111)

// Page & buffer sizes
#define PAGESIZE (128)  // Size of page operations in EEPROM 25LC512
#define BBUFSIZE (512)  // Transferbuffersize

// utility byte
byte clr;

// spi_address tracking
word spi_address = 0; // SPI_EEPROM address
unsigned int page_ctn = 0;

// binary transfer buffer
byte byteBuffer [BBUFSIZE + 1];

enum {noROM, rom2kB, rom4kB, rom8kB, rom16kB, rom32kB, rom64kB, lastType};

// configuration addresses in internal EEPROM - use either enumeration or constant variables
enum {mem_cfg, save_cfg, auto_cfg, sram_cfg, ver_cfg = 10};
//const int mem_cfg = 0;  // EEPROM address where we store last used EPROM type (value 1 = 2716, 2 = 2732, 3 = 2764, 4 = 27128, 5 = 27256, 6 = 27512)
//const int save_cfg = 1; // EEPROM address where we store settings for "SAVE to SPI EEPROM option" (value 1 = yes, 0 = no)
//const int auto_cfg = 2; // EEPROM address where we store settings for "Auto load from SPI EEPROM" option (value 1 = yes, 0 = no)
//const int sram_cfg = 3; // EEPROM address where we store last EPROM type loaded in SRAM
//const int ver_cfg = 10; // EEPROM start addres where we store hardware version (up to 10 characters)

// global config settings
bool saveSPI;
bool autoLoad;
unsigned int lastEPROM;

const char FW_ver[] = "2.0rc3";
char HW_ver[10];

// Name strings for romTypes (same order as in enumeration)
const char* memTable[lastType] = {"invalid", "2716", "2732", "2764", "27128", "27256", "27512"}; // lookup table for memory type

void setup() {

  // Initialize all pins
#ifdef UNFIXED
  pinMode( EN_RST, OUTPUT ); digitalWrite( EN_RST, LOW );  // disable outputs to start with AD0-15 from outside
#else
  // At power on, SRAM is undefined. We must fill it first.
  // So we start with holding target in RESET,
  // disabled 541 buffers and enabled shift registers
  // After MCU reset EN_RST is in INPUT mode and therefore floating (but should be pulled to HIGH with an external resistor)
  pinMode( EN_RST, INPUT_PULLUP);  // activates the internal pullup and prepares the pin for immediately HIGH when it is set to OUTPUT mode
  pinMode( EN_RST, OUTPUT );       // now EN_RST is output and HIGH
#endif
  pinMode( DATA, OUTPUT ); digitalWrite( DATA, LOW );
  pinMode( ADLO, OUTPUT ); digitalWrite( ADLO, LOW );
  pinMode( ADHI, OUTPUT ); digitalWrite( ADHI, LOW );

  pinMode( DAT_LD, OUTPUT ); digitalWrite( DAT_LD, LOW );
  pinMode( SCLK, OUTPUT ); digitalWrite( SCLK, LOW );

  // After MCU reset WE is in INPUT mode and therefore floating (but should be pulled to HIGH with an external resistor)
  pinMode( WE, INPUT_PULLUP);  // activates the internal pullup and prepares the pin for immediately HIGH when it is set to OUTPUT mode
  pinMode( WE, OUTPUT );       // now WE is output and HIGH
  pinMode( LED_G, OUTPUT ); digitalWrite( LED_G, LOW );

  // address lines gate signals as outputs
  pinMode( EN_A11, OUTPUT );
  pinMode( EN_A12, OUTPUT );
  pinMode( EN_A13, OUTPUT );
  pinMode( EN_A14, OUTPUT );
  pinMode( EN_A15, OUTPUT );

  setBus(rom64kB); //  default settings to enable all address lines

  pinMode(LD_BTN, INPUT_PULLUP);  // Force High if runs on Arduino without PCB

  // SPI EEPROM pins
  pinMode(DATAOUT, OUTPUT);
  pinMode(DATAIN, INPUT);
  pinMode(SPICLOCK, OUTPUT);
  pinMode(SLAVESELECT, OUTPUT); digitalWrite(SLAVESELECT, HIGH); //disable device

  // SPCR = 01010000; SPSR = 00000001
  //interrupt disabled,spi enabled,msb 1st,master,clk low when idle,
  //sample on leading edge of clk,system clock/2 rate (fastest)
  //results in 8MHz SPI clock. 25LC512 supports at least 10MHz.
  ATOMIC_BLOCK(ATOMIC_FORCEON) {
    SPCR = (1 << SPE) | (1 << MSTR);
    SPSR = (1 << SPI2X);
  } // end ATOMIC
  clr = SPSR;
  clr = SPDR;
  //
  Serial.begin(115200);

  //clearSPIEEPROM(); // one time only erase EEPROM

  // load configuration, set defaults if needed

  //
  // Set default config values if config EEPROM contains invalid values
  // Should only occur during the very first start uf program
  //
  if ((EEPROM.read(mem_cfg) > rom64kB) || (EEPROM.read(mem_cfg) < rom2kB)) {
    EEPROM.update(mem_cfg, rom64kB);
    Serial.println(F("Setting default EPROM Memory to 27512"));
  }
  if ((EEPROM.read(sram_cfg) > rom64kB) || (EEPROM.read(sram_cfg) < rom2kB)) {
    lastEPROM = rom64kB;
    EEPROM.update(sram_cfg, rom64kB);
    Serial.println(F("Setting default SRAM Memory to 27512"));
  }
  else {  // sram_cfg is valid
    lastEPROM = EEPROM.read(sram_cfg) ;
    Serial.println(String(F("Config: Last EPROM used from SRAM is ")) + memTable[lastEPROM]);
    Serial.println(String(F("Config: Last EPROM stored in SPI is  ")) + memTable[EEPROM.read(mem_cfg)]);
  }
  //
  // get "save to SPI_EEPROM" config from EEPROM
  //
  switch (EEPROM.read(save_cfg)) {
    case 0:
      saveSPI = false;
      Serial.println(F("Option: Save to SPI EEPROM set to disable"));
      break;
    case 1:
      saveSPI = true;
      Serial.println(F("Option: Save to SPI EEPROM set to enable"));
      break;
    default:
      saveSPI = false;
      EEPROM.update(save_cfg, 0);
      Serial.println(F("Default: disabling save to spi"));
      break;
  }
  //
  // get "auto load from SPI_EEPROM" config from EEPROM
  //
  switch (EEPROM.read(auto_cfg)) {
    case 0:
      autoLoad = false;
      Serial.println(F("Option: Auto load from SPI EEPROM set to disable"));
      break;
    case 1:
      autoLoad = true;
      Serial.println(F("Option: Auto load from SPI EEPROM set to enable"));
      break;
    default:
      autoLoad = false;
      EEPROM.update(auto_cfg, 0);
      Serial.println(F("Default: Disabling Auto load from SPI EEPROM "));
      break;
  }

  // load data from SPI EEPROM if configured

  if (autoLoad) {  // we use the SPI stored image an start emulation
    lastEPROM = EEPROM.read(mem_cfg);
    load_SPIEEPROM(lastEPROM);  // Fills the SRAM from SPI, setBus() and release the target RESET (start emulation)
  } else {         // we use the last in SRAM stored image and remain in reset
    setBus(lastEPROM);
  }

  //
  // get hardware version from EEPROM
  //
  EEPROM.get(ver_cfg, HW_ver); // load version from EEPROM

  if (HW_ver[0] != 'v') {
    // set default version
    Serial.println(F("setting default internal hw version"));
    //
    EEPROM.put(ver_cfg, "v0.0"); //Set this to whatever you have on the PCB, if you did the HW mod as per Errata, call it v1.6f or similar   <--------- README
    EEPROM.get(ver_cfg, HW_ver);
  }
  //
  // if you missed setting up the hw version before, you can temporary replace the "if((HW_ver[0] != 'v')" with "if(1)"
  //

  Serial.println(F("\nEmulator Started, waiting for computer contol"));
  printabout();
} // end setup()

//
// restore SRAM from SPI EEPROM
//
void load_SPIEEPROM (unsigned int eprom) {

  unsigned long time = millis(); // Start time measurement
  setBus(rom64kB);              // allows access to entire 64kb SRAM Memory - equivlet of emulating 27512 eprom
#ifdef UNFIXED
  digitalWrite( WE, LOW);       // brings data bus of SRAM to high-z (only unmodified 1.6 boards)
#endif
  digitalWrite( EN_RST, HIGH ); // reset target, disable 541 buffers, enable shift registers
  digitalWrite( LED_G, LOW );

  unsigned int maxPage;
  // Calculate number of pages: #pages = eprom_size / PAGESIZE
  switch (eprom) {
    case rom2kB:
      maxPage = (1L << 11) / PAGESIZE; // A0..A10 16kb/2kB
      break;
    case rom4kB:
      maxPage = (1L << 12) / PAGESIZE; // A0..A11 32kb/4kB
      break;
    case rom8kB:
      maxPage = (1L << 13) / PAGESIZE; // A0..A12 64kb/8kB
      break;
    case rom16kB:
      maxPage = (1L << 14) / PAGESIZE; // A0..A13 128kb/16kB
      break;
    case rom32kB:
      maxPage = (1L << 15) / PAGESIZE; // A0..A14 256kb/32kB
      break;
    case rom64kB:
      maxPage = (1L << 16) / PAGESIZE; // A0..A15 512kb/64kB
      break;
    default:
      Serial.print(F( "-> Default: "));
      maxPage = (1L << 16) / PAGESIZE; // A0..A15 512kb/64kB
      break;
  }
  Serial.println(String(F("-> Loading ")) + memTable[eprom] + String(F(" EPROM from SPI")));

  // now load this many pages from SPI EEPROM

  int clmn = 1;

  for ( unsigned int page = 0; page < maxPage; page++) {
    if (spi_address & 00010000) {   // flash LED while loading
      LEDPIN = 1 << LEDBIT;         // Toogle bit on pin A0 (by writing 1 in PINC:0 register)
    }

    digitalWrite(SLAVESELECT, LOW);      // select SPI EEPROM
    spi_transfer(READ);                     //transmit read opcode
    spi_transfer((byte)(spi_address >> 8)); //send MSByte address first
    spi_transfer((byte)(spi_address));      //send LSByte address

    for (int i = 0; i < PAGESIZE; i++) {
      SPDR = 0;                        // Start the transmission, clear pending SPIF flag
      while (!(SPSR & (1 << SPIF))) {} // Wait for the end of the transmission (SPIF is set)
      writeMemoryLocation(spi_address++, SPDR);  // get received byte and write it to SRAM, clear SPIF flag
    }
    digitalWrite(SLAVESELECT, HIGH);   //release chip, signal end transfer
    Serial.write('.');
    if (++clmn > 64) {
      Serial.write('\n');
      clmn = 1;
    }
  }
  Serial.write('\n');

  EEPROM.update(sram_cfg, eprom);  // After load from SPI, SRAM and SPI is the same ROM type (sram_cfg = mem_cfg)
  setBus(eprom);                   // now that SRAM is loaded with data, we can enable correct address lines

#ifdef UNFIXED
  digitalWrite( WE, HIGH);         // enable SRAM outputs, emulator ist now ready for target access
#endif
  digitalWrite( EN_RST, LOW );     // disable shift registers, address enable buffers, release target RESET
  digitalWrite( LED_G, HIGH );     // turn on "RUN" LED
  Serial.println(String(F("Done loading from SPI EEPROM to SRAM in ")) + (millis() - time) + String(F("ms")));
} // end load_SPIEEPROM()

//
// procedure to setup address for spi transaction
//

void clearSPIEEPROM () {
  // send write enable command to unlock writting to EEPROM
  digitalWrite(SLAVESELECT, LOW);
  spi_transfer(WREN);
  digitalWrite(SLAVESELECT, HIGH);

  // chip erase command
  digitalWrite(SLAVESELECT, LOW);
  spi_transfer(CE);            //chip erase instruction
  digitalWrite(SLAVESELECT, HIGH);
  delay(10);
} // end clearSPIEEPROM()

//
// procedure to setup address for spi transaction
//

void setAddressSPI (word address) {
  // send write enable command to unlock writting to EEPROM
  digitalWrite(SLAVESELECT, LOW);
  spi_transfer(WREN);
  digitalWrite(SLAVESELECT, HIGH);

  // write command
  digitalWrite(SLAVESELECT, LOW);
  spi_transfer(WRITE);            //write instruction

  // set start address
  spi_transfer((byte)(address >> 8));
  spi_transfer((byte)(address));
} // end setAddressSPI()

//
// new as of 2.0
//
// data should be in the byteBuffer, either 128, 256, 384 or 512 bytes
//
void writeBufferSPI(word address, unsigned int bytecount) {
  // process data from byteBuffer

  setAddressSPI(address); // set initial spi address

  int m = 0; // page counter for SPI EEPROM
  for (int k = 0; k < bytecount; k++) {
    // every 128 bytes issue a save command and set address
    if (m == PAGESIZE ) {
      // every 128 bytes write data and set new address
      digitalWrite(SLAVESELECT, HIGH);
      // wait for write to finish, for 25LC512 Twc = 5ms
      delay(6);
      setAddressSPI(address); // set address
      m = 0;
    }
    spi_transfer(byteBuffer[k + 1]);
    address++;
    m++;
  }
  digitalWrite(SLAVESELECT, HIGH);
} // end writeBufferSPI()

// SPI EEPROM write routine - rename to spi_write ??

inline void spi_transfer(byte data) {
  SPDR = data;                    // Start the transmission, clear pending SPIF flag
  while (!(SPSR & (1 << SPIF)))   // Wait for the end of the transmission (SPIF is set)
  {
  };                              // SPIF is cleared during next access to SPDR which happens at the next "start transmision"
} // end spi_transfer()


//
// print "about" message
//
void printabout() {
  Serial.print(F("-> HW: ")); Serial.print(HW_ver);
  Serial.print(F(" @ 115200, FW: ")); Serial.print(FW_ver);
  Serial.print(F(", SPI: ")); Serial.print(saveSPI);
  Serial.print(F(", Auto: ")); Serial.print(autoLoad);
  Serial.print(F(", Last EPROM: ")); Serial.print(memTable[lastEPROM]);
  Serial.println(F(", mygeekyhobby.com 2020 :)"));
} // end printabout()

//  Adjusts address lanes A11 - A15, based on imput EPROM type is 2716 = 1 ... 27512 = 6

void setBus (unsigned int eprom) {
  switch (eprom) {
    case rom2kB:
      digitalWrite( EN_A11, LOW );
      digitalWrite( EN_A12, LOW );
      digitalWrite( EN_A13, LOW );
      digitalWrite( EN_A14, LOW );
      digitalWrite( EN_A15, LOW );
      break;
    case rom4kB:
      digitalWrite( EN_A11, HIGH );
      digitalWrite( EN_A12, LOW );
      digitalWrite( EN_A13, LOW );
      digitalWrite( EN_A14, LOW );
      digitalWrite( EN_A15, LOW );
      break;
    case rom8kB:
      digitalWrite( EN_A11, HIGH );
      digitalWrite( EN_A12, HIGH );
      digitalWrite( EN_A13, LOW );
      digitalWrite( EN_A14, LOW );
      digitalWrite( EN_A15, LOW );
      break;
    case rom16kB:
      digitalWrite( EN_A11, HIGH );
      digitalWrite( EN_A12, HIGH );
      digitalWrite( EN_A13, HIGH );
      digitalWrite( EN_A14, LOW );
      digitalWrite( EN_A15, LOW );
      break;
    case rom32kB:
      digitalWrite( EN_A11, HIGH );
      digitalWrite( EN_A12, HIGH );
      digitalWrite( EN_A13, HIGH );
      digitalWrite( EN_A14, HIGH );
      digitalWrite( EN_A15, LOW );
      break;
    case rom64kB:  // same setting as default:
      digitalWrite( EN_A11, HIGH );
      digitalWrite( EN_A12, HIGH );
      digitalWrite( EN_A13, HIGH );
      digitalWrite( EN_A14, HIGH );
      digitalWrite( EN_A15, HIGH );
      break;
    default:
      digitalWrite( EN_A11, HIGH );
      digitalWrite( EN_A12, HIGH );
      digitalWrite( EN_A13, HIGH );
      digitalWrite( EN_A14, HIGH );
      digitalWrite( EN_A15, HIGH );
      break;
  }
  Serial.println(String(F("-> Bus set to ")) + memTable[eprom] + String(F(" EPROM Memory")));
} // end setBus()

//
// process the binary data buffer, new as of 2.0
//
void writeBuffer(word address, unsigned int bytecount) {
  // process data from byteBuffer
  for (int k = 0; k < bytecount; k++) {
    writeMemoryLocation(address++, byteBuffer[k + 1]);
  }
} // end writeBuffer()

// Write single byte of data to SRAM
// Redesigned by HubertH
// AS6C62256-55: twp_min = 45ns
// HM62256B-8:   twp_min = 55ns
// 74HC595:      SCK, RCK min pulse width (-40°..+85°)
//               Vcc = 2V:   95ns
//               Vcc = 4,5V: 19ns

void writeMemoryLocation (word address, byte memData ) {
  register byte addressHi =  address >> 8 ;
  register byte addressLo = address & 0xFF ;
  //  unsigned char memData =  data ;
  // send data

  ATOMIC_BLOCK(ATOMIC_FORCEON) {
    for (uint8_t i = 0; i < 8; ++i) {
      if (memData & 0x80) {
        DATAPORT |= (1 << DATABIT);
      } else {
        DATAPORT &= ~(1 << DATABIT);
      }

      if (addressHi & 0x80) {
        ADHIPORT |= (1 << ADHIBIT);
      } else {
        ADHIPORT &= ~(1 << ADHIBIT);
      }

      if (addressLo & 0x80) {
        ADLOPORT |= (1 << ADLOBIT);
      } else {
        ADLOPORT &= ~(1 << ADLOBIT);
      }
      SCLKPORT |= (1 << SCLKBIT);  // SCLK LOW to HIGH
      SCLKPORT &= ~(1 << SCLKBIT); // SCLK HIGH to LOW, ~125ns pulse width
      // Shift address and data bits
      memData <<= 1;
      addressHi <<= 1;
      addressLo <<= 1;
    }
    // latch data and address out
    DATLDPORT |= (1 << DATLDBIT);   // DAT_LD LOW to HIGH
    DATLDPORT &= ~(1 << DATLDBIT);  // DAT_LD HIGH to LOW, ~125ns pulse width

    // at this point we have data and address loaded into the shift registers
    // now we can request a write to SRAM memory
    WEPORT &= ~(1 << WEBIT);     // /WE HIGH to LOW
    WEPORT |= (1 << WEBIT);      // /WE LOW to HIGH, ~125ns pulse width
  } // end ATOMIC
} // end writeMemoryLocation()


// process received data frame from PC

char ch = 0;
String rxFrame = "";

// main program loop

void loop() {

if (digitalRead(LD_BTN) == LOW) {     // Yeah, the button is pressed
    delay(500);                         // Wait 0.5s and look again
    if (digitalRead(LD_BTN) == HIGH) {  // Button released after 500ms = short press
      // short press will load from SPI_EEPROM
      Serial.println(F("Load from SPI EEPROM triggered by pushbutton"));
      lastEPROM = EEPROM.read(mem_cfg);
      load_SPIEEPROM(lastEPROM);
    } else {                             // Button not released after 500ms
      delay(1000);                       // Wait 1s more
      if (digitalRead(LD_BTN) == LOW) {  // Button not released after (500+1000)ms = long press
        // long press resets target
        digitalWrite( EN_RST, HIGH);
        delay(150);
        digitalWrite( EN_RST, LOW );
        Serial.println(F("-> Resetting target via button"));
        digitalWrite( LED_G, LOW );   // turn the LED off to indicate the RESET was succesful
        while (digitalRead(LD_BTN) == LOW) {
          delay(100);                    // wait for releasing button
        }
        digitalWrite( LED_G, HIGH );    //
      }
    }
    delay(3000);                         // next possible push in 3s

  } // end processing push button

  if (Serial.available() > 0) {
    char ch = Serial.read();
    if ( ch == 0x0D /*CR*/) {
      return;
    }
    // detect end of frame
    if ( ch == 0x0A /*LF*/) {
      // decode frame

      //
      // get sync or stop emulation (like :EMUOFF)
      //
      if ( rxFrame == ":dml" ) {
#ifdef UNFIXED
        digitalWrite( WE, LOW);       // brings data bus of SRAM to high-z (only unmodified 1.6 boards)
#endif
        digitalWrite( EN_RST, HIGH ); // reset target, disable 541 buffers, enable shift registers
        digitalWrite( LED_G, LOW );   // RUN LED off
        printabout();
      }
      //
      // start emulation
      //
      if ( rxFrame == ":EMUON" ) {
#ifdef UNFIXED
        digitalWrite( WE, HIGH);      // enable SRAM outputs
#endif
        digitalWrite( EN_RST, LOW );  // disable shift registers, address enable buffers.
        digitalWrite( LED_G, HIGH );  // turn on "RUN" LED

        Serial.println(F("-> Emulator Running."));
        spi_address = 0;              // may have to remove that later
      }
      //
      // Load SRAM from SPI
      //
      if ( rxFrame == ":LOADSPI") {
        lastEPROM = EEPROM.read(mem_cfg);
        load_SPIEEPROM(lastEPROM);
      }

      if ( rxFrame == ":ini16" ) {
        setBus(rom2kB);
        EEPROM.update(sram_cfg, rom2kB);
        if (saveSPI) {
          EEPROM.update(mem_cfg, rom2kB);
        }
      }
      if ( rxFrame == ":ini32" ) {
        setBus(rom4kB);
        EEPROM.update(sram_cfg, rom4kB);
        if (saveSPI) {
          EEPROM.update(mem_cfg, rom4kB);
        }
      }
      if ( rxFrame == ":ini64" ) {
        setBus(rom8kB);
        EEPROM.update(sram_cfg, rom8kB);
        if (saveSPI) {
          EEPROM.update(mem_cfg, rom8kB);
        }
      }
      if ( rxFrame == ":ini128" ) {
        setBus(rom16kB);
        EEPROM.update(sram_cfg, rom16kB);
        if (saveSPI) {
          EEPROM.update(mem_cfg, rom16kB);
        }
      }
      if ( rxFrame == ":ini256" ) {
        setBus(rom32kB);
        EEPROM.update(sram_cfg, rom32kB);
        if (saveSPI) {
          EEPROM.update(mem_cfg, rom32kB);
        }
      }
      if ( rxFrame == ":ini512" ) {
        setBus(rom64kB);
        EEPROM.update(sram_cfg, rom64kB);
        if (saveSPI) {
          EEPROM.update(mem_cfg, rom64kB);
        }
      }
      // data will be also written to SPI
      if ( rxFrame == ":iniSPI1" ) {
        saveSPI = true;
        EEPROM.update(save_cfg, 1);
        Serial.println(F("-> SPI save ON"));
        spi_address = 0;
      }
      // data will be also written to SPI
      if ( rxFrame == ":iniSPI0" ) {
        saveSPI = false;
        EEPROM.update(save_cfg, 0);
        Serial.println(F("-> SPI save OFF"));
      }
      //
      // process binary frame, new as of 2.0, process binary frame format <start address><bytecount><payload up to size of buffer> in chunks of 128Bytes
      //
      if ( rxFrame == ":SBN" ) {
        Serial.println(F("->in"));
        unsigned int startaddr = Serial.parseInt();
        unsigned int bytecount = Serial.parseInt();

        // now we know start address and how many bytes are coming
        int transfered = Serial.readBytes(byteBuffer, bytecount + 1); // may need bytecount + 1
        // now write the buffer to SRAM
        writeBuffer(startaddr, bytecount);
        if (saveSPI) {
          writeBufferSPI(startaddr, bytecount);
        }
        Serial.println(F("ACK"));
      }
      // load the SPI EEPROM data to SRAM on boot.
      if ( rxFrame == ":iniAuto1" ) {
        autoLoad = true;
        EEPROM.update(auto_cfg, 1);
        Serial.println(F("-> Auto Load ON"));
      }
      // data will be also written to SPI
      if ( rxFrame == ":iniAuto0" ) {
        autoLoad = false;
        EEPROM.update(auto_cfg, 0);
        Serial.println(F("-> Auto Load OFF"));
      }
      // Erase SPI EEPROM ( you can call it from Arduiono serial monitor by typing :SPICLR)
      if ( rxFrame == ":SPICLR" ) {
        clearSPIEEPROM();
        Serial.println(F("->ERASED SPI EEPROM"));
      }

      rxFrame = "";
    }
    else {
      rxFrame += ch;
    }
  }
} // end loop()
