/*
 * HITICommSupport
 * HCS_LowAccess_IO.h
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



// *****************************************************************************
// Include Guard
// *****************************************************************************

#ifndef HCS_LowAccess_IO_h
#define HCS_LowAccess_IO_h



// *****************************************************************************
// Include dependencies
// *****************************************************************************

// HITICommSupport
#include "HITICommSupport.h"



// ********************************************************************************
// Methods
// ********************************************************************************


// --------------------------------------------------------------------------------
// Loop() Cycle Time --------------------------------------------------------------
// --------------------------------------------------------------------------------

// Calculate Cycle Time (us)
// must be called at each loop cycle (called by HC_receiveMessageFromComputer())
void HCS_calculateCycleTime();

// Get cycle time (us)
unsigned long HCS_getCycleTime();


// --------------------------------------------------------------------------------
// pin data (mode, registers) -----------------------------------------------------
// --------------------------------------------------------------------------------

// Set pin mode (INPUT, OUTPUT, INPUT_PULLUP, INPUT_PULLDOWN)
void HCS_pinMode(uint8_t pin, uint8_t mode);

// Get pin mode (INPUT, OUTPUT, INPUT_PULLUP, INPUT_PULLDOWN, MODE_UNKNOWN)
uint8_t HCS_getPinMode(uint8_t pin);


// --------------------------------------------------------------------------------
// ADC/DAC/PWM specificities ------------------------------------------------------
// --------------------------------------------------------------------------------

#if defined ARDUINO_ARCH_SAMD
    // set ADC/DAC/PWM resolution (in bit). To call from setup()
    void HCS_adcResolution(uint8_t resolution);
    void HCS_pwmResolution(uint8_t resolution);
    void HCS_dacResolution(uint8_t resolution);
    
    // get ADC/DAC/PWM resolution
    uint8_t HCS_getAdcResolution(void);
    uint8_t HCS_getPwmResolution(void);
    uint8_t HCS_getDacResolution(void);

    // DAC is enabled
    bool HCS_DacIsEnabled(uint8_t pin);
#endif


// --------------------------------------------------------------------------------
// read DIO (Digital state, Analog value) -----------------------------------------
// --------------------------------------------------------------------------------

// read Digital IO
bool HCS_readDI_LA(uint8_t pin);
bool HCS_readDO_LA(uint8_t pin);

// read Analog IO
uint16_t HCS_readAI_LA(uint8_t pin);
#if defined ARDUINO_ARCH_SAMD
    uint16_t HCS_readAI_LA(uint8_t pin, bool forceReading);
    uint16_t HCS_readPWM_LA(uint8_t pin);
    uint16_t HCS_readDAC_LA(uint8_t pin);
#else
    uint8_t HCS_readPWM_LA(uint8_t pin);
#endif

// --------------------------------------------------------------------------------
// write DIO (Digital state, Analog value) ----------------------------------------
// --------------------------------------------------------------------------------

// write Digital Output
void HCS_writeDO_LA(uint8_t pin, uint8_t val);

// write Analog Output
#if defined ARDUINO_ARCH_SAMD
    void HCS_writePWM_LA(uint8_t pin, uint16_t val);
    void HCS_writeDAC_LA(uint8_t pin, uint16_t val);
#else
    void HCS_writePWM_LA(uint8_t pin, uint8_t val);
#endif

#endif
