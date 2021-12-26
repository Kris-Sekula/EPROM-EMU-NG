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
// 2.0rc2 Dec-2020 
//		- eliminate pull up on button by using the CPU built in pull up resistors
//		- change the "long-press" LOAD button function from "disable auto-load" to more useful "reset target" function
// 2.0rc3 Aug-2021 
//    - fix for black nano with atmega328 in vqfn package 

#include <SPI.h>
#include <EEPROM.h>

#define DATA 3  // data pin for shift register handling D0-D7
#define ADLO 4  // data pin for shift register handling A0-A7
#define ADHI 5  // data pin for shift register handling A8-A15
#define SCLK 7  // clock ping for all shift registers

#define DAT_LD 6  // latch pin for all shift registers 


#define EN_RST 2  //  internal / external bus control pin
#define WE 8      // SRAM Write Enable Pin

#define LD_BTN 9  // load data from SPI button
#define LED_G A0  // RED LED



// Gating signals - ignores selected A11 - A15 address pins besed on EPROM Type
#define EN_A11 A1  // A11 data pin gating signal 
#define EN_A12 A2  // A12 data pin gating signal 
#define EN_A13 A3  // A13 data pin gating signal 
#define EN_A14 A4  // A14 data pin gating signal 
#define EN_A15 A5  // A15 data pin gating signal 

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
int mem_cfg = 0;  // EEPROM address where we store last used EPROM information (value 1 = 2716, 2 = 2732, 3 = 2764, 4 = 27128, 5 = 27256, 6 = 27512)
int save_cfg = 1; // EEPROM address where we store settings for "SAVE to SPI EEPROM option" (value 1 = yes, 2 = no)
int auto_cfg = 2; // EEPROM address where we store settings for "Auto load from SPI EEPROM" option (value 1 = yes, 2 = no)
int ver_cfg = 10;  // EEPROM start addres where we store hardware version (up to 10 characters)

// global config settings
bool saveSPI;
bool autoLoad;
unsigned int lastEPROM;
                       
const char FW_ver[] = "2.0rc3";
char HW_ver[10];

const char* memTable[7] = {"27xx","2716","2732","2764","27128","27256","27512"}; // lookup table for memory type

uint8_t sclkMask;
uint8_t mosiDATAMask;
uint8_t mosiADLOMask;
uint8_t mosiADHIMask;

void setup() {

  sclkMask = digitalPinToBitMask(SCLK);       // used later for shift registers
  mosiDATAMask = digitalPinToBitMask(DATA);
  mosiADLOMask = digitalPinToBitMask(ADLO);
  mosiADHIMask = digitalPinToBitMask(ADHI);
  
  pinMode( EN_RST, OUTPUT ); digitalWrite( EN_RST, LOW );  // disable outputs to start with AD0-15 from outside

  pinMode( DATA, OUTPUT ); digitalWrite( DATA, LOW );
  pinMode( ADLO, OUTPUT ); digitalWrite( ADLO, LOW );
  pinMode( ADHI, OUTPUT ); digitalWrite( ADHI, LOW );

  pinMode( DAT_LD, OUTPUT ); digitalWrite( DAT_LD, LOW );
  pinMode( SCLK, OUTPUT ); digitalWrite( SCLK, LOW );

  pinMode( WE, OUTPUT ); digitalWrite( WE, HIGH );
  pinMode( LED_G, OUTPUT ); digitalWrite( LED_G, LOW );

  // address lines gate signals as outputs
  pinMode( EN_A11, OUTPUT );
  pinMode( EN_A12, OUTPUT );
  pinMode( EN_A13, OUTPUT ); 
  pinMode( EN_A14, OUTPUT );
  pinMode( EN_A15, OUTPUT );

  //
  pinMode(LD_BTN,INPUT_PULLUP); 
  
  setBus(6); //  default settings to enable all address lines  6 = 27512 memory code

  // SPI EEPROM pins
  pinMode(DATAOUT, OUTPUT);
  pinMode(DATAIN, INPUT);
  pinMode(SPICLOCK,OUTPUT);
  pinMode(SLAVESELECT,OUTPUT); digitalWrite(SLAVESELECT,HIGH); //disable device

  // SPCR = 01010000
  //interrupt disabled,spi enabled,msb 1st,master,clk low when idle,
  //sample on leading edge of clk,system clock/4 rate (fastest)
  SPCR = (1<<SPE)|(1<<MSTR);
  clr=SPSR;
  clr=SPDR;
  //
  Serial.begin(115200);

  //clearSPIEEPROM(); // one time only erase EEPROM

  // load configuration, set defaults if needed
  
  //
  // get "last used EPROM" config from SPI_EEPROM
  //
  if ((EEPROM.read(mem_cfg) > 6) || (EEPROM.read(mem_cfg) < 1)) {
    EEPROM.write(mem_cfg,6);  // set default to 27512
    Serial.println(F("Setting default EPROM Memory to 27512"));
  }
  else {
    lastEPROM = EEPROM.read(mem_cfg) ;
    Serial.println(String(F("Config: Last EPROM used is ")) + memTable[lastEPROM]);
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
    EEPROM.put(ver_cfg, "v2.2c"); //Set this to whatever you have on the PCB, if you did the HW mod as per Errata, call it v1.6f or similar   <--------- README                                  
    EEPROM.get(ver_cfg, HW_ver);
  }
  //
  // if you missed setting up the hw version before, you can temporary replace the "if((HW_ver[0] != 'v')" with "if(1)"
  //

  Serial.println("");
  Serial.println(F("Emulator Started, waiting for computer contol"));
  printabout();
  //

}

