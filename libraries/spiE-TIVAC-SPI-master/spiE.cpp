/* SPI.cpp
 * Copyright (c) 2010 by Cristian Maglie <c.maglie@bug.st>
 * SPI Master library for arduino.
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of either the GNU General Public License version 2
 * or the GNU Lesser General Public License version 2.1, both as
 * published by the Free Software Foundation.

 * Enhanced functions for lm4f series boards

*/

#include "wiring_private.h"
#include "inc/hw_memmap.h"
#include "inc/hw_ssi.h"
#include "inc/hw_types.h"
#include "driverlib/ssi.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/pin_map.h"
#include "part.h"

#include "spiE.h"

#define SSIBASE g_ulSSIBase[SSIModule]
#define SSPIN g_u8SlaveSelPins[SSIModule] 
#define NOT_ACTIVE 0xA

/* variants
   stellarpad - LM4F120H5QR, TM4C123GH6PM, aka TARGET_IS_BLIZZARD_RB1
    i base  port
    0 SSI0 PA
    1 SSI1 PF
    2 SSI2 PB
    3 SSI3 PD

   dktm4c129 - TM4C129XNCZAD 
    i base  port
    0 SSI0  PA
    1 SSI1  PB/PE
    2 SSI2  PD
    3 SSI3  PF 
    4 SSI2  PG 
    5 SSI3  PQ

   ektm4c12944XL - TM4C1294NCPDT (TIVA C)
    i base  port
    0 SSI0  PA
    1 SSI1  PB/PE
    2 SSI2  PD
    3 SSI3  PF 
    4 SSI3  PQ
*/

static const unsigned long g_ulSSIBase[] = {
#ifdef TARGET_IS_BLIZZARD_RB1
    SSI0_BASE, SSI1_BASE, SSI2_BASE, SSI3_BASE
#else
#ifdef __TM4C129XNCZAD__
    SSI0_BASE, SSI1_BASE, SSI2_BASE, SSI3_BASE, SSI2_BASE, SSI3_BASE
#endif
#ifdef __TM4C1294NCPDT__
    SSI0_BASE, SSI1_BASE, SSI2_BASE, SSI3_BASE, SSI3_BASE
#endif
#endif
};

//*****************************************************************************
//
// The list of SSI peripherals.
//
//*****************************************************************************
static const unsigned long g_ulSSIPeriph[] = {
#ifdef TARGET_IS_BLIZZARD_RB1
    SYSCTL_PERIPH_SSI0, SYSCTL_PERIPH_SSI1, SYSCTL_PERIPH_SSI2, SYSCTL_PERIPH_SSI3
#else
#ifdef __TM4C129XNCZAD__
    SYSCTL_PERIPH_SSI0, SYSCTL_PERIPH_SSI1, SYSCTL_PERIPH_SSI2, SYSCTL_PERIPH_SSI3, SYSCTL_PERIPH_SSI2, SYSCTL_PERIPH_SSI3
#endif
#ifdef __TM4C1294NCPDT__
    SYSCTL_PERIPH_SSI0, SYSCTL_PERIPH_SSI1, SYSCTL_PERIPH_SSI2, SYSCTL_PERIPH_SSI3, SYSCTL_PERIPH_SSI3
#endif
#endif

};

