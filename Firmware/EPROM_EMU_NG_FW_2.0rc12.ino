// 
// This sketch controls the EPROM memory Emulator hardware described on mygeekyhobby.com, code is maintained by Kris Sekula with support from community 
// Parts of this code have been kindly contributed by Ralf-Peter Nerlich
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
// 2.0rc2 Dec-2020 
//		- eliminate pull up on button by using the CPU built in pull up resistors
//		- change the "long-press" LOAD button function from "disable auto-load" to more useful "reset target" function
// 2.0rc6 Mar-2021
//    - speed increase by optimising pin/port control with low level code instead of "digitalwrite"
//    - SPI speed increas to 8MHz
//	  - Added support for 28C256
// 2.0rc7 Mar-2021
//    - Added support for 28C64 (it's just a label as 2764 = 28C64 pinout)
// 2.0rc8 Mar-2021
//    - Added direct to SRAM upload (this is only used when "save to SPI EEPROM" is not enabled, and improves speed by shifting data from serial directly to SRAM without extra buffor)
// 2.0rc9 Mar-2021
//	  - prevously filename rc8 had rc7 config inside, now changed to rc9 to match the SW rc9
// 2.0rc10 Aug-2021
//    - added a fix for black nano with vqfn atmega328p
// 2.0rc11 Nov-2021, a number of fixes and improvements kindly contributed by by Ralf-Peter Nerlich <early8bitz.de> 2021-11-14
//    - fixed EPROM type 8 (28C64) handling in setup()
//    - changed numeric EPROM IDs (memory codes) to symbolic names for clarity
//    - corrected help entries for :iniE256 and :iniE64
// 2.0rc12 Dec-2021
//	  - fix for operation without the 100nF reset capacitor (auto-reset on connect disabled in HW)

#include <EEPROM.h>

// pin configurations

#define DATA 3  // data pin for shift register handling D0-D7
#define ADLO 4  // data pin for shift register handling A0-A7
#define ADHI 5  // data pin for shift register handling A8-A15
#define SCLK 7  // clock ping for all shift registers
#define DAT_LD 6  // latch pin for all shift registers  PD6

// Gating signals - ignores selected A11 - A15 address pins besed on EPROM Type
#define EN_A11 A1  // A11 data pin gating signal 
#define EN_A12 A2  // A12 data pin gating signal 
#define EN_A13 A3  // A13 data pin gating signal 
#define EN_A14 A4  // A14 data pin gating signal 
#define EN_A15 A5  // A15 data pin gating signal


#define EN_RST 2  //  internal / external bus control pin
#define WE 8      // SRAM Write Enable Pin

#define LD_BTN 9  // load data from SPI button
#define LED_G A0  // RED LED


// low level ping / port control
#define ADHI_LO PORTD &= ~(1<<PD5) // set LOW
#define ADHI_HI PORTD |=  (1<<PD5) // set High

#define ADLO_LO PORTD &= ~(1<<PD4) // set LOW
#define ADLO_HI PORTD |=  (1<<PD4) // set High

#define DATA_LO PORTD &= ~(1<<PD3) // set LOW
#define DATA_HI PORTD |=  (1<<PD3) // set High

#define SCLK_LO PORTD &= ~(1<<PD7) // set LOW
#define SCLK_HI PORTD |=  (1<<PD7) // set High

#define DAT_LD_LO PORTD &= ~(1<<PD6) // set LOW
#define DAT_LD_HI PORTD |=  (1<<PD6) // set High

#define WE_LO PORTB &= ~(1<<PB0) // set LOW
#define WE_HI PORTB |=  (1<<PB0) // set High

#define SS_LO PORTB &= ~(1<<PB2) // set LOW
#define SS_HI PORTB |=  (1<<PB2) // set High

#define LED_OFF PORTC &= ~(1<<PC0) // LED OFF
#define LED_ON PORTC |=  (1<<PC0) // LED ON
#define LED_FLIP PINC = bit(PC0) // toggle LED

// SPI EEPROM stuff

