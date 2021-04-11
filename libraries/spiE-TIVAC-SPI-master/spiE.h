//SPI.h
/*
 * Copyright (c) 2014 Paul Dymerski (DrOldies) modifications
 * to SPI2.h by: reaper7 @ http://forum.stellarisiti.com/topic/620-two-spi-modules/
 * the original cor Energis lm4f library:
 * Copyright (c) 2010 by Cristian Maglie <c.maglie@bug.st>
 * SPI Master library for Energia TIVA C  and LM4F.
 
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of either the GNU General Public License version 2
 * or the GNU Lesser General Public License version 2.1, both as
 * published by the Free Software Foundation.
*/

/*  This library COMBINES Maglie's mods as "SPI2.h" from the Stellererista 
forum for the LM4C, with the core LM4F Energia SPI.h library. 

This library is backwards compatible with SPI.h, (by adding SPI2 changes
back into SPI) and adds functions for improved user SPI.h libraries.
SPI2.h calls to pinMode and digitalWrite to faster Energia macros.

1. Array transfer up to 255 bytes. This is 5x faster than single byte transfer.
    LSBFIRST reverses bits and bytes - low byte first and low bit of each byt first
2. SPI2's pinMode() and digitalWrire() functions are replaced with faster macros.
3. Support for __TM4C129XNCZAD__, __TM4C1294NCPDT__, BLIZZARD_RB1 (LM4F) boards
4. Up to 4 instances can be defined: SPI0, SPI1, SPI3, & SPI2. SPI2 is instantiated 
   by default as SPI for Arduino code compatibility.
5. Multiple SlaveSelect pins can be used for multiple devices on any one data line. 
   defaultSS, default+SS2, or SS1 + SS2, etc.

1. New overloaded versions of several functions are available:

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


2. Four instantiations of SPI are available for handling multiple devices
    with different modules.
    Converted from one SPI instantiation of SPI to 4 modules: 
    SPI0, SPI1, SPI (for backwards compatibility), and SPI3 
    SPI is the default for previous SPI libraries and for 
    TIVA C: SPI BUS 2 -> BOOST_PACK_SPI
    SPIClass SPI0(0);   //spi3
    SPIClass SPI1(1);
    SPIClass SPI(2);   // New default as "SPI" - //spi3
    SPIClass SPI3(3);
*/

#ifndef _SPI_H_INCLUDED
#define _SPI_H_INCLUDED

#include <stdio.h>
#include <Energia.h>

#define SPI_CLOCK_DIV2 2
#define SPI_CLOCK_DIV4 4
#define SPI_CLOCK_DIV8 8
#define SPI_CLOCK_DIV16 16
#define SPI_CLOCK_DIV32 32
#define SPI_CLOCK_DIV64 64
#define SPI_CLOCK_DIV128 128

#define SPI_MODE0 0x00
#define SPI_MODE1 0x80
#define SPI_MODE2 0x40
#define SPI_MODE3 0xC0

#define BOOST_PACK_SPI 2

#define MSBFIRST 1
#define LSBFIRST 0

class SPIClass {

private:

	uint8_t SSIModule;
	uint8_t SSIBitOrder;

  //unsigned long slaveSelect; //spi3
  uint32_t   _ssBase_;
  uint8_t _ssMask_;

public:

  SPIClass(void);
  //SPIClass(uint8_t);
  SPIClass(unsigned long);  //spi3
  void begin(); // Default
  void begin(uint8_t ssPin); //spi3
  void end();
  void end(uint8_t ssPin);  //spi3

  void setBitOrder(uint8_t order);
  void setBitOrder(uint8_t ssPin, uint8_t order);

  void setDataMode(uint8_t mode);

  void setClockDivider(uint8_t divider);

  uint8_t transfer(uint8_t data);
  uint8_t transfer(uint8_t ssPin, uint8_t data);  //spi3
  uint8_t transfer(uint8_t ssPin, uint8_t data, uint8_t transferMode);
  void transfer(uint8_t *dataString, uint8_t numBytes);

  //Stellarpad-specific functions
  void setModule(uint8_t module);
};

extern SPIClass SPI;   //default LM4F SPI BUS 2 -> BOOST_PACK_SPI
extern SPIClass SPI0;
extern SPIClass SPI1;
extern SPIClass SPI2;   //spi3
extern SPIClass SPI3;
#endif


