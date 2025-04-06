/*
 * HITICommSupport
 * HCS_Board.h
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

#ifndef HCS_Board_h
#define HCS_Board_h



// *****************************************************************************
// Boards variation (from boards.txt) 
// *****************************************************************************

// AVR
#define HC_VARIANT_UNKNOWN             0x0 
#define HC_VARIANT_CIRCUITPLAY         0x1 
#define HC_VARIANT_EIGHTANALOGINPUTS   0x2 
#define HC_VARIANT_ETHERNET            0x3 
#define HC_VARIANT_GEMMA               0x4 
#define HC_VARIANT_LEONARDO            0x5 
#define HC_VARIANT_MEGA                0x6 
#define HC_VARIANT_MICRO               0x7 
#define HC_VARIANT_ROBOTCONTROL        0x8 
#define HC_VARIANT_ROBOTMOTOR          0x9 
#define HC_VARIANT_STANDARD            0xA 
#define HC_VARIANT_YUN                 0xB

// MegaAVR
#define HC_VARIANT_UNO2018             0x10
#define HC_VARIANT_NONA4809            0x11

// Cortex M0+
#define HC_VARIANT_ZERO                0x20
#define HC_VARIANT_MKRZERO             0x21
#define HC_VARIANT_MKR1000             0x22
#define HC_VARIANT_MKR1010             0x23
#define HC_VARIANT_MKR1200             0x24
#define HC_VARIANT_MKR1300             0x25 
#define HC_VARIANT_MKR1400             0x26
#define HC_VARIANT_MKR1500             0x27
#define HC_VARIANT_MKR4000             0x28
#define HC_VARIANT_NANO33IOT           0x29

// Cortex M3
#define HC_VARIANT_DUE                 0x30



// *****************************************************************************
// Boards type (from boards.txt)  
// *****************************************************************************

#if defined(TEENSYDUINO) 
    // Boards.txt for Teensy ************************************************

    // Teensy 1.0
    #if defined(__AVR_AT90USB162__)
        #define HC_VARIANT HC_VARIANT_UNKNOWN

    // Teensy 2.0
    #elif defined(__AVR_ATmega32U4__)
        #define HC_VARIANT HC_VARIANT_UNKNOWN

    // Teensy++ 2.0
    #elif defined(__AVR_AT90USB1286__)       
        #define HC_VARIANT HC_VARIANT_UNKNOWN

    // Teensy 3.0
    #elif defined(__MK20DX128__)       
        #define HC_VARIANT HC_VARIANT_UNKNOWN

    // Teensy 3.2 and 3.1 (obsolete)
    #elif defined(__MK20DX256__)       
        #define HC_VARIANT HC_VARIANT_UNKNOWN

    // Teensy LC
    #elif defined(__MKL26Z64__)       
        #define HC_VARIANT HC_VARIANT_UNKNOWN

    // Teensy 3.5
    #elif defined(__MK64FX512__)
        #define HC_VARIANT HC_VARIANT_UNKNOWN

    // Teensy 3.6
    #elif defined(__MK66FX1M0__)
        #define HC_VARIANT HC_VARIANT_UNKNOWN

    // Unknown 
    #else
        #define HC_VARIANT HC_VARIANT_UNKNOWN

    #endif

#else 
    // Boards.txt for AVR ***************************************************

    // Yun
    // - mcu : atmega32u4
    // - variant : yun
    #if defined(ARDUINO_AVR_YUN)       
        #define HC_VARIANT HC_VARIANT_YUN

    // Uno
    // - mcu : atmega328p
    // - variant : standard
    #elif defined(ARDUINO_AVR_UNO)       
        #define HC_VARIANT HC_VARIANT_STANDARD

    // Duemilanove or Diecimila
    // - mcu : atmega328p, atmega168
    // - variant : standard
    #elif defined(ARDUINO_AVR_DUEMILANOVE)       
        #define HC_VARIANT HC_VARIANT_STANDARD
        #define HC_USE_FTDI

    // Nano
    // - mcu : atmega328p, atmega168
    // - variant : eightanaloginputs
    #elif defined(ARDUINO_AVR_NANO)       
        #define HC_VARIANT HC_VARIANT_EIGHTANALOGINPUTS
        #define HC_USE_FTDI
    
    // Mega 2560
    // - mcu : atmega2560
    // - variant : mega
    #elif defined(ARDUINO_AVR_MEGA2560)       
        #define HC_VARIANT HC_VARIANT_MEGA

    // Mega
    // - mcu : atmega1280
    // - variant : mega
    #elif defined(ARDUINO_AVR_MEGA)       
        #define HC_VARIANT HC_VARIANT_MEGA

    // Mega Adk
    // - mcu : atmega2560
    // - variant : mega
    #elif   defined(ARDUINO_AVR_ADK)       
        #define HC_VARIANT HC_VARIANT_MEGA

    // Leonardo
    // - mcu : atmega32u4
    // - variant : leonardo
    #elif defined(ARDUINO_AVR_LEONARDO)       
        #define HC_VARIANT HC_VARIANT_LEONARDO

    // Leonardo Ethernet
    // - mcu : atmega32u4
    // - variant : leonardo
    #elif defined(ARDUINO_AVR_LEONARDO_ETH)       
        #define HC_VARIANT HC_VARIANT_LEONARDO

    // Micro
    // - mcu : atmega32u4
    // - variant : micro
    #elif defined(ARDUINO_AVR_MICRO)       
        #define HC_VARIANT HC_VARIANT_MICRO

    // Esplora
    // - mcu : atmega32u4
    // - variant : leonardo
    #elif defined(ARDUINO_AVR_ESPLORA)       
        #define HC_VARIANT HC_VARIANT_LEONARDO

    // Mini
    // - mcu : atmega328p, atmega168
    // - variant : eightanaloginputs
    #elif defined(ARDUINO_AVR_MINI)       
        #define HC_VARIANT HC_VARIANT_EIGHTANALOGINPUTS

    // Ethernet
    // - mcu : atmega328p
    // - variant : ethernet
    #elif defined(ARDUINO_AVR_ETHERNET)       
        #define HC_VARIANT HC_VARIANT_ETHERNET

    // Fio
    // - mcu : atmega328p
    // - variant : eightanaloginputs
    #elif defined(ARDUINO_AVR_FIO)       
        #define HC_VARIANT HC_VARIANT_EIGHTANALOGINPUTS

    // BT
    // - mcu : atmega328p, atmega168
    // - variant : eightanaloginputs
    #elif defined(ARDUINO_AVR_BT)
        #define HC_VARIANT HC_VARIANT_EIGHTANALOGINPUTS

    // Lilypad Usb
    // - mcu : atmega32u4
    // - variant : leonardo
    #elif defined(ARDUINO_AVR_LILYPAD_USB)
        #define HC_VARIANT HC_VARIANT_LEONARDO

    // Lilypad
    // - mcu : atmega328p, atmega168
    // - variant : standard
    #elif defined(ARDUINO_AVR_LILYPAD)
        #define HC_VARIANT HC_VARIANT_STANDARD

    // Pro or Pro Mini
    // - mcu : atmega328p, atmega168
    // - variant : eightanaloginputs
    #elif defined(ARDUINO_AVR_PRO)       
        #define HC_VARIANT HC_VARIANT_EIGHTANALOGINPUTS
        #define HC_USE_FTDI

    // NG or older
    // - mcu : atmega168, atmega8
    // - variant : standard
    #elif defined(ARDUINO_AVR_NG)       
        #define HC_VARIANT HC_VARIANT_STANDARD
        #define HC_USE_FTDI

    // Robot Control
    // - mcu : atmega32u4
    // - variant : robot_control
    #elif defined(ARDUINO_AVR_ROBOT_CONTROL)       
        #define HC_VARIANT HC_VARIANT_ROBOTCONTROL

    // Robot Motor
    // - mcu : atmega32u4
    // - variant : robot_motor
    #elif defined(ARDUINO_AVR_ROBOT_MOTOR)       
        #define HC_VARIANT HC_VARIANT_ROBOTMOTOR

    // Gemma
    // - mcu : attiny85
    // - variant : gemma
    #elif defined(ARDUINO_AVR_GEMMA)
        #define HC_VARIANT HC_VARIANT_GEMMA

    // Adafruit Circuit Playground 32u4 / Caterina Configuration
    // - mcu : atmega32u4
    // - variant : circuitplay32u4
    #elif defined(ARDUINO_AVR_CIRCUITPLAY)
        #define HC_VARIANT HC_VARIANT_CIRCUITPLAY

    // Yun Mini
    // - mcu : atmega32u4
    // - variant : yun
    #elif defined(ARDUINO_AVR_YUNMINI)
        #define HC_VARIANT HC_VARIANT_YUN

    // Industrial 101
    // - mcu : atmega32u4
    // - variant : yun
    #elif defined(ARDUINO_AVR_INDUSTRIAL101)
        #define HC_VARIANT HC_VARIANT_YUN

    // Linino One
    // - mcu : atmega32u4
    // - variant : yun
    #elif defined(ARDUINO_AVR_LININO_ONE)
        #define HC_VARIANT HC_VARIANT_YUN

    // Uno Wifi
    // - mcu : atmega328p
    // - variant : standard
    #elif defined(ARDUINO_AVR_UNO_WIFI_DEV_ED)
        #define HC_VARIANT HC_VARIANT_STANDARD


    // Boards.txt for megaAVR *****************************************

    // Uno Wifi Rev2
    // - mcu : atmega4809
    // - variant : uno2018
    #elif defined(ARDUINO_AVR_UNO_WIFI_REV2)       
        #define HC_VARIANT HC_VARIANT_UNO2018

    // Nano Every
    // - mcu : atmega4809
    // - variant : nona4809
    #elif defined(ARDUINO_AVR_NANO_EVERY)       
        #define HC_VARIANT HC_VARIANT_NONA4809


    // Boards.txt for SAMD ********************************************

    // Zero
    // - mcu : ATSAMD21G18A (cortex M0+)
    // - variant : arduino_zero
    #elif defined(ARDUINO_SAMD_ZERO)       
        #define HC_VARIANT HC_VARIANT_ZERO

	// MKR Zero
    // - mcu : ATSAMD21G18A (cortex M0+)
    // - variant : mkrzero
    #elif defined(ARDUINO_SAMD_MKRZERO)       
        #define HC_VARIANT HC_VARIANT_MKRZERO

    // MKR 1000
    // - mcu : ATSAMD21G18A (cortex M0+) 
    // - variant : mkr1000
    #elif defined(ARDUINO_SAMD_MKR1000)       
        #define HC_VARIANT HC_VARIANT_MKR1000

    // MKR Wifi 1010
    // - mcu : ATSAMD21G18A (cortex M0+)
    // - variant : mkrwifi1010
    #elif defined(ARDUINO_SAMD_MKRWIFI1010)       
        #define HC_VARIANT HC_VARIANT_MKR1010

    // MKR FOX 1200
    // - mcu : ATSAMD21G18A (cortex M0+)
    // - variant : mkrfox1200
    #elif defined(ARDUINO_SAMD_MKRFox1200)       
        #define HC_VARIANT HC_VARIANT_MKR1200

    // MKR WAN 1300
    // - mcu : ATSAMD21G18A (cortex M0+)
    // - variant : mkrwan1300
    #elif defined(ARDUINO_SAMD_MKRWAN1300)       
        #define HC_VARIANT HC_VARIANT_MKR1300

    // MKR WAN 1310
    // - mcu : ATSAMD21G18A (cortex M0+)
    // - variant : mkrwan1300
    #elif defined(ARDUINO_SAMD_MKRWAN1310)       
        #define HC_VARIANT HC_VARIANT_MKR1300

    // MKR GSM 1400
    // - mcu : ATSAMD21G18A (cortex M0+)
    // - variant : mkrgsm1400
    #elif defined(ARDUINO_SAMD_MKRGSM1400)       
        #define HC_VARIANT HC_VARIANT_MKR1400

    // MKR NB 1500
    // - mcu : ATSAMD21G18A (cortex M0+)
    // - variant : mkrnb1500
    #elif defined(ARDUINO_SAMD_MKRNB1500)       
        #define HC_VARIANT HC_VARIANT_MKR1500

    // MKR Vidor 4000
    // - mcu : ATSAMD21G18A (cortex M0+)
    // - variant : mkrvidor4000
    #elif defined(ARDUINO_SAMD_MKRVIDOR4000)       
        #define HC_VARIANT HC_VARIANT_MKR4000

    // Nano 33 IOT
    // - mcu : ATSAMD21G18A (cortex M0+)
    // - variant : nano_33_iot
    #elif defined(ARDUINO_SAMD_NANO_33_IOT)       
    #define HC_VARIANT HC_VARIANT_NANO33IOT

    // Adafruit Circuit Playground M0
    // - mcu : ATSAMD21G18A (cortex M0+)
    // - variant : circuitplay
    #elif defined(ARDUINO_SAMD_CIRCUITPLAYGROUND_EXPRESS)       
        #define HC_VARIANT HC_VARIANT_CIRCUITPLAY

    // M0 Pro and M0
    // - mcu : ATSAMD21G18A (cortex M0+)
    // - variant : arduino_mzero
    #elif defined(ARDUINO_SAM_ZERO)       
        #define HC_VARIANT HC_VARIANT_UNKNOWN

    // Tian
    // - mcu : ATSAMD21G18A (cortex M0+)
    // - variant : arduino_mzero
    #elif defined(ARDUINO_SAMD_TIAN)       
        #define HC_VARIANT HC_VARIANT_UNKNOWN


    // Boards.txt for SAM *********************************************

    // Due
    // - mcu : ATSAM3X8E (cortex M3)
    // - variant : arduino_due_x
    #elif defined(ARDUINO_SAM_DUE)       
        #define HC_VARIANT HC_VARIANT_DUE


    // Boards.txt for Intel Curie *************************************

    // 101
    // - mcu : ARCv2EM
    // - variant : arduino_101
    #elif defined(ARDUINO_ARC32_TOOLS)       
        #define HC_VARIANT HC_VARIANT_UNKNOWN


    // Boards.txt for nRF52 *******************************************

    // Primo Core
    // - mcu : cortex-m4
    // - variant : arduino_primo_core
    #elif defined(ARDUINO_NRF52_PRIMO_CORE)       
        #define HC_VARIANT HC_VARIANT_UNKNOWN


    // Unknown
    #else
        #define HC_VARIANT HC_VARIANT_UNKNOWN
    #endif

#endif



// *****************************************************************************
// Getters
// *****************************************************************************

// return variables stored in PROGMEM
const char* HCS_getProcessor_P();
const char* HCS_getBoard_P();

// return normal variables
unsigned long HCS_getArduinoLibVersion();


#endif