#define DATAOUT 11  //MOSI
#define DATAIN  12  //MISO
#define SPICLOCK  13  //sck
#define SLAVESELECT 10  //ss

// SPI opcodes - taken from Arduino SPI examples
#define WREN  6
#define WRDI  4
#define RDSR  5
#define WRSR  1
#define READ  3
#define WRITE 2
#define CE 199 

// utility byte
byte clr;

// spi_address tracking
unsigned int spi_address = 0; // SPI_EEPROM address
unsigned int page_ctn = 0;

// binary transfer buffer
byte byteBuffer [513];

// configuration addresses in internal EEPROM
int mem_cfg = 0;  // EEPROM address where we store last used EPROM information (value 1 = 2716, 2 = 2732, 3 = 2764, 4 = 27128, 5 = 27256, 6 = 27512, 7 = 28256)
int save_cfg = 1; // EEPROM address where we store settings for "SAVE to SPI EEPROM option" (value 1 = yes, 2 = no)
int auto_cfg = 2; // EEPROM address where we store settings for "Auto load from SPI EEPROM" option (value 1 = yes, 2 = no)
int ver_cfg = 10;  // EEPROM start addres where we store hardware version (up to 10 characters)

// global config settings
bool saveSPI;
bool autoLoad;
unsigned int lastEPROM;
                       
const char FW_ver[] = "2.0rc12";
char HW_ver[10];

// ID numbers for ROM types
enum {noROM, rom16K, rom32K, rom64K, rom128K, rom256K, rom512K, romE256K, romE64K, lastType};

// Name strings for  ROM types (same order as in enumeration)
const char* memTable[lastType] = {"27xx","2716","2732","2764","27128","27256","27512","28C256","28C64"}; // lookup table for memory type

void setup() {
  //Serial.begin(1000000);
  Serial.begin(115200);
  
  pinMode( WE, OUTPUT ); digitalWrite( WE, LOW );
  pinMode( EN_RST, OUTPUT ); digitalWrite( EN_RST, HIGH );  // disable emulaton, wait for computer control or autload

  pinMode( DATA, OUTPUT ); digitalWrite( DATA, LOW );
  pinMode( ADLO, OUTPUT ); digitalWrite( ADLO, LOW );
  pinMode( ADHI, OUTPUT ); digitalWrite( ADHI, LOW );

  pinMode( DAT_LD, OUTPUT ); digitalWrite( DAT_LD, LOW );
  pinMode( SCLK, OUTPUT ); digitalWrite( SCLK, LOW );


  pinMode( LED_G, OUTPUT ); digitalWrite( LED_G, LOW );

  // address lines gate signals as outputs
  pinMode( EN_A11, OUTPUT );
  pinMode( EN_A12, OUTPUT );
  pinMode( EN_A13, OUTPUT ); 
  pinMode( EN_A14, OUTPUT );
  pinMode( EN_A15, OUTPUT );

  //
  pinMode(LD_BTN,INPUT_PULLUP); 
  
  setBus(rom512K); //  default settings to enable all address lines

  // SPI EEPROM pins
  pinMode(DATAOUT, OUTPUT);
  pinMode(DATAIN, INPUT);
  pinMode(SPICLOCK,OUTPUT);
  pinMode(SLAVESELECT,OUTPUT); digitalWrite(SLAVESELECT,HIGH); //disable device

  // SPCR = 01010000
  //interrupt disabled,spi enabled,msb 1st,master,clk low when idle,
  //sample on leading edge of clk,system clock/2 rate (8MHz SPI clock)
  SPCR = (1<<SPE)|(1<<MSTR);
  SPSR = (1 << SPI2X);
  clr=SPSR;
  clr=SPDR;

  // load configuration, set defaults if needed
  
  //
  // get "last used EPROM" config from SPI_EEPROM
  //
  if ((EEPROM.read(mem_cfg) >= lastType) || (EEPROM.read(mem_cfg) < rom16K)) {
    EEPROM.update(mem_cfg, rom512K);
    Serial.println(F("Setting default EPROM Memory to 27512"));
  }
  else {
    lastEPROM = EEPROM.read(mem_cfg) ;
    Serial.println(String("Config: Last EPROM used is ") + memTable[lastEPROM]);
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
      EEPROM.write(save_cfg,0);
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
      EEPROM.write(auto_cfg,0);
      Serial.println(F("Default: Disabling Auto load from SPI EEPROM "));
      break;
  }
  
  // fix for black nano with atmega328p vqfn package
  writeMemoryLocation(0x0001,0x00);
  writeMemoryLocation(0x0001,0x00);
  //
  
  // load data from SPI EEPROM if configured

  if (autoLoad == true ) {
    load_SPIEEPROM(lastEPROM);
  }

  //
  // get hardware version from EEPROM
  //
  EEPROM.get(ver_cfg, HW_ver); // load version from EEPROM
  
  if(HW_ver[0] != 'v') {
    // set default version
    Serial.println(F("setting default internal hw version"));
    //
    EEPROM.put(ver_cfg, "v2.1"); //Set this to whatever you have on the PCB, if you did the HW mod as per Errata, call it v1.6f or similar   <--------- README                                  
    EEPROM.get(ver_cfg, HW_ver);
  }
  //
  // if you missed setting up the hw version before, you can temporary replace the "if((HW_ver[0] != 'v')" with "if(1)"
  //

  Serial.println("");
  Serial.println(F("Emulator Started, waiting for computer contol"));
  printabout();
}