//*****************************************************************************
//
// The list of SSI gpio configurations.
//
//*****************************************************************************
static const unsigned long g_ulSSIConfig[][4] = {
#ifdef TARGET_IS_BLIZZARD_RB1
    {GPIO_PA2_SSI0CLK, GPIO_PA3_SSI0FSS, GPIO_PA4_SSI0RX, GPIO_PA5_SSI0TX},
    {GPIO_PF2_SSI1CLK, GPIO_PF3_SSI1FSS, GPIO_PF0_SSI1RX, GPIO_PF1_SSI1TX},
    {GPIO_PB4_SSI2CLK, GPIO_PB5_SSI2FSS, GPIO_PB6_SSI2RX, GPIO_PB7_SSI2TX},
    {GPIO_PD0_SSI3CLK, GPIO_PD1_SSI3FSS, GPIO_PD2_SSI3RX, GPIO_PD3_SSI3TX}
#else
#ifdef __TM4C129XNCZAD__
// from Table 20-1. SSI Signals (212BGA)
    {GPIO_PA2_SSI0CLK, GPIO_PA3_SSI0FSS, GPIO_PA4_SSI0XDAT0, GPIO_PA5_SSI0XDAT1},
    {GPIO_PB5_SSI1CLK, GPIO_PB4_SSI1FSS, GPIO_PE4_SSI1XDAT0, GPIO_PE5_SSI1XDAT1},
    {GPIO_PD3_SSI2CLK, GPIO_PD2_SSI2FSS, GPIO_PD1_SSI2XDAT0, GPIO_PD0_SSI2XDAT1},
    {GPIO_PF3_SSI3CLK, GPIO_PF2_SSI3FSS, GPIO_PF1_SSI3XDAT0, GPIO_PF0_SSI3XDAT1},
    {GPIO_PG7_SSI2CLK, GPIO_PG6_SSI2FSS, GPIO_PG5_SSI2XDAT0, GPIO_PG4_SSI2XDAT1},
    {GPIO_PQ0_SSI3CLK, GPIO_PQ1_SSI3FSS, GPIO_PQ2_SSI3XDAT0, GPIO_PQ3_SSI3XDAT1}
#endif
#ifdef __TM4C1294NCPDT__
// from Table 17-1. SSI Signals (128TQFP)
    {GPIO_PA2_SSI0CLK, GPIO_PA3_SSI0FSS, GPIO_PA4_SSI0XDAT0, GPIO_PA5_SSI0XDAT1},
    {GPIO_PB5_SSI1CLK, GPIO_PB4_SSI1FSS, GPIO_PE4_SSI1XDAT0, GPIO_PE5_SSI1XDAT1},
    {GPIO_PD3_SSI2CLK, GPIO_PD2_SSI2FSS, GPIO_PD1_SSI2XDAT0, GPIO_PD0_SSI2XDAT1},
    {GPIO_PF3_SSI3CLK, GPIO_PF2_SSI3FSS, GPIO_PF1_SSI3XDAT0, GPIO_PF0_SSI3XDAT1},
    {GPIO_PQ0_SSI3CLK, GPIO_PQ1_SSI3FSS, GPIO_PQ2_SSI3XDAT0, GPIO_PQ3_SSI3XDAT1}
#endif
#endif
,};

//*****************************************************************************
//
// The list of SSI gpio port bases.
//
//*****************************************************************************
static const unsigned long g_ulSSIPort[] = {
#ifdef TARGET_IS_BLIZZARD_RB1
    GPIO_PORTA_BASE, GPIO_PORTF_BASE, GPIO_PORTB_BASE, GPIO_PORTD_BASE
#else
#ifdef __TM4C129XNCZAD__
    GPIO_PORTA_BASE, GPIO_PORTB_BASE, GPIO_PORTD_BASE, GPIO_PORTF_BASE, GPIO_PORTG_BASE, GPIO_PORTQ_BASE
#endif
#ifdef __TM4C1294NCPDT__
    GPIO_PORTA_BASE, GPIO_PORTB_BASE, GPIO_PORTD_BASE, GPIO_PORTF_BASE, GPIO_PORTQ_BASE
#endif
#endif
};

//*****************************************************************************
//
// The list of SSI gpio configurations.
//
//*****************************************************************************
static const unsigned long g_ulSSIPins[] = {
#ifdef TARGET_IS_BLIZZARD_RB1
	GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5,
	GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3,
	GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7,
	GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3
#else
#ifdef __TM4C129XNCZAD__
	GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5,
	GPIO_PIN_5 | GPIO_PIN_4 | GPIO_PIN_4 | GPIO_PIN_5,
	GPIO_PIN_3 | GPIO_PIN_2 | GPIO_PIN_1 | GPIO_PIN_0,
	GPIO_PIN_3 | GPIO_PIN_2 | GPIO_PIN_1 | GPIO_PIN_0,
	GPIO_PIN_7 | GPIO_PIN_6 | GPIO_PIN_5 | GPIO_PIN_4,
	GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3
#endif
#ifdef __TM4C1294NCPDT__
	GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5,
	GPIO_PIN_5 | GPIO_PIN_4 | GPIO_PIN_4 | GPIO_PIN_5,
	GPIO_PIN_3 | GPIO_PIN_2 | GPIO_PIN_1 | GPIO_PIN_0,
	GPIO_PIN_3 | GPIO_PIN_2 | GPIO_PIN_1 | GPIO_PIN_0,
	GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3
#endif
#endif
};

//*****************************************************************************
//
// The list of SlaveSelect Default Pins.    entire section spi3
//
//*****************************************************************************
static const uint8_t g_u8SlaveSelPins[] = {
#ifdef TARGET_IS_BLIZZARD_RB1
  PA_3, PF_3, PB_5, PD_1
#else
#ifdef __TM4C129XNCZAD__
  PA_3, PB_4, PD_2, PF_2
#endif
#ifdef __TM4C1294NCPDT__
   PA_3, PB_4, PD_2, PF_2, PQ_1
#endif
#endif
};


