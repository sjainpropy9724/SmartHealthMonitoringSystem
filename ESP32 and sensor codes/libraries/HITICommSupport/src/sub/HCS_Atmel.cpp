/*
 * HITICommSupport
 * HCS_Atmel.cpp
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


#include "HCS_Atmel.h"



// *****************************************************************************
// Include dependencies
// *****************************************************************************

// Arduino
#include <Arduino.h>



// *****************************************************************************
// Methods
// *****************************************************************************

// Ram (on SAMD, unsigned int = uint32_t)
unsigned int HCS_getRamStart()             { return RAMSTART; }
unsigned int HCS_getRamEnd()               { return RAMEND; }

// Stack pointer
#if defined(ARDUINO_ARCH_SAMD)
    // get Main Stack Pointer (there are 2 SP, but only that one is used by Arduino)
    uint32_t HCS_getMSP()                  { return __get_MSP(); }
#endif