//
// restore SRAM from SPI EEPROM
//

void load_SPIEEPROM (int eprom) {
  
  setBus(rom512K); // allows access to enire 64KiB SRAM Memory - equivlet of emulating 27512 eprom
  digitalWrite( WE, LOW);       // brings data bus of SRAM to high-z
  digitalWrite( EN_RST, HIGH ); // reset target, disable 541 buffers, enable shift registers
  digitalWrite( LED_G, LOW );
  
  unsigned int max_size;
  switch (eprom) {
    case rom16K:
      Serial.println(F( "-> Loading 2716 EPROM from SPI"));
      max_size = 16; // pages of 128 bytes
      break;
    case rom32K:
      Serial.println(F( "->  Loading 2732 EPROM from SPI"));   
      max_size = 32; // pages of 128 bytes
      break;
    case rom64K:
      Serial.println(F( "->  Loading 2764 EPROM from SPI"));
      max_size = 64; // pages of 128 bytes
      break;
    case rom128K:
      Serial.println(F( "->  Loading 27128 EPROM from SPI"));
      max_size = 128; // pages of 128 bytes
      break;
    case rom256K:
      Serial.println(F( "-> Loading 27256 EPROM from SPI"));
      max_size = 256; // pages of 128 bytes
      break;
    case rom512K:
      Serial.println(F( "-> Loading 27512 EPROM from SPI"));
      max_size = 512; // pages of 128 bytes
      break;
    case romE256K:
      Serial.println(F( "-> Loading 28C256 EEPROM from SPI"));
      max_size = 512; // pages of 128 bytes
      break;
     case romE64K:
      Serial.println(F( "->  Loading 28C64 EEPROM from SPI"));
      max_size = 64; // pages of 128 bytes
      break;
    default:
      Serial.println(F( "-> Default: Loading 27512 EPROM from SPI"));
      max_size = 512; // pages of 128 bytes
      break;
  }

  // now load this many pages from SPI EEPROM
  
  byte SPIdata;
  int clmn = 0;
  
  for (unsigned int page = 0; page < max_size; page++) {

    // visual feedback to user that trasfer from SPI EEPROM to SRAM is happening
    if (spi_address & 0010000) {
      LED_FLIP;
    }

    SS_LO; // select SPI EEPROM
    
    spi_transfer(READ);                     //transmit read opcode
    spi_transfer((byte)(spi_address>>8));   //send MSByte address first
    spi_transfer((byte)(spi_address));      //send LSByte address
    
    for (int i=0; i<128; i++) {
      SPIdata = spi_transfer(0xFF); //get data byte by trasfering dummy 0xff data to spi eeprom
      writeMemoryLocation(spi_address, SPIdata);
      spi_address++;
    }
    //digitalWrite(SLAVESELECT,HIGH); //release chip, signal end transfer
    SS_HI;  // release SPI EEPROM
    
    // this section is so that the python script trying to connect to the Arduino knows SPI loading is happening and can wait for it to finish.
    Serial.print(".");
    clmn ++;
    if (clmn > 63) {
      Serial.println("");
      clmn = 0;
    }
  }
 
  setBus(eprom); // now that SRAM is loaded with data, we can enable correct address lines

  digitalWrite( EN_RST, LOW );  // disable shift registers, address enable buffers.
  digitalWrite( WE, HIGH);      // enable SRAM outputs
  digitalWrite( LED_G, HIGH );  // turn on "RUN" LED
  
  Serial.println(F("Done loading from SPI EEPROM to SRAM"));  
}

