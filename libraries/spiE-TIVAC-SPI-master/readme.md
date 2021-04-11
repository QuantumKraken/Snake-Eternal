spiE.h Enhanced SPI library for TI TIVA C and LM4F boards

I wanted to improve the compatibility of the  lm4f\SPI.h library to include functions of Arduino user contributed SPI.h libraries. I found SPI2.h here: http://forum.stellar...two-spi-modules. This library added multiple instances  but removed the 123 & 1294 compatibility. I have merged SPI2 and the core SPI lib into spiE.h and added several more function variations. 
 
spiE.h features are:
 
1. Array transfer up to 255 bytes. This is 5x faster than single byte transfer.
    LSBFIRST reverses bits and bytes - low byte first and low bit of each byt first
2. SPI2's pinMode() and digitalWrire() functions are replaced with faster macros.
3. Support for __TM4C129XNCZAD__, __TM4C1294NCPDT__, BLIZZARD_RB1 (LM4F) boards
4. Up to 4 instances can be defined: SPI0, SPI1, SPI3, & SPI2. SPI2 is instantiated by default as SPI for Arduino code compatibility.
5. Multiple SlaveSelect pins can be used for multiple devices on any one data line. defaultSS, default+SS2, or SS1 + SS2, etc.
 
6. New overloaded versions of several functions are available:

  void begin();
  void begin( ssPin);
  void end();
  void end( ssPin);
  void setBitOrder( order);
  void setBitOrder( ssPin,  order);
  void setDataMode( mode);
  void setClockDivider( divider);
  void transfer(*dataString, numbytes); 5x faster than byte transfer
  uint8_t transfer( data);    faster
  uint8_t transfer( ssPin,  data);    faster
  uint8_t transfer( ssPin,  data,  transferMode);   slowest

  //Stellarpad-specific functions
  void setModule(uint8_t module);


7.  Four instantiations of SPI are available for handling multiple devices
    with different modules.
    Converted from one SPI instantiation of SPI to 4 modules: 
    SPI0, SPI1, SPI (for backwards compatibility), and SPI3 
    SPI is the default for previous SPI libraries and for 
    TIVA C: SPI BUS 2 -> BOOST_PACK_SPI
