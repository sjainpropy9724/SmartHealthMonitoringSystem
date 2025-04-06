/*
 * HITICommSupport
 * HCS_Serial.cpp
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


#include "HCS_Serial.h"



// *****************************************************************************
// Define
// *****************************************************************************



// *****************************************************************************
// Include dependencies
// *****************************************************************************

// Arduino
#include <Arduino.h>



// *****************************************************************************
// Standard methods
// *****************************************************************************

// utility
void HCS_Serial_setBaudrate(long baudrate)                  { Serial.begin(baudrate); }
int HCS_Serial_isAvailable()                                { return Serial.available(); }

// read
int HCS_Serial_read()                                       { return Serial.read(); }

// print
void HCS_Serial_println()                                   { Serial.println(); }
void HCS_Serial_print(char c)                               { Serial.print(c); }
void HCS_Serial_print(const char* string)                   { Serial.print(string); }
void HCS_Serial_print(int i, int base /* = 10 */)           { Serial.print(i, base); }
void HCS_Serial_print(unsigned int i, int base /* = 10 */)  { Serial.print(i, base); }
void HCS_Serial_print(unsigned long l, int base /* = 10 */) { Serial.print(l, base); }
void HCS_Serial_print(double d, int decimal /* = 2*/)       { Serial.print(d, decimal); }



// *****************************************************************************
// PROGMEM String methods
// *****************************************************************************

// convert PROGMEM String pointer to FlashStringHelper pointer
#define pgm_to_stringHelper(pgm_string)                     (reinterpret_cast<const __FlashStringHelper*>(pgm_string))

// print PROGMEM String
void HCS_Serial_print_P(const char* pgm_string)             { Serial.print(pgm_to_stringHelper(pgm_string)); }