//
// procedure to setup erase SPI EEPROM
//

void clearSPIEEPROM (){
   // send write enable command to unlock writting to EEPROM

      //digitalWrite(SLAVESELECT,LOW);
      SS_LO;
      spi_transfer(WREN);
      //digitalWrite(SLAVESELECT,HIGH);
      SS_HI;

      // chip erase command
      //digitalWrite(SLAVESELECT,LOW);
      SS_LO;
      spi_transfer(CE);            //chip erase instruction
      //digitalWrite(SLAVESELECT,HIGH);
      SS_HI;
      delay(10);
}

//
// procedure to setup address for spi transaction
//

void setAddressSPI (unsigned short address){
   // send write enable command to unlock writting to EEPROM

      //digitalWrite(SLAVESELECT,LOW);
      SS_LO;
      spi_transfer(WREN);
      //digitalWrite(SLAVESELECT,HIGH);
      SS_HI;

      // write command
      //digitalWrite(SLAVESELECT,LOW);
      SS_LO;
      spi_transfer(WRITE);            //write instruction
      
      // set start address
      spi_transfer((char)(address>>8));
      spi_transfer((char)(address));
}

//
// new as of 2.0
//
// data should be in the byteBuffer, either 128, 256, 384 or 512 bytes
// 
void writeBufferSPI(unsigned short address, unsigned int bytecount) {
  // process data from byteBuffer
 
  setAddressSPI(address); // set initial spi address
  
  int m=0; // page counter for SPI EEPROM
  for (int k=0; k<bytecount; k++) {
    // every 128 bytes issue a save command and set address
    if(m == 128 ) {
      // every 128 bytes write data and set new address
      SS_HI;
      // wait for write to finish, for 25LC512 Twc = 5ms, here 5.05ms 
      delay(5);
      delayMicroseconds(50); 
      setAddressSPI(address); // set address
      m=0;
    }
    spi_transfer(byteBuffer[k+1]);
    address++;
    m++;
  }
  SS_HI;
}

// SPI EEPROM transfer routine

char spi_transfer(volatile char data)
{
  SPDR = data;                    // Start the transmission
  while (!(SPSR & (1<<SPIF)))     // Wait for the end of the transmission
  {
  };
  return SPDR;                    // return the received byte
}

//
// print "about" message 
//
void printabout() {    
          Serial.print(F("-> HW: ")); Serial.print(HW_ver);
          Serial.print(F(", FW: ")); Serial.print(FW_ver);
          Serial.print(F(", SPI: ")); Serial.print(saveSPI);
          Serial.print(F(", Auto: ")); Serial.print(autoLoad);
          Serial.print(F(", Last EPROM: ")); Serial.print(memTable[lastEPROM]);        
          Serial.println(F(", mygeekyhobby.com 2020 :)"));
}

//  Adjusts address lanes A11 - A15, based on imput EPROM type is 2716 = 1 ... 27512 = 6