SPIClass::SPIClass(void) {
	SSIModule = NOT_ACTIVE;
	SSIBitOrder = MSBFIRST;

    //slaveSelect = SSPIN;            
    _ssBase_ = (uint32_t) portBASERegister(digitalPinToPort(SSPIN));   
    _ssMask_ = digitalPinToBitMask(SSPIN);                  
}

SPIClass::SPIClass(unsigned long module) {   
	SSIModule = module;
	SSIBitOrder = MSBFIRST;
	//slaveSelect = SSPIN;        
    _ssBase_ = (uint32_t) portBASERegister(digitalPinToPort(SSPIN));   
    _ssMask_ = digitalPinToBitMask(SSPIN); 
}
  
void SPIClass::begin(uint8_t ssPin) {
	unsigned long initialData = 0;

    if(SSIModule == NOT_ACTIVE) {
        SSIModule = BOOST_PACK_SPI;
    }

	ROM_SysCtlPeripheralEnable(g_ulSSIPeriph[SSIModule]);
	ROM_SSIDisable(SSIBASE);
	ROM_GPIOPinConfigure(g_ulSSIConfig[SSIModule][0]);
	ROM_GPIOPinConfigure(g_ulSSIConfig[SSIModule][1]);
	ROM_GPIOPinConfigure(g_ulSSIConfig[SSIModule][2]);
	ROM_GPIOPinConfigure(g_ulSSIConfig[SSIModule][3]);
#ifdef TARGET_IS_BLIZZARD_RB1
	ROM_GPIOPinTypeSSI(g_ulSSIPort[SSIModule], g_ulSSIPins[SSIModule]);
#else
#if defined(__TM4C129XNCZAD__) || defined(__TM4C1294NCPDT__)
    if (SSIModule == 1) { // 1 is a split port 
	    ROM_GPIOPinTypeSSI(GPIO_PORTB_BASE, GPIO_PIN_5 | GPIO_PIN_4);
	    ROM_GPIOPinTypeSSI(GPIO_PORTE_BASE, GPIO_PIN_4 | GPIO_PIN_5);
    } else {
	    ROM_GPIOPinTypeSSI(g_ulSSIPort[SSIModule], g_ulSSIPins[SSIModule]);
    }
#endif
#endif
	/*
	  Polarity Phase        Mode
	     0 	   0   SSI_FRF_MOTO_MODE_0
	     0     1   SSI_FRF_MOTO_MODE_1
	     1     0   SSI_FRF_MOTO_MODE_2
	     1     1   SSI_FRF_MOTO_MODE_3
	*/
	
    //slaveSelect = ssPin;                
     _ssBase_ = (uint32_t) portBASERegister(digitalPinToPort(ssPin));   
    _ssMask_ =  digitalPinToBitMask(ssPin);   
	//pinMode(slaveSelect, OUTPUT); 
    ROM_GPIOPinTypeGPIOOutput(_ssBase_, _ssMask_);

	/*
	 * Default to
	 * System Clock, SPI_MODE_0, MASTER,
	 * 4MHz bit rate, and 8 bit data
	*/
	ROM_SSIClockSourceSet(SSIBASE, SSI_CLOCK_SYSTEM);

#ifdef TARGET_IS_BLIZZARD_RB1
	ROM_SSIConfigSetExpClk(SSIBASE, SysCtlClockGet(), SSI_FRF_MOTO_MODE_0, SSI_MODE_MASTER, 8000000, 8);
#else
	ROM_SSIConfigSetExpClk(SSIBASE, F_CPU, SSI_FRF_MOTO_MODE_0, SSI_MODE_MASTER, 8000000, 8);
#endif

	ROM_SSIEnable(SSIBASE);

	//clear out any initial data that might be present in the RX FIFO
	while(ROM_SSIDataGetNonBlocking(SSIBASE, &initialData));
}
//spi3 replaces original
void SPIClass::begin() {
	begin(SSPIN);
}

//spi3 simulates both usual spi versions
void SPIClass::end(uint8_t ssPin) {     //for SPI compatibility only
	ROM_SSIDisable(SSIBASE);
}

void SPIClass::end() {    //for SPI compatibility only
	end(_ssMask_);         //any 8 bit value
}

void SPIClass::setBitOrder(uint8_t ssPin, uint8_t bitOrder) {
	SSIBitOrder = bitOrder;
}

void SPIClass::setBitOrder(uint8_t bitOrder) {
	SSIBitOrder = bitOrder;
}

