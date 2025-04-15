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


#include "HCS_Toolbox.h"



// *****************************************************************************
// Include dependencies
// *****************************************************************************

// Arduino
#include <Arduino.h>



// *****************************************************************************
// Methods
// *****************************************************************************

// --------------------------------------------------------------------------
// Math ---------------------------------------------------------------------
// --------------------------------------------------------------------------

float HCS_map(float input, float inputRange_min, float inputRange_max, float outputRange_min, float outputRange_max)
{
	return outputRange_min + (input - inputRange_min) * (outputRange_max - outputRange_min) / (inputRange_max - inputRange_min);
}