//
// restore SRAM from SPI EEPROM
//

void load_SPIEEPROM (int eprom) {
  
  setBus(6); // allows access to enire 64kb SRAM Memory - equivlet of emulating 27512 eprom
  digitalWrite( WE, LOW);       // brings data bus of SRAM to high-z
  digitalWrite( EN_RST, HIGH ); // reset target, disable 541 buffers, enable shift registers
  digitalWrite( LED_G, LOW );
  
  unsigned int max_size;
  switch (eprom) {
    case 1:
      Serial.println(F( "-> Loading 2716 EPROM from SPI"));
      max_size = 16; // pages of 128 bytes
      break;
    case 2:
      Serial.println(F( "->  Loading 2732 EPROM from SPI"));   
      max_size = 32; // pages of 128 bytes
      break;
    case 3:
      Serial.println(F( "->  Loading 2764 EPROM from SPI"));
      max_size = 64; // pages of 128 bytes
      break;
    case 4:
      Serial.println(F( "->  Loading 27128 EPROM from SPI"));
      max_size = 128; // pages of 128 bytes
      break;
    case 5:
      Serial.println(F( "-> Loading 27256 EPROM from SPI"));
      max_size = 256; // pages of 128 bytes
      break;
    case 6:
      Serial.println(F( "-> Loading 27512 EPROM from SPI"));
      max_size = 512; // pages of 128 bytes
      break;
    default:
      Serial.println(F( "-> Default: Loading 27512 EPROM from SPI"));
      max_size = 512; // pages of 128 bytes
      break;
  }

  // now load this many pages from SPI EEPROM
  
  //int data;
  byte data;
  int clmn = 0;
  
  for ( int page = 0; page <= max_size; page++) {

    if (spi_address & 00010000) {
      digitalWrite(LED_G, !digitalRead(LED_G)); // flash LED while loading
    }
    
    digitalWrite(SLAVESELECT,LOW); // select SPI EEPROM
    spi_transfer(READ); //transmit read opcode
    spi_transfer((char)(spi_address>>8));   //send MSByte address first
    spi_transfer((char)(spi_address));      //send LSByte address
    
    for (int i=0; i<128; i++) {
      data = spi_transfer(0xFF); //get data byte by trasfering dummy 0xff data to spi eeprom
      writeMemoryLocation((short)(spi_address),(char)(data));
      spi_address++;

    }
    digitalWrite(SLAVESELECT,HIGH); //release chip, signal end transfer
    Serial.print(".");
    clmn ++;
    if (clmn > 64) {
      Serial.println("");
      clmn = 0;
    }
  }
  Serial.println("");

  setBus(eprom); // now that SRAM is loaded with data, we can enable correct address lines

  digitalWrite( EN_RST, LOW );  // disable shift registers, address enable buffers.
  digitalWrite( WE, HIGH);      // enable SRAM outputs
  digitalWrite( LED_G, HIGH );  // turn on "RUN" LED
  
  Serial.println(F("Done loading from SPI EEPROM to SRAM"));  
}
//


