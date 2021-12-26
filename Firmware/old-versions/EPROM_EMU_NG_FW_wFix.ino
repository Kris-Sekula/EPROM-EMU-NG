// parts of this sketch were adapted from https://github.com/bienata/CA80/tree/master/ca80loader_ardu , credits to Natasza
// 
// this sketch controls the EPROM memory Emulator hardware described on mygeekyhobby.com
//
// ver 1.6f ... WE driven high-Z SRAM

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

// utility byte
byte clr;

// spi_address tracking
unsigned int spi_address = 0; // SPI_EEPROM address
unsigned int page_ctn = 0;

//data buffer for SPI write
char buffer [128] = "";

// configuration addresses in internal EEPROM
int mem_cfg = 0;  // EEPROM address where we store last used EPROM information (value 1 = 2716, 2 = 2732, 3 = 2764, 4 = 27128, 5 = 27256, 6 = 27512)
int save_cfg = 1; // EEPROM address where we store settings for "SAVE to SPI EEPROM option" (value 1 = yes, 2 = no)
int auto_cfg = 2; // EEPROM address where we store settings for "Auto load from SPI EEPROM" option (value 1 = yes, 2 = no)

// global config settings
bool saveSPI;
bool autoLoad;
unsigned int lastEPROM;
                       
const char HW_ver[] = "1.6";
const char FW_ver[] = "1.6fix";

//

void setup() {
  pinMode( WE, OUTPUT ); 
  digitalWrite( WE, LOW );     // this will set SRAM D0-D7 as inputs
  
  pinMode( EN_RST, OUTPUT ); 
  digitalWrite( EN_RST, HIGH );  // disconnect external buffers, enable shift registers (A0-A15, D0-D7 driven by Arduino), keep target in RESET state

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
  
  setBus(6); //  default settings to enable all address lines  6 = 27512 memory code

  // SPI EEPROM pins
  pinMode(DATAOUT, OUTPUT);
  pinMode(DATAIN, INPUT);
  pinMode(SPICLOCK,OUTPUT);
  pinMode(SLAVESELECT,OUTPUT);
  digitalWrite(SLAVESELECT,HIGH); //disable device

  // SPCR = 01010000
  //interrupt disabled,spi enabled,msb 1st,master,clk low when idle,
  //sample on leading edge of clk,system clock/4 rate (fastest)
  SPCR = (1<<SPE)|(1<<MSTR);
  clr=SPSR;
  clr=SPDR;
  //
  Serial.begin(115200);

  // load configuration, set defaults if needed

  if ((EEPROM.read(mem_cfg) > 6) || (EEPROM.read(mem_cfg) < 1)) {
    EEPROM.write(mem_cfg,6);  // set default to 27512
    Serial.println("Setting default EPROM Memory to 27512");
  }
  else {
    lastEPROM = EEPROM.read(mem_cfg) ;
    Serial.println(String("Config: Last EPROM used is ") + lastEPROM);
  }
  
  switch (EEPROM.read(save_cfg)) {
    case 0:
      saveSPI = false;
      Serial.println("Option: Save to SPI EEPROM set to disable");
      break;
    case 1:
      saveSPI = true;
      Serial.println("Option: Save to SPI EEPROM set to enable");
      break;
    default:
      EEPROM.write(save_cfg,0);
      Serial.println("Default: disabling save to spi");
      break;
  }
  
  switch (EEPROM.read(auto_cfg)) {
    case 0:
      autoLoad = false;
      Serial.println("Option: Auto load from SPI EEPROM set to disable");
      break;
    case 1:
      autoLoad = true;
      Serial.println("Option: Auto load from SPI EEPROM set to enable");
      break;
    default:
      EEPROM.write(auto_cfg,0);
      Serial.println("Default: Disabling Auto load from SPI EEPROM ");
      break;
  }

  // load data from SPI EEPROM if configured

  if (autoLoad == true ) {
    load_SPIEEPROM(lastEPROM);
  }
  
  //
  Serial.println("");
  Serial.println("Emulator Started, waiting for computer contol");
  printabout();
}