void setBus (int eprom){
  switch (eprom) {
    case rom16K:
      Serial.println(F( "-> Bus set to 2716 EPROM Memory"));
      digitalWrite( EN_A11, LOW );
      digitalWrite( EN_A12, LOW );
      digitalWrite( EN_A13, LOW );
      digitalWrite( EN_A14, LOW );
      digitalWrite( EN_A15, LOW );
      break;
    case rom32K:
      Serial.println(F( "-> Bus set to 2732 EPROM Memory"));
      digitalWrite( EN_A11, HIGH );
      digitalWrite( EN_A12, LOW );
      digitalWrite( EN_A13, LOW );
      digitalWrite( EN_A14, LOW );
      digitalWrite( EN_A15, LOW );     
      break;
    case rom64K:
      Serial.println(F( "-> Bus set to 2764 EPROM Memory"));
      digitalWrite( EN_A11, HIGH );
      digitalWrite( EN_A12, HIGH );
      digitalWrite( EN_A13, LOW );
      digitalWrite( EN_A14, LOW );
      digitalWrite( EN_A15, LOW );
      break;
    case rom128K:
      Serial.println(F( "-> Bus set to 27128 EPROM Memory"));
      digitalWrite( EN_A11, HIGH );
      digitalWrite( EN_A12, HIGH );
      digitalWrite( EN_A13, HIGH );
      digitalWrite( EN_A14, LOW );
      digitalWrite( EN_A15, LOW );
      break;
    case rom256K:
      Serial.println(F( "-> Bus set to 27256 EPROM Memory"));
      digitalWrite( EN_A11, HIGH );
      digitalWrite( EN_A12, HIGH );
      digitalWrite( EN_A13, HIGH );
      digitalWrite( EN_A14, HIGH );
      digitalWrite( EN_A15, LOW );
      break;
    case rom512K:
      Serial.println(F( "-> Bus set to 27512 EPROM Memory"));
      digitalWrite( EN_A11, HIGH );
      digitalWrite( EN_A12, HIGH );
      digitalWrite( EN_A13, HIGH );
      digitalWrite( EN_A14, HIGH );
      digitalWrite( EN_A15, HIGH );
      break;
    case romE256K:
      Serial.println(F( "-> Bus set to 28C256 EEPROM Memory"));
      digitalWrite( EN_A11, HIGH );
      digitalWrite( EN_A12, HIGH );
      digitalWrite( EN_A13, HIGH );
      digitalWrite( EN_A14, LOW );
      digitalWrite( EN_A15, HIGH );
      break;
    case romE64K:
      Serial.println(F( "-> Bus set to 28C64 EPROM Memory"));
      digitalWrite( EN_A11, HIGH );
      digitalWrite( EN_A12, HIGH );
      digitalWrite( EN_A13, LOW );
      digitalWrite( EN_A14, LOW );
      digitalWrite( EN_A15, LOW );
      break;
    default:
      Serial.println(F( "-> Bus set to 27512 EPROM Memory"));
      digitalWrite( EN_A11, HIGH );
      digitalWrite( EN_A12, HIGH );
      digitalWrite( EN_A13, HIGH );
      digitalWrite( EN_A14, HIGH );
      digitalWrite( EN_A15, HIGH );
      break;
  }
  return;
}

//
// process the binary data buffer, new as of 2.0
//   
void writeBuffer(unsigned short address, unsigned int bytecount) {
  // process data from byteBuffer
  for (int k=0; k<bytecount; k++) {
    writeMemoryLocation(address,byteBuffer[k+1]);
    address++; 
  }
}

//
// Write single byte of data to SRAM
//

void writeMemoryLocation (word address, byte memData ) {
  
  // disable interrputs
  cli();

  for (uint8_t i = 0; i < 8; ++i) {
      // shift address and data to the HC595
      
      if (memData & (1 << 7)) {    // Mask data byte MSB (Bit 7)
        DATA_HI;
      } else {
        DATA_LO;
      }

      if (address & (1 << 15)) {   // Mask address high byte MSB (Bit 15)
        ADHI_HI;
      } else {
        ADHI_LO;
      }

      if (address & (1 << 7)) {    // Mask address low byte MSB (Bit 7)
        ADLO_HI;
      } else {
        ADLO_LO;
      }
      SCLK_HI;   // SCLK LOW to HIGH
      SCLK_LO;  // SCLK HIGH to LOW, ~125ns pulse width
      
      // Shift address and data bits
      memData <<= 1;
      address <<= 1;
    }

    DAT_LD_HI; // latch data and address on the HC595
    DAT_LD_LO;
    
    // at this point we have data and address loaded into the shift registers
    // now we can request a write to SRAM memory
    
    WE_LO;
    WE_HI;
    
    // re-enable interrupts
    sei();
}

