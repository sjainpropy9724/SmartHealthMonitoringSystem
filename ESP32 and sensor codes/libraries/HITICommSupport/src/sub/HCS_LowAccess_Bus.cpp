/*
 * HITICommSupport
 * HCS_LowAccess_Bus.cpp
 *
 * Copyright © 2021 Christophe LANDRET
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *         http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */



#include "HCS_LowAccess_Bus.h"



// *****************************************************************************
// Include dependencies
// *****************************************************************************

// Arduino
#include <Arduino.h>



// *****************************************************************************
// Define
// *****************************************************************************


// SPI
#if defined ARDUINO_ARCH_AVR
    #define SPI0_is_enabled()   (bit_is_set(SPCR, SPE) != 0)
    #define SPI0_is_master()    (bit_is_set(SPCR, MSTR) != 0)
#elif defined ARDUINO_ARCH_MEGAAVR
    #define SPI0_is_enabled()   (bit_is_set(SPI0.CTRLA, SPI_ENABLE_bp) != 0)
    #define SPI0_is_master()    (bit_is_set(SPI0.CTRLA, SPI_MASTER_bp) != 0)
#elif defined ARDUINO_ARCH_SAMD
    #define SPI0_is_enabled()   (SERCOM1->SPI.CTRLA.bit.ENABLE != 0)
    #define SPI0_is_master()    (SERCOM1->SPI.CTRLA.bit.MODE == SPI_MASTER_OPERATION)

    #define SPI1_is_enabled()   (SERCOM4->SPI.CTRLA.bit.ENABLE != 0)
    #define SPI1_is_master()    (SERCOM4->SPI.CTRLA.bit.MODE == SPI_MASTER_OPERATION)  // Nina wifi, Serial2
#endif

#ifndef SPI0_is_enabled
    #define SPI0_is_enabled()   false
#endif
#ifndef SPI0_is_master
    #define SPI0_is_master()    false
#endif
#ifndef SPI1_is_enabled
    #define SPI1_is_enabled()   false
#endif
#ifndef SPI1_is_master
    #define SPI1_is_master()    false
#endif

#define SPI_mode     ((uint8_t)SPI0_is_enabled() | (SPI0_is_master() << 1) | (SPI1_is_enabled() << 2) | (SPI1_is_master() << 3))


// TWI / 2-wire / i2c
#if defined ARDUINO_ARCH_AVR
    #define TWI0_is_enabled()   (bit_is_set(TWCR, TWEN) != 0)
#elif defined ARDUINO_ARCH_MEGAAVR
    #define TWI0_is_enabled()   ((bit_is_set(TWI0.MCTRLA, SPI_ENABLE_bp) | bit_is_set(TWI0.SCTRLA, SPI_ENABLE_bp)) != 0)
#elif defined ARDUINO_ARCH_SAMD
    #define TWI0_is_enabled()   (SERCOM2->I2CM.CTRLA.bit.ENABLE != 0)  // I2CM and I2CS should point to the same address
    #define TWI0_is_master()    (SERCOM2->I2CM.CTRLA.bit.MODE == I2C_MASTER_OPERATION)
#endif

#ifndef TWI0_is_enabled
    #define TWI0_is_enabled()   false
#endif
#ifndef TWI0_is_master
    #define TWI0_is_master()    false
#endif
#ifndef TWI1_is_enabled
    #define TWI1_is_enabled()   false
#endif
#ifndef TWI1_is_master
    #define TWI1_is_master()    false
#endif

#define TWI_mode     ((uint8_t)TWI0_is_enabled() | (TWI0_is_master() << 1) | (TWI1_is_enabled() << 2) | (TWI1_is_master() << 3))


// Serial TX/RX
#if defined ARDUINO_ARCH_AVR
    #if defined(UCSR0B)
        #define USART0_TX_is_enabled()   (bit_is_set(UCSR0B, TXEN0) != 0)
        #define USART0_RX_is_enabled()   (bit_is_set(UCSR0B, RXEN0) != 0)
    #endif    
    #if defined(UCSR1B)
        #define USART1_TX_is_enabled()   (bit_is_set(UCSR1B, TXEN1) != 0)
        #define USART1_RX_is_enabled()   (bit_is_set(UCSR1B, RXEN1) != 0)
    #endif
    #if defined(UCSR2B)
        #define USART2_TX_is_enabled()   (bit_is_set(UCSR2B, TXEN2) != 0)
        #define USART2_RX_is_enabled()   (bit_is_set(UCSR2B, RXEN2) != 0) 
    #endif
    #if defined(UCSR3B)
        #define USART3_TX_is_enabled()   (bit_is_set(UCSR3B, TXEN3) != 0)
        #define USART3_RX_is_enabled()   (bit_is_set(UCSR3B, RXEN3) != 0) 
    #endif
