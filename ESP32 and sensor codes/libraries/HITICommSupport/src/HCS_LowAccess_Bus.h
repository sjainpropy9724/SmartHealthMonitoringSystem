/*
 * HITICommSupport
 * HCS_LowAccess_Bus.h
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

#ifndef HCS_LowAccess_Bus_h
#define HCS_LowAccess_Bus_h



// *****************************************************************************
// Include dependencies
// *****************************************************************************

// HITICommSupport
#include "HITICommSupport.h"



// *****************************************************************************
// Methods
// *****************************************************************************

uint8_t HCS_getSpiMode();
uint8_t HCS_getTwiMode();
uint8_t HCS_getUsartMode();
uint8_t HCS_getI2SMode();


#endif