void SPIClass::setDataMode(uint8_t mode) {
	HWREG(SSIBASE + SSI_O_CR0) &= ~(SSI_CR0_SPO | SSI_CR0_SPH);
	HWREG(SSIBASE + SSI_O_CR0) |= mode;
}

void SPIClass::setClockDivider(uint8_t divider){
  //value must be even
  HWREG(SSIBASE + SSI_O_CPSR) = divider;
}

//spi3 new version
uint8_t SPIClass::transfer(uint8_t ssPin, uint8_t data, uint8_t transferMode) {
	static uint8_t repeatFlag = false;         //are we CONTUNUEing a string?
    unsigned long rxtxData;
	//digitalWrite(ssPin, LOW);   
    //spi3a - pin=0 means use current
    //getting base and mask every time is much slower
    if (!ssPin || (!repeatFlag)) {
        _ssBase_ = (uint32_t) portBASERegister(digitalPinToPort(ssPin)); 
        _ssMask_ = digitalPinToBitMask(ssPin);  
    }
    ROM_GPIOPinWrite(_ssBase_, _ssMask_, 0); 

	rxtxData = data;
	if(SSIBitOrder == LSBFIRST) {
		asm("rbit %0, %1" : "=r" (rxtxData) : "r" (rxtxData));	// reverse order of 32 bits 
		asm("rev %0, %1" : "=r" (rxtxData) : "r" (rxtxData));	// reverse order of bytes to get original bits into lowest byte 
	}
	ROM_SSIDataPut(SSIBASE, (uint8_t) rxtxData);
    
	if(transferMode == SPI_LAST){ 
		//digitalWrite(ssPin, HIGH);
            ROM_GPIOPinWrite(_ssBase_, _ssMask_, _ssMask_); 
            repeatFlag = false;
        }
	else  {                                    //SPI_CONTUNUE
	   //digitalWrite(ssPin, LOW);
           //ROM_GPIOPinWrite(_ssBase_, ssMask_, 0);    // already low ???
            repeatFlag = true;                  //s we are in CONTIUNE transfer mode
        }
	while(ROM_SSIBusy(SSIBASE));

	ROM_SSIDataGet(SSIBASE, &rxtxData);
	if(SSIBitOrder == LSBFIRST) {
		asm("rbit %0, %1" : "=r" (rxtxData) : "r" (rxtxData));	// reverse order of 32 bits 
		asm("rev %0, %1" : "=r" (rxtxData) : "r" (rxtxData));	// reverse order of bytes to get original bits into lowest byte 
	}

	return (uint8_t) rxtxData;
}

//spi3 other form
uint8_t SPIClass::transfer(uint8_t ssPin, uint8_t data) {
  return transfer(ssPin, data, SPI_LAST);
}
//spi3 other form
uint8_t SPIClass::transfer(uint8_t data) {
  return transfer(0, data, SPI_LAST);       // pin=0 to use current
}

//byte array to 255 bytes
void SPIClass::transfer(uint8_t *dataString, uint8_t numBytes) {
    unsigned long rxtxData;

    //use currently active pin
    ROM_GPIOPinWrite(_ssBase_, _ssMask_, 0);     //low

    if (SSIBitOrder == LSBFIRST) {          // low byte first and low bit first within each byte
        for(unsigned char i = 0; i < numBytes ; i++){ 
            rxtxData = dataString[i];
            asm("rbit %0, %1" : "=r" (rxtxData) : "r" (rxtxData));  // reverse order of 32 bits 
            asm("rev %0, %1" : "=r" (rxtxData) : "r" (rxtxData));   // reverse order of bytes to get original bits into lowest byte 
            ROM_SSIDataPut(SSIBASE, (uint8_t) rxtxData);
            while(ROM_SSIBusy(SSIBASE));            //avoid buffer overflow
         }
    }
    else {
        for(unsigned char i = numBytes; i > 0 ; i--){ 
            rxtxData = dataString[i - 1];
            ROM_SSIDataPut(SSIBASE, (uint8_t) rxtxData);
            while(ROM_SSIBusy(SSIBASE));            //avoid buffer overflow
        }
    }
    ROM_GPIOPinWrite(_ssBase_, _ssMask_, _ssMask_); //high
}

void SPIClass::setModule(uint8_t module) {
	SSIModule = module;
	begin();
}

//Convert from one SPI instantiation of SPI to 4 modules: 
//SPI0, SPI1, SPI (for backwards compatibility), and SPI3 
//SPI is the default for previous SPI libraries and for 
//TIVA C: SPI BUS 2 -> BOOST_PACK_SPI
SPIClass SPI0(0);
SPIClass SPI1(1);
SPIClass SPI(2);   // default as "SPI" 
SPIClass SPI3(3);
