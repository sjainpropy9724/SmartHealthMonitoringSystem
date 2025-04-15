/*
 * HITICommSupport
 * HCS_Board.cpp
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



#include "HCS_Board.h"



// *****************************************************************************
// Include dependencies
// *****************************************************************************

// Arduino
#include <arduino.h>



// *****************************************************************************
// Define
// *****************************************************************************

#define HC_xstr(s)     HC_str(s)
#define HC_str(s)      #s



// *****************************************************************************
// Architecture
// *****************************************************************************

// (predefined AVR-GCC compiler macro)
// according to https://gcc.gnu.org/onlinedocs/gcc/AVR-Options.html#AVR-Options
// 
// valid for AVR and megaAVR
// values:  2, 25, 3, 31, 35, 4, 5, 51, 6
//       => mcu = avr2, avr25, avr3, avr31, avr35, avr4, avr5, avr51, avr6
// values: 100, 102, 103, 104, 105, 106, 107 
//       => mcu = avrxmega2, avrxmega3, avrxmega4, avrxmega5, avrxmega6, avrxmega7, avrtiny
#if defined (__AVR_ARCH__)
    const char HC_g_architecture = __AVR_ARCH__;
#endif

// (predefined Arduino macro)
#if !defined(ARDUINO_ARCH_AVR) && !defined(ARDUINO_ARCH_MEGAAVR) && !defined(ARDUINO_ARCH_SAMD)
    //!defined(ARDUINO_ARCH_STM32F4) && !defined(ARDUINO_ARCH_NRF52) && !defined(ARDUINO_ARCH_MBED)
    #error "This library only supports boards with an AVR, MegaAVR, or SAMD processor."
#endif



// *****************************************************************************
// Processor
// *****************************************************************************

// AVR, megaAVR
#if defined (__AVR_DEVICE_NAME__)
    #define HC_xstr(s)     HC_str(s)
    #define HC_str(s)      #s
    const char HC_g_processor[] PROGMEM = HC_xstr(__AVR_DEVICE_NAME__); // predefined AVR-GCC macro

// SAMD
#elif defined(ARDUINO_ARCH_SAMD)
    #if defined(__SAMD21G18A__)
        const char HC_g_processor[] PROGMEM = "samd21g18a";
    #else
        const char HC_g_processor[] PROGMEM = "Uk";
    #endif

// SAMD
#elif defined(ARDUINO_ARCH_SAM)
    #if defined(__SAM3X8E__)
        const char HC_g_processor[] PROGMEM = "sam3x8e";
    #else
        const char HC_g_processor[] PROGMEM = "Uk";
    #endif

#else
    const char HC_g_processor[] PROGMEM = "Uk";
#endif



// *****************************************************************************
// Boards type (from boards.txt)  
// *****************************************************************************

#if defined(TEENSYDUINO) 
    // Boards.txt for Teensy ************************************************

    // Teensy 1.0
    #if defined(__AVR_AT90USB162__)
        const char HC_g_board[] PROGMEM = "T1";

    // Teensy 2.0
    #elif defined(__AVR_ATmega32U4__)
        const char HC_g_board[] PROGMEM = "T2";

    // Teensy++ 2.0
    #elif defined(__AVR_AT90USB1286__)       
        const char HC_g_board[] PROGMEM = "T+2";

    // Teensy 3.0
    #elif defined(__MK20DX128__)       
        const char HC_g_board[] PROGMEM = "T3";

    // Teensy 3.2 and 3.1 (obsolete)
    #elif defined(__MK20DX256__)       
        const char HC_g_board[] PROGMEM = "T32";

    // Teensy LC
    #elif defined(__MKL26Z64__)       
        const char HC_g_board[] PROGMEM = "TLC";

    // Teensy 3.5
    #elif defined(__MK64FX512__)
        const char HC_g_board[] PROGMEM = "T35";

    // Teensy 3.6
    #elif defined(__MK66FX1M0__)
        const char HC_g_board[] PROGMEM = "T36";

    // Unknown 
    #else
        const char HC_g_board[] PROGMEM = "Uk";

    #endif

#else 
    // Boards.txt for AVR ***************************************************

    // Yun
    // - mcu : atmega32u4
    // - variant : yun
    #if defined(ARDUINO_AVR_YUN)       
        const char HC_g_board[] PROGMEM = "Y";

    // Uno
    // - mcu : atmega328p
    // - variant : standard
    #elif defined(ARDUINO_AVR_UNO)       
        const char HC_g_board[] PROGMEM = "U";

    // Duemilanove or Diecimila
    // - mcu : atmega328p, atmega168
    // - variant : standard
    #elif defined(ARDUINO_AVR_DUEMILANOVE)       
        const char HC_g_board[] PROGMEM = "DM";

    // Nano
    // - mcu : atmega328p, atmega168
    // - variant : eightanaloginputs
    #elif defined(ARDUINO_AVR_NANO)       
        const char HC_g_board[] PROGMEM = "NN";

    // Mega 2560
    // - mcu : atmega2560
    // - variant : mega
    #elif defined(ARDUINO_AVR_MEGA2560)       
        const char HC_g_board[] PROGMEM = "M256";

    // Mega
    // - mcu : atmega1280
    // - variant : mega
    #elif defined(ARDUINO_AVR_MEGA)       
        const char HC_g_board[] PROGMEM = "M";

    // Mega Adk
    // - mcu : atmega2560
    // - variant : mega
    #elif   defined(ARDUINO_AVR_ADK)       
        const char HC_g_board[] PROGMEM = "MAdk";

    // Leonardo
    // - mcu : atmega32u4
    // - variant : leonardo
    #elif defined(ARDUINO_AVR_LEONARDO)       
        const char HC_g_board[] PROGMEM = "L";

    // Leonardo Ethernet
    // - mcu : atmega32u4
    // - variant : leonardo
    #elif defined(ARDUINO_AVR_LEONARDO_ETH)       
        const char HC_g_board[] PROGMEM = "LE";

    // Micro
    // - mcu : atmega32u4
    // - variant : micro
    #elif defined(ARDUINO_AVR_MICRO)       
        const char HC_g_board[] PROGMEM = "MC";

    // Esplora
    // - mcu : atmega32u4
    // - variant : leonardo
    #elif defined(ARDUINO_AVR_ESPLORA)       
        const char HC_g_board[] PROGMEM = "EP";

    // Mini
    // - mcu : atmega328p, atmega168
    // - variant : eightanaloginputs
    #elif defined(ARDUINO_AVR_MINI)       
        const char HC_g_board[] PROGMEM = "MN";

    // Ethernet
    // - mcu : atmega328p
    // - variant : ethernet
    #elif defined(ARDUINO_AVR_ETHERNET)       
        const char HC_g_board[] PROGMEM = "Et";

    // Fio
    // - mcu : atmega328p
    // - variant : eightanaloginputs
    #elif defined(ARDUINO_AVR_FIO)       
        const char HC_g_board[] PROGMEM = "Fi";

    // BT
    // - mcu : atmega328p, atmega168
    // - variant : eightanaloginputs
    #elif defined(ARDUINO_AVR_BT)
        const char HC_g_board[] PROGMEM = "BT";

    // Lilypad Usb
    // - mcu : atmega32u4
    // - variant : leonardo
    #elif defined(ARDUINO_AVR_LILYPAD_USB)
        const char HC_g_board[] PROGMEM = "LiU";

    // Lilypad
    // - mcu : atmega328p, atmega168
    // - variant : standard
    #elif defined(ARDUINO_AVR_LILYPAD)
        const char HC_g_board[] PROGMEM = "Li";

    // Pro or Pro Mini
    // - mcu : atmega328p, atmega168
    // - variant : eightanaloginputs
    #elif defined(ARDUINO_AVR_PRO)       
        const char HC_g_board[] PROGMEM = "Pr";

    // NG or older
    // - mcu : atmega168, atmega8
    // - variant : standard
    #elif defined(ARDUINO_AVR_NG)       
        const char HC_g_board[] PROGMEM = "NG";

    // Robot Control
    // - mcu : atmega32u4
    // - variant : robot_control
    #elif defined(ARDUINO_AVR_ROBOT_CONTROL)       
        const char HC_g_board[] PROGMEM = "RC";

    // Robot Motor
    // - mcu : atmega32u4
    // - variant : robot_motor
    #elif defined(ARDUINO_AVR_ROBOT_MOTOR)       
        const char HC_g_board[] PROGMEM = "RM";

    // Gemma
    // - mcu : attiny85
    // - variant : gemma
    #elif defined(ARDUINO_AVR_GEMMA)
        const char HC_g_board[] PROGMEM = "G";

    // Adafruit Circuit Playground 32u4 / Caterina Configuration
    // - mcu : atmega32u4
    // - variant : circuitplay32u4
    #elif defined(ARDUINO_AVR_CIRCUITPLAY)
        const char HC_g_board[] PROGMEM = "Ad4";

    // Yun Mini
    // - mcu : atmega32u4
    // - variant : yun
    #elif defined(ARDUINO_AVR_YUNMINI)
        const char HC_g_board[] PROGMEM = "YM";

    // Industrial 101
    // - mcu : atmega32u4
    // - variant : yun
    #elif defined(ARDUINO_AVR_INDUSTRIAL101)
        const char HC_g_board[] PROGMEM = "I101";

    // Linino One
    // - mcu : atmega32u4
    // - variant : yun
    #elif defined(ARDUINO_AVR_LININO_ONE)
        const char HC_g_board[] PROGMEM = "LNO";

    // Uno Wifi
    // - mcu : atmega328p
    // - variant : standard
    #elif defined(ARDUINO_AVR_UNO_WIFI_DEV_ED)
        const char HC_g_board[] PROGMEM = "UW";


    // Boards.txt for megaAVR *****************************************

    // Uno Wifi Rev2
    // - mcu : atmega4809
    // - variant : uno2018
    #elif defined(ARDUINO_AVR_UNO_WIFI_REV2)       
        const char HC_g_board[] PROGMEM = "UW2";

    // Nano Every
    // - mcu : atmega4809
    // - variant : nona4809
    #elif defined(ARDUINO_AVR_NANO_EVERY)       
        const char HC_g_board[] PROGMEM = "NNE";


    // Boards.txt for SAMD ********************************************

    // Zero
    // - mcu : ATSAMD21G18A (cortex M0+)
    // - variant : arduino_zero
    #elif defined(ARDUINO_SAMD_ZERO)       
        const char HC_g_board[] PROGMEM = "Z";

    // MKR Zero
    // - mcu : ATSAMD21G18A (cortex M0+)
    // - variant : mkrzero
    #elif defined(ARDUINO_SAMD_MKRZERO)       
        const char HC_g_board[] PROGMEM = "MKZ";

    // MKR 1000
    // - mcu : ATSAMD21G18A (cortex M0+), ATSAMW25 
    // - variant : mkr1000
    #elif defined(ARDUINO_SAMD_MKR1000)       
        const char HC_g_board[] PROGMEM = "MK10";

    // MKR Wifi 1010
    // - mcu : ATSAMD21G18A (cortex M0+)
    // - variant : mkrwifi1010
    #elif defined(ARDUINO_SAMD_MKRWIFI1010)       
        const char HC_g_board[] PROGMEM = "MK1";

    // MKR FOX 1200
    // - mcu : ATSAMD21G18A (cortex M0+)
    // - variant : mkrfox1200
    #elif defined(ARDUINO_SAMD_MKRFox1200)       
        const char HC_g_board[] PROGMEM = "MK12";

    // MKR WAN 1300
    // - mcu : ATSAMD21G18A (cortex M0+)
    // - variant : mkrwan1300
    #elif defined(ARDUINO_SAMD_MKRWAN1300)    
        const char HC_g_board[] PROGMEM = "MK13";

    // MKR WAN 1310
    // - mcu : ATSAMD21G18A (cortex M0+)
    // - variant : mkrwan1310
    #elif defined(ARDUINO_SAMD_MKRWAN1310)       
        const char HC_g_board[] PROGMEM = "MK131";

    // MKR GSM 1400
    // - mcu : ATSAMD21G18A (cortex M0+)
    // - variant : mkrgsm1400
    #elif defined(ARDUINO_SAMD_MKRGSM1400)       
        const char HC_g_board[] PROGMEM = "MK14";

    // MKR NB 1500
    // - mcu : ATSAMD21G18A (cortex M0+)
    // - variant : mkrnb1500
    #elif defined(ARDUINO_SAMD_MKRNB1500)       
        const char HC_g_board[] PROGMEM = "MK15";

    // MKR Vidor 4000
    // - mcu : ATSAMD21G18A (cortex M0+)
    // - variant : mkrnb1500
    #elif defined(ARDUINO_SAMD_MKRVIDOR4000)       
        const char HC_g_board[] PROGMEM = "MK40";

    // Nano 33 IOT
    // - mcu : ATSAMD21G18A (cortex M0+)
    // - variant : nano_33_iot
    #elif defined(ARDUINO_SAMD_NANO_33_IOT)       
        const char HC_g_board[] PROGMEM = "33I";

    // Adafruit Circuit Playground M0
    // - mcu : ATSAMD21G18A (cortex M0+)
    // - variant : circuitplay
    #elif defined(ARDUINO_SAMD_CIRCUITPLAYGROUND_EXPRESS)       
        const char HC_g_board[] PROGMEM = "Ad0";

    // M0 Pro and M0
    // - mcu : ATSAMD21G18A (cortex M0+)
    // - variant : arduino_mzero
    #elif defined(ARDUINO_SAM_ZERO)       
        const char HC_g_board[] PROGMEM = "M0";

    // Tian
    // - mcu : ATSAMD21G18A (cortex M0+)
    // - variant : arduino_mzero
    #elif defined(ARDUINO_SAMD_TIAN)       
        const char HC_g_board[] PROGMEM = "T";


    // Boards.txt for SAM *********************************************

    // Due
    // - mcu : ATSAM3X8E (cortex M3)
    // - variant : arduino_due_x
    #elif defined(ARDUINO_SAM_DUE)       
        const char HC_g_board[] PROGMEM = "D";


    // Boards.txt for Intel Curie *************************************

    // 101
    // - mcu : ARCv2EM
    // - variant : arduino_101
    #elif defined(ARDUINO_ARC32_TOOLS)       
        const char HC_g_board[] PROGMEM = "101";


    // Boards.txt for nRF52 *******************************************

    // Primo Core
    // - mcu : cortex-m4
    // - variant : arduino_primo_core
    #elif defined(ARDUINO_NRF52_PRIMO_CORE)       
        const char HC_g_board[] PROGMEM = "Pr";


    // Unknown
    #else
        const char HC_g_board[] PROGMEM = "Uk";

    #endif

#endif



// *****************************************************************************
// Getters
// *****************************************************************************

// return variables stored in PROGMEM
const char* HCS_getProcessor_P()            { return HC_g_processor; }
const char* HCS_getBoard_P()                { return HC_g_board; }

// return normal variables
unsigned long HCS_getArduinoLibVersion()    { return (unsigned long) ARDUINO; }