//
// procedure to setup address for spi transaction
//

void clearSPIEEPROM (){
   // send write enable command to unlock writting to EEPROM

      digitalWrite(SLAVESELECT,LOW);
      spi_transfer(WREN);
      digitalWrite(SLAVESELECT,HIGH);

      // chip erase command
      digitalWrite(SLAVESELECT,LOW);
      spi_transfer(CE);            //chip erase instruction
      digitalWrite(SLAVESELECT,HIGH);
      delay(10);
}

//
// procedure to setup address for spi transaction
//

void setAddressSPI (unsigned short address){
   // send write enable command to unlock writting to EEPROM

      digitalWrite(SLAVESELECT,LOW);
      spi_transfer(WREN);
      digitalWrite(SLAVESELECT,HIGH);

      // write command
      digitalWrite(SLAVESELECT,LOW);
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
      digitalWrite(SLAVESELECT,HIGH);
      // wait for write to finish, for 25LC512 Twc = 5ms 
      delay(6);
      setAddressSPI(address); // set address
      m=0;
    }
    spi_transfer(byteBuffer[k+1]);
    address++;
    m++;
  }
  digitalWrite(SLAVESELECT,HIGH);
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
          Serial.print(F(" @ 115200, FW: ")); Serial.print(FW_ver);
          Serial.print(F(", SPI: ")); Serial.print(saveSPI);
          Serial.print(F(", Auto: ")); Serial.print(autoLoad);
          Serial.print(F(", Last EPROM: ")); Serial.print(memTable[lastEPROM]);        
          Serial.println(F(", mygeekyhobby.com 2020 :)"));
}

//  Adjusts address lanes A11 - A15, based on imput EPROM type is 2716 = 1 ... 27512 = 6