void printhelp(){
Serial.println(F("-> Help for remote control commands (case sensitive)\n"
                         ":EMUOFF   Get sync or stop emulation \n"
                         ":EMUON    Start emulation\n"
                         ":SPICLR   Erase whole SPI EEPROM - Warning - destroys all data in SPI EEPROM\n"
                         ":ini16    Set memory type and address bus for 16Kib ROMs (2716)\n"
                         ":ini32    Set memory type and address bus for 32Kib ROMs (2732)\n"
                         ":ini64    Set memory type and address bus for 64Kib ROMs (2764)\n"
                         ":iniE64   Set memory type and address bus for 64Kib EEPROM (28C64)\n"
                         ":ini128   Set memory type and address bus for 128Kib ROMs (27128)\n"
                         ":ini256   Set memory type and address bus for 256Kib ROMs (27256)\n"
                         ":iniE256  Set memory type and address bus for 256Kib EEPROM (28C256)\n"
                         ":ini512   Set memory type and address bus for 512Kib ROMs (27512)\n"
                         ":iniSPI0  Disable save to SPI EEPROM for next uploads\n"
                         ":iniSPI1  Enable save to SPI EEPROM for next uploads\n"
                         ":iniAuto0 Set auto load off\n"
                         ":iniAuto1 Set auto load on\n"
                         ":help     This help\n"
                        ));
}


// process recieved data frame from PC 

char ch = 0;
String rxFrame = "";

// main program loop

