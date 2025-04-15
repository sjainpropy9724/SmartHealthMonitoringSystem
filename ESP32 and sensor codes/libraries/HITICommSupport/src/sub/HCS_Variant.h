/*
 * HITICommSupport
 * HCS_Variant.h
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

#ifndef HCS_Variant_h
#define HCS_Variant_h



// *****************************************************************************
// Include dependencies
// *****************************************************************************

// HITICommSupport
#include "HITICommSupport.h"
#include "HCS_Board.h"



// --------------------------------------------------------------------------------
// Variants (from boards.txt) -----------------------------------------------------
// --------------------------------------------------------------------------------


// Adafruit Circuit Playground 32u4 and M0 
#if HC_VARIANT == HC_VARIANT_CIRCUITPLAY
    #define HC_EEPROM_ONBOARD		

// Nano, Mini, Fio, BT, Pro, Pro Mini
#elif HC_VARIANT == HC_VARIANT_EIGHTANALOGINPUTS
    #define HC_EEPROM_ONBOARD		

// Ethernet
#elif HC_VARIANT == HC_VARIANT_ETHERNET
    #define HC_EEPROM_ONBOARD		

// Gemma
#elif HC_VARIANT == HC_VARIANT_GEMMA
    #define HC_EEPROM_ONBOARD		

// Leonardo, Leonardo Ethernet, Esplora, Lilypad Usb
#elif HC_VARIANT == HC_VARIANT_LEONARDO
    #define HC_EEPROM_ONBOARD		
    
// Mega 2560, Mega, Mega Adk
#elif HC_VARIANT == HC_VARIANT_MEGA
    #define HC_EEPROM_ONBOARD		

// Micro
#elif HC_VARIANT == HC_VARIANT_MICRO
    #define HC_EEPROM_ONBOARD		

// Robot Control
#elif HC_VARIANT == HC_VARIANT_ROBOTCONTROL
    #define HC_EEPROM_ONBOARD		

// Robot Motor
#elif HC_VARIANT == HC_VARIANT_ROBOTMOTOR
    #define HC_EEPROM_ONBOARD		

// Uno, Duemilanove, Diecimila, Lilypad, NG (or older), Uno Wifi
#elif HC_VARIANT == HC_VARIANT_STANDARD
    #define HC_EEPROM_ONBOARD		

// Yun, Yun Mini, Industrial 101, Linino One
#elif HC_VARIANT == HC_VARIANT_YUN
    #define HC_EEPROM_ONBOARD		

// Uno Wifi Rev2
#elif HC_VARIANT == HC_VARIANT_UNO2018
    #define HC_EEPROM_ONBOARD		

// Nano Every
#elif HC_VARIANT == HC_VARIANT_NONA4809
    #define HC_EEPROM_ONBOARD		

// Zero
#elif HC_VARIANT == HC_VARIANT_ZERO

// MKR ZERO
#elif HC_VARIANT == HC_VARIANT_MKRZERO

// MKR 1000
#elif HC_VARIANT == HC_VARIANT_MKR1000

// MKR WIFI 1010
#elif HC_VARIANT == HC_VARIANT_MKR1010

// MKR FOX 1200
#elif HC_VARIANT == HC_VARIANT_MKR1200

// MKR WAN 1300
#elif HC_VARIANT == HC_VARIANT_MKR1300

// MKR GSM 1400
#elif HC_VARIANT == HC_VARIANT_MKR1400

// MKR NB 1500
#elif HC_VARIANT == HC_VARIANT_MKR1500

// MKR Vidor 4000
#elif HC_VARIANT == HC_VARIANT_MKR4000

// Nano 33 IOT
#elif HC_VARIANT == HC_VARIANT_NANO33IOT

// Due
#elif HC_VARIANT == HC_VARIANT_DUE

#endif



// *****************************************************************************
// Getters
// *****************************************************************************

uint8_t HCS_getDIO_qty();
uint8_t HCS_getAI_qty();

uint8_t HCS_getDIO_startIndex();
uint8_t HCS_getDIO_endIndex();

uint8_t HCS_getAI_startIndex();
uint8_t HCS_getAI_endIndex();

bool HCS_hasPWM(uint8_t pin);

uint8_t HCS_getServo_qty();

bool HCS_isPWMEnabled(uint8_t pin, uint8_t servo_qty);

#if defined(ARDUINO_ARCH_SAMD) || defined(ARDUINO_ARCH_SAM)
uint8_t HCS_getDacPin(uint8_t index);
#endif

#endif
