/*
 * HITICommSupport
 * HCS_Serial.h
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

#ifndef HCS_Serial_h
#define HCS_Serial_h



// *****************************************************************************
// Include dependencies
// *****************************************************************************

// HITICommSupport
#include "HITICommSupport.h"



// *****************************************************************************
// Standard methods
// *****************************************************************************

// utility
void HCS_Serial_setBaudrate(long baudrate);
int HCS_Serial_isAvailable();

// read
int HCS_Serial_read();

// print
void HCS_Serial_println();
void HCS_Serial_print(char c);
void HCS_Serial_print(const char* string);
void HCS_Serial_print(int i, int base = 10);
void HCS_Serial_print(unsigned int i, int base = 10);
void HCS_Serial_print(unsigned long l, int base = 10);
void HCS_Serial_print(double d, int decimal = 2);



// *****************************************************************************
// PROGMEM String methods
// *****************************************************************************

// print PROGMEM String
void HCS_Serial_print_P(const char* pgm_string);

#endif