void load_SPIEEPROM (int eprom) {
  
  setBus(6); // allows access to enire 64kb SRAM Memory - equivlet of emulating 27512 eprom
  digitalWrite( WE, LOW );      // this will set SRAM D0-D7 as inputs
  digitalWrite( EN_RST, HIGH ); // disconnect external buffers, enable shift registers (A0-A15, D0-D7 driven by Arduino), keep target in RESET state
  
  digitalWrite( LED_G, LOW );
  
  unsigned int max_size;
  switch (eprom) {
    case 1:
      Serial.println( "-> Loading 2716 EPROM from SPI" );
      max_size = 16; // pages of 128 bytes
      break;
    case 2:
      Serial.println( "->  Loading 2732 EPROM from SPI" );   
      max_size = 32; // pages of 128 bytes
      break;
    case 3:
      Serial.println( "->  Loading 2764 EPROM from SPI" );
      max_size = 64; // pages of 128 bytes
      break;
    case 4:
      Serial.println( "->  Loading 27128 EPROM from SPI" );
      max_size = 128; // pages of 128 bytes
      break;
    case 5:
      Serial.println( "-> Loading 27256 EPROM from SPI" );
      max_size = 256; // pages of 128 bytes
      break;
    case 6:
      Serial.println( "-> Loading 27512 EPROM from SPI" );
      max_size = 512; // pages of 128 bytes
      break;
    default:
      Serial.println( "-> Default: Loading 27512 EPROM from SPI" );
      max_size = 512; // pages of 128 bytes
      break;
  }

  // now load this many pages from SPI EEPROM
  
  //int data;
  byte data;
  int clmn = 0;
  
  for ( int page = 0; page <= max_size; page++) {
    //Serial.println(String("Reading SPI Page: ")+page);
    //Serial.println(String("Address: ")+spi_address);

    digitalWrite(LED_G, !digitalRead(LED_G)); // flash LED while loading
    
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

  digitalWrite( EN_RST, LOW );  // enable external buffers
  digitalWrite( WE, HIGH );     // this will set SRAM D0-D7 as outputs
  
  digitalWrite( LED_G, HIGH );
  
  Serial.println("Done loading from SPI EEPROM to SRAM" );  
  
}

//
// saves byte to 128 page buffer, saves page to EEPROM after each 128 bytes
//
// can't selectivly write data, only blocks of 128 bytes starting from address 0x0000h and up to 0xFFFFh

void saveToSPI (char data) {
  // write to SPI buffer
  
  buffer[page_ctn] = data;
  
  page_ctn++;
  
  if(page_ctn == 128) {
    
      // send write enable command to unlock writting to EEPROM
      digitalWrite(SLAVESELECT,LOW);
      spi_transfer(WREN);
      digitalWrite(SLAVESELECT,HIGH);

      // write command
      digitalWrite(SLAVESELECT,LOW);
      spi_transfer(WRITE);            //write instruction
      
      // set start address
      spi_transfer((char)(spi_address>>8));
      spi_transfer((char)(spi_address));
      
      // send all 128 bytes
      
      for (int k=0;k<128;k++) {
        spi_transfer(buffer[k]);
      }
      digitalWrite(SLAVESELECT,HIGH);
      
      // wait for write to finish, for 25LC512 Twc = 5ms 
       delay(10);
    //
    page_ctn = 0;
    spi_address = spi_address + 128;
  }
  
  return;
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

void printabout() {    
          Serial.print("-> HW: ");
          Serial.print(HW_ver);
          Serial.print(" @ 115200, FW: ");
          Serial.print(FW_ver);
          Serial.print(", SPI: ");
          Serial.print(saveSPI);
          Serial.print(", Auto: ");
          Serial.print(autoLoad);
          Serial.print(", Last EPROM: ");
          Serial.print(lastEPROM);        
          Serial.println(", mygeekyhobby.com 2020 :)");
}

//  Adjusts address lanes A11 - A15, based on imput EPROM type is 2716 = 1 ... 27512 = 6

void setBus (int eprom){
  switch (eprom) {
    case 1:
      Serial.println( "-> Bus set to 2716 EPROM Memory" );
      digitalWrite( EN_A11, LOW );
      digitalWrite( EN_A12, LOW );
      digitalWrite( EN_A13, LOW );
      digitalWrite( EN_A14, LOW );
      digitalWrite( EN_A15, LOW );
      break;
    case 2:
      Serial.println( "-> Bus set to 2732 EPROM Memory" );
      digitalWrite( EN_A11, HIGH );
      digitalWrite( EN_A12, LOW );
      digitalWrite( EN_A13, LOW );
      digitalWrite( EN_A14, LOW );
      digitalWrite( EN_A15, LOW );     
      break;
    case 3:
      Serial.println( "-> Bus set to 2764 EPROM Memory" );
      digitalWrite( EN_A11, HIGH );
      digitalWrite( EN_A12, HIGH );
      digitalWrite( EN_A13, LOW );
      digitalWrite( EN_A14, LOW );
      digitalWrite( EN_A15, LOW );
      break;
    case 4:
      Serial.println( "-> Bus set to 27128 EPROM Memory" );
      digitalWrite( EN_A11, HIGH );
      digitalWrite( EN_A12, HIGH );
      digitalWrite( EN_A13, HIGH );
      digitalWrite( EN_A14, LOW );
      digitalWrite( EN_A15, LOW );
      break;
    case 5:
      Serial.println( "-> Bus set to 27256 EPROM Memory" );
      digitalWrite( EN_A11, HIGH );
      digitalWrite( EN_A12, HIGH );
      digitalWrite( EN_A13, HIGH );
      digitalWrite( EN_A14, HIGH );
      digitalWrite( EN_A15, LOW );
      break;
    case 6:
      Serial.println( "-> Bus set to 27512 EPROM Memory" );
      digitalWrite( EN_A11, HIGH );
      digitalWrite( EN_A12, HIGH );
      digitalWrite( EN_A13, HIGH );
      digitalWrite( EN_A14, HIGH );
      digitalWrite( EN_A15, HIGH );
      break;
    default:
      Serial.println( "-> Bus set to 27512 EPROM Memory" );
      digitalWrite( EN_A11, HIGH );
      digitalWrite( EN_A12, HIGH );
      digitalWrite( EN_A13, HIGH );
      digitalWrite( EN_A14, HIGH );
      digitalWrite( EN_A15, HIGH );
      break;
  }
  return;
}

// Write single byte of data to SRAM

void writeMemoryLocation (unsigned short address, unsigned char data ) {
    unsigned char addressHi =  address >> 8 ;
    unsigned char addressLo = address & 0xFF ;    
    unsigned char memData =  data ;
    
    // send data
    
    for (int i = 0; i < 8 ; i++ ) { 
      digitalWrite( DATA, 0x80 & (memData << i) );
      digitalWrite( ADLO, 0x80 & (addressLo << i));
      digitalWrite( ADHI, 0x80 & (addressHi << i) ); 
          
      digitalWrite( SCLK, HIGH );
      digitalWrite( SCLK, LOW );             
    }
    
    digitalWrite( DAT_LD, HIGH ); // latch data and address out
    digitalWrite( DAT_LD, LOW );
    
    // at this point we have data and address loaded into the shift registers
    // now we can request a write to SRAM memory

    digitalWrite( WE, LOW );   // raising edge of WE writes data to SRAM
    digitalWrite( WE, HIGH );    
}


// process recieved data frame from PC 

char ch = 0;
String rxFrame = "";

void processFrame(const char *pS ) {
  char szTemp[5] = "";
  int  recLen = 0;
  unsigned char data = 0;
  unsigned short address = 0;

  strncpy(szTemp, pS+1, 2 );  szTemp[ 2 ] = 0x00;
  recLen = (int)strtol( szTemp, 0, 16 );
  
  strncpy(szTemp, pS+3, 4 );  szTemp[ 4 ] = 0x00;
  address = (unsigned short)strtol( szTemp, 0, 16 );

  for (int i = 0; i < recLen; i++ ) {
    strncpy(szTemp, pS+9 + i*2, 2 );  szTemp[ 2 ] = 0x00;
    data = (unsigned char)strtol( szTemp, 0, 16 );
    writeMemoryLocation( address + i , data );
    // optionally write to SPI buffer
    if (saveSPI) {saveToSPI(data);}    
  }
}

int isHexFrame ( const char *f ) {
  return (f[0] == ':' ) && (f[7] == '0') && (f[8] == '0');  // ":" iHEX, record type 00 = Data
}

// main program loop

void loop() {
  
  if (digitalRead(LD_BTN) == LOW) {
    delay(500);
    if (digitalRead(LD_BTN) == HIGH) {
    // short press will load from SPI_EEPROM
    Serial.println("Load from SPI EEPROM triggered by pushbutton");
    load_SPIEEPROM(lastEPROM);
    delay(5000);
    } else {
    // long press disable AutoLoad, otherwise you can't upload due to auto-reset when arduino port opens on computer
    EEPROM.write(auto_cfg,0);
    autoLoad = false;
    Serial.println("Auto load disabled by pushbutton");
    delay(5000);
    }
  }
  
  if (Serial.available() > 0) {
    char ch = Serial.read();            
    if ( ch == 0x0D /*CR*/) {
      return;
    }
    if ( ch == 0x0A /*LF*/) {
      // start emulation      
      if ( rxFrame == ":00000001FF" ) {
          
          digitalWrite( EN_RST, LOW );  // enable external buffers
          digitalWrite( WE, HIGH );     // this will set SRAM D0-D7 as outputs

          digitalWrite( LED_G, HIGH );  
          Serial.println("-> Emulator Running.");
          spi_address = 0;                    
      }
      // get sync      
      if ( rxFrame == ":dml" ) {
        // start communication from computer 
        // 
        digitalWrite( WE, LOW );      // this will set SRAM D0-D7 as inputs
        digitalWrite( EN_RST, HIGH ); // disconnect external buffers, enable shift registers (A0-A15, D0-D7 driven by Arduino), keep target in RESET state
        printabout();
      }
      if ( rxFrame == ":ini16" ) {
        setBus(1);
        EEPROM.update(mem_cfg,1);  
      }
      if ( rxFrame == ":ini32" ) {
        setBus(2);
        EEPROM.update(mem_cfg,2);
      }
      if ( rxFrame == ":ini64" ) {
        setBus(3);
        EEPROM.update(mem_cfg,3);      
      }
      if ( rxFrame == ":ini128" ) {
        setBus(4);
        EEPROM.update(mem_cfg,4);  
      }
      if ( rxFrame == ":ini256" ) {
        setBus(5);
        EEPROM.update(mem_cfg,5);         
      }
      if ( rxFrame == ":ini512" ) {
        setBus(6);
        EEPROM.update(mem_cfg,6);     
      }
      // data will be also written to SPI
      if ( rxFrame == ":iniSPI1" ) {
        saveSPI = true;
        EEPROM.update(save_cfg,1);
        Serial.println("-> SPI save ON");
        spi_address = 0;     
      }
      // data will be also written to SPI
      if ( rxFrame == ":iniSPI0" ) {
        saveSPI = false;
        EEPROM.update(save_cfg,0);
        Serial.println("-> SPI save OFF");     
      }
      //
       // load the SPI EEPROM data to SRAM on boot.
      if ( rxFrame == ":iniAuto1" ) {
        autoLoad = true;
        EEPROM.update(auto_cfg,1);
        Serial.println("-> Auto Load ON");     
      }
      // data will be also written to SPI
      if ( rxFrame == ":iniAuto0" ) {
        autoLoad = false;
        EEPROM.update(auto_cfg,0);
        Serial.println("-> Auto Load OFF");     
      }
 
      if (isHexFrame( rxFrame.c_str() ) ) {
        processFrame( rxFrame.c_str() );
        Serial.println( "> " + rxFrame.substring(3, 7) );
      }
      rxFrame = "";          
    }
    else {
      rxFrame += ch;      
    }
  }
}