#elif defined ARDUINO_ARCH_MEGAAVR
    #if defined(USART0)
        #define USART0_TX_is_enabled()   (bit_is_set(USART0.CTRLB, USART_TXEN_bp) != 0)
        #define USART0_RX_is_enabled()   (bit_is_set(USART0.CTRLB, USART_RXEN_bp) != 0)
    #endif    
    #if defined(USART1)
        #define USART1_TX_is_enabled()   (bit_is_set(USART1.CTRLB, USART_TXEN_bp) != 0)
        #define USART1_RX_is_enabled()   (bit_is_set(USART1.CTRLB, USART_RXEN_bp) != 0) 
    #endif
    #if defined(USART2)
        #define USART2_TX_is_enabled()   (bit_is_set(USART2.CTRLB, USART_TXEN_bp) != 0)
        #define USART2_RX_is_enabled()   (bit_is_set(USART2.CTRLB, USART_RXEN_bp) != 0) 
    #endif
    #if defined(USART3)
        #define USART3_TX_is_enabled()   (bit_is_set(USART3.CTRLB, USART_TXEN_bp) != 0)
        #define USART3_RX_is_enabled()   (bit_is_set(USART3.CTRLB, USART_RXEN_bp) != 0) 
    #endif
#elif defined ARDUINO_ARCH_SAMD
    #define USART1_TX_is_enabled()   (SERCOM5->USART.CTRLA.bit.ENABLE != 0)  // Serial1
    #define USART1_RX_is_enabled()   (SERCOM5->USART.CTRLA.bit.ENABLE != 0)

    // Sercom 0 and 3 are unused by Arduino
#endif

#ifndef USART0_TX_is_enabled
    #define USART0_TX_is_enabled()   false
#endif
#ifndef USART0_RX_is_enabled
    #define USART0_RX_is_enabled()   false
#endif
#ifndef USART1_TX_is_enabled
    #define USART1_TX_is_enabled()   false
#endif
#ifndef USART1_RX_is_enabled
    #define USART1_RX_is_enabled()   false
#endif
#ifndef USART2_TX_is_enabled
    #define USART2_TX_is_enabled()   false
#endif
#ifndef USART2_RX_is_enabled
    #define USART2_RX_is_enabled()   false
#endif
#ifndef USART3_TX_is_enabled
    #define USART3_TX_is_enabled()   false
#endif
#ifndef USART3_RX_is_enabled
    #define USART3_RX_is_enabled()   false
#endif

#define USART_mode       (((uint8_t)USART0_TX_is_enabled()  | (USART0_RX_is_enabled() << 1) | (USART1_RX_is_enabled() << 2) | (USART1_RX_is_enabled() << 3) | \
                           (USART2_TX_is_enabled() << 4) | (USART2_RX_is_enabled() << 5) | (USART3_RX_is_enabled() << 6) | (USART3_RX_is_enabled() << 7))) 


// I2S
#if defined (ARDUINO_ARCH_SAMD) && defined(_SAMD21_)
    // "I2S" atmel macro has been redefined by Arduino... The only way to access it is to retrieve the I2S register address (see samd21g18a.h)
    //   => only works for SAMD21 / SAMDA1 family. 
    #define I2S0_is_enabled()   ((((I2s*) 0x42005000UL)->CTRLA.bit.ENABLE != 0) && (PM->APBCMASK.bit.I2S_ != 0))
#endif

#ifndef I2S0_is_enabled
    #define I2S0_is_enabled()   false
#endif
#ifndef I2S0_is_master
    #define I2S0_is_master()    false
#endif

#define I2S_mode     ((uint8_t)I2S0_is_enabled() | (I2S0_is_master() << 1))



// *****************************************************************************
// Methods
// *****************************************************************************

uint8_t HCS_getSpiMode(void)   { return SPI_mode; }
uint8_t HCS_getTwiMode(void)   { return TWI_mode; }
uint8_t HCS_getUsartMode(void) { return USART_mode; }
uint8_t HCS_getI2SMode(void)   { return I2S_mode; }