void setBus (int eprom){
  switch (eprom) {
    case 1:
      Serial.println(F( "-> Bus set to 2716 EPROM Memory"));
      digitalWrite( EN_A11, LOW );
      digitalWrite( EN_A12, LOW );
      digitalWrite( EN_A13, LOW );
      digitalWrite( EN_A14, LOW );
      digitalWrite( EN_A15, LOW );
      break;
    case 2:
      Serial.println(F( "-> Bus set to 2732 EPROM Memory"));
      digitalWrite( EN_A11, HIGH );
      digitalWrite( EN_A12, LOW );
      digitalWrite( EN_A13, LOW );
      digitalWrite( EN_A14, LOW );
      digitalWrite( EN_A15, LOW );     
      break;
    case 3:
      Serial.println(F( "-> Bus set to 2764 EPROM Memory"));
      digitalWrite( EN_A11, HIGH );
      digitalWrite( EN_A12, HIGH );
      digitalWrite( EN_A13, LOW );
      digitalWrite( EN_A14, LOW );
      digitalWrite( EN_A15, LOW );
      break;
    case 4:
      Serial.println(F( "-> Bus set to 27128 EPROM Memory"));
      digitalWrite( EN_A11, HIGH );
      digitalWrite( EN_A12, HIGH );
      digitalWrite( EN_A13, HIGH );
      digitalWrite( EN_A14, LOW );
      digitalWrite( EN_A15, LOW );
      break;
    case 5:
      Serial.println(F( "-> Bus set to 27256 EPROM Memory"));
      digitalWrite( EN_A11, HIGH );
      digitalWrite( EN_A12, HIGH );
      digitalWrite( EN_A13, HIGH );
      digitalWrite( EN_A14, HIGH );
      digitalWrite( EN_A15, LOW );
      break;
    case 6:
      Serial.println(F( "-> Bus set to 27512 EPROM Memory"));
      digitalWrite( EN_A11, HIGH );
      digitalWrite( EN_A12, HIGH );
      digitalWrite( EN_A13, HIGH );
      digitalWrite( EN_A14, HIGH );
      digitalWrite( EN_A15, HIGH );
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

// Write single byte of data to SRAM

void writeMemoryLocation (unsigned short address, unsigned char data ) {
  unsigned char addressHi =  address >> 8 ;
  unsigned char addressLo = address & 0xFF ;    
  unsigned char memData =  data ;
    
  // send data
  
  volatile uint8_t *sclkPort = portOutputRegister(digitalPinToPort(SCLK));
  //
  volatile uint8_t *mosiDATAPort = portOutputRegister(digitalPinToPort(DATA));
  volatile uint8_t *mosiADLOPort = portOutputRegister(digitalPinToPort(ADLO));
  volatile uint8_t *mosiADHIPort = portOutputRegister(digitalPinToPort(ADHI));

  uint8_t oldSREG = SREG;
  
  for (uint8_t i = 0; i < 8; ++i)
  {
    if (memData & 0x80)
    {
      *mosiDATAPort |= mosiDATAMask;
    }
    else
    {
      *mosiDATAPort &= ~mosiDATAMask;
    }
  
  if (addressHi & 0x80)
    {
      *mosiADHIPort |= mosiADHIMask;
    }
    else
    {
      *mosiADHIPort &= ~mosiADHIMask;
    }
  
  if (addressLo & 0x80)
    {
      *mosiADLOPort |= mosiADLOMask;
    }
    else
    {
      *mosiADLOPort &= ~mosiADLOMask;
    }
  
    *sclkPort |= sclkMask;
    memData <<= 1;
  addressHi <<= 1;
  addressLo <<= 1;
  
    *sclkPort &= ~sclkMask;
  }
  SREG = oldSREG;
    
    digitalWrite( DAT_LD, HIGH ); // latch data and address out
    digitalWrite( DAT_LD, LOW );
    
    // at this point we have data and address loaded into the shift registers
    // now we can request a write to SRAM memory

    digitalWrite( WE, LOW );
    digitalWrite( WE, HIGH );
}


// process recieved data frame from PC 

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
      // get sync
      //      
      if ( rxFrame == ":dml" ) {
        digitalWrite( WE, LOW);       // brings data bus of SRAM to high-z
        digitalWrite( EN_RST, HIGH ); // reset target, disable 541 buffers, enable shift registers
        digitalWrite( LED_G, LOW );  // RUN LED off
        printabout();
      }
      //
      // start emulation
      //
      if ( rxFrame == ":EMUON" ) {
          digitalWrite( EN_RST, LOW );  // disable shift registers, address enable buffers.
          digitalWrite( WE, HIGH);      // enable SRAM outputs
          digitalWrite( LED_G, HIGH );  // turn on "RUN" LED
          Serial.println(F("-> Emulator Running!"));
          spi_address = 0;              // may have to remove that later
      }
      
      if ( rxFrame == ":ini16" ) {
        setBus(1);
        if (saveSPI) {EEPROM.update(mem_cfg,1);}
      }
      if ( rxFrame == ":ini32" ) {
        setBus(2);
        if (saveSPI) {EEPROM.update(mem_cfg,2);}
      }
      if ( rxFrame == ":ini64" ) {
        setBus(3);
        if (saveSPI) {EEPROM.update(mem_cfg,3);}
      }
      if ( rxFrame == ":ini128" ) {
        setBus(4);
        if (saveSPI) {EEPROM.update(mem_cfg,4);}
      }
      if ( rxFrame == ":ini256" ) {
        setBus(5);
        if (saveSPI) {EEPROM.update(mem_cfg,5);}
      }
      if ( rxFrame == ":ini512" ) {
        setBus(6);
        if (saveSPI) {EEPROM.update(mem_cfg,6);}
      }
      // data will be also written to SPI
      if ( rxFrame == ":iniSPI1" ) {
        saveSPI = true;
        EEPROM.update(save_cfg,1);
        Serial.println(F("-> SPI save ON"));
        spi_address = 0;     
      }
      // data will be also written to SPI
      if ( rxFrame == ":iniSPI0" ) {
        saveSPI = false;
        EEPROM.update(save_cfg,0);
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
        int transfered = Serial.readBytes(byteBuffer,bytecount+1);   // may need bytecount + 1
        // now write the buffer to SRAM
        writeBuffer(startaddr, bytecount);
        if (saveSPI) {writeBufferSPI(startaddr, bytecount);}
        Serial.println(F("ACK"));
      }
       // load the SPI EEPROM data to SRAM on boot.
      if ( rxFrame == ":iniAuto1" ) {
        autoLoad = true;
        EEPROM.update(auto_cfg,1);
        Serial.println(F("-> Auto Load ON"));     
      }
      // data will be also written to SPI
      if ( rxFrame == ":iniAuto0" ) {
        autoLoad = false;
        EEPROM.update(auto_cfg,0);
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
}