void loop() {
  
if (digitalRead(LD_BTN) == LOW) {           // Yeah, the button is pressed
    delay(500);                             // Wait 0.5s and look again
    if (digitalRead(LD_BTN) == HIGH) {      // Button released after 500ms = short press
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
      
      if (rxFrame.length() == 0){
        Serial.println(F(">"));
        rxFrame = "";
        return;     
      }
      
      //
      if ( rxFrame == ":>" ) {
        Serial.println(F("<"));
        rxFrame = "";
        return;     
      }
      //

      //
      // get sync
      //      
      if ( rxFrame == ":EMUOFF" ) {
        digitalWrite( WE, LOW);       // brings data bus of SRAM to high-z
        digitalWrite( EN_RST, HIGH ); // reset target, disable 541 buffers, enable shift registers
        digitalWrite( LED_G, LOW );  // RUN LED off
        printabout();
        rxFrame = "";
		setBus(rom512K); // new in rc12
        return;     
      }
      //
      // start emulation
      //
      if ( rxFrame == ":EMUON" ) {
          digitalWrite( EN_RST, LOW );  // disable shift registers, address enable buffers.
          digitalWrite( WE, HIGH);      // enable SRAM outputs
          digitalWrite( LED_G, HIGH );  // turn on "RUN" LED
          
          Serial.println(F("-> Emulator Running."));
          spi_address = 0;              // may have to remove that later   
          rxFrame = "";
          return;                  
      }
      
      if ( rxFrame == ":ini16" ) {
        setBus(rom16K);
        if (saveSPI) {EEPROM.update(mem_cfg, rom16K);}
        rxFrame = "";
        return;     
      }
      if ( rxFrame == ":ini32" ) {
        setBus(rom32K);
        if (saveSPI) {EEPROM.update(mem_cfg, rom32K);}
        rxFrame = "";
        return;     
      }
      if ( rxFrame == ":ini64" ) {
        setBus(rom64K);
        if (saveSPI) {EEPROM.update(mem_cfg, rom64K);}
        rxFrame = "";
        return;     
      }
      if ( rxFrame == ":ini128" ) {
        setBus(rom128K);
        if (saveSPI) {EEPROM.update(mem_cfg, rom128K);}
        rxFrame = "";
        return;     
      }
      if ( rxFrame == ":ini256" ) {
        setBus(rom256K);
        if (saveSPI) {EEPROM.update(mem_cfg, rom256K);}
        rxFrame = "";
        return;     
      }
      if ( rxFrame == ":ini512" ) {
        setBus(rom512K);
        if (saveSPI) {EEPROM.update(mem_cfg, rom512K);}
        rxFrame = "";
        return;  
      }
      // Set memory to 28C256 EEPROM
      if ( rxFrame == ":iniE256" ) {
        setBus(romE256K);
        if (saveSPI) {EEPROM.update(mem_cfg, romE256K);}
        rxFrame = "";
        return;     
      }
      // Set memory to 28C64 EEPROM
      if ( rxFrame == ":iniE64" ) {
        setBus(romE64K);
        if (saveSPI) {EEPROM.update(mem_cfg, romE64K);}
        rxFrame = "";
        return;     
      }
      // data will be also written to SPI
      if ( rxFrame == ":iniSPI1" ) {
        saveSPI = true;
        EEPROM.update(save_cfg,1);
        Serial.println(F("-> SPI save ON"));
        spi_address = 0;     
        rxFrame = "";
        return;     
      }
      // data will be also written to SPI
      if ( rxFrame == ":iniSPI0" ) {
        saveSPI = false;
        EEPROM.update(save_cfg,0);
        Serial.println(F("-> SPI save OFF"));  
        rxFrame = "";
        return;        
      }
      //
      // process binary frame, new as of 2.0, process binary frame format <start address><bytecount><payload up to size of buffer> in chunks of 128Bytes
      //
      if ( rxFrame == ":SBN" ) {
        Serial.println(F("->in"));
        unsigned int startaddr = Serial.parseInt();
        unsigned int bytecount = Serial.parseInt();
        
        // now we know start address and how many bytes are coming
        int transfered = Serial.readBytes(byteBuffer,bytecount+1);   // may need bytecount + 1
        // now write the buffer to SRAM
        writeBuffer(startaddr, bytecount);
        if (saveSPI) {writeBufferSPI(startaddr, bytecount);}
        Serial.println(F("ACK"));
        rxFrame = "";
        return;     
      }
      //
      // write directly to SRAM, new as of 2.0rc8
      // 
      if ( rxFrame == ":DIR" ) {
        Serial.println(F(">"));
        unsigned int startaddr = Serial.parseInt();
        unsigned int bytecount = Serial.parseInt();

        // now we know start address and how many bytes are coming
        byte in_data;
        in_data = Serial.read(); // this is to ignore an extra /n in serial buffer
        
        while (bytecount>0){
        // process data from byteBuffer
          if (Serial.available() > 0){
            in_data = Serial.read();
            writeMemoryLocation(startaddr,in_data);
            startaddr++;
            bytecount--;
          }
        }
        Serial.println(F(">"));
        rxFrame = "";
        return;     
      }
      
       // load the SPI EEPROM data to SRAM on boot.
      if ( rxFrame == ":iniAuto1" ) {
        autoLoad = true;
        EEPROM.update(auto_cfg,1);
        Serial.println(F("-> Auto Load ON"));   
        rxFrame = "";
        return;       
      }
      // data will be also written to SPI
      if ( rxFrame == ":iniAuto0" ) {
        autoLoad = false;
        EEPROM.update(auto_cfg,0);
        Serial.println(F("-> Auto Load OFF"));
        rxFrame = "";
        return;     
      }
      // Erase SPI EEPROM ( you can call it from Arduiono serial monitor by typing :SPICLR)
      if ( rxFrame == ":SPICLR" ) {
        clearSPIEEPROM();
        Serial.println(F("->ERASED SPI EEPROM"));
        rxFrame = "";
        return;  
      }
      if ( rxFrame == ":help" or rxFrame =="?" or rxFrame == "help") {
        printhelp();
        rxFrame = "";
        return;
      }

      rxFrame = "";          
    }
    else {
      rxFrame += ch;      
    }
  }
}
