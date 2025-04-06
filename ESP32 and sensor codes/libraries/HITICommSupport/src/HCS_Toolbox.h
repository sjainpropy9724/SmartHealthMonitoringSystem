/*
 * HITICommSupport
 * HCS_Toolbox.h
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

#ifndef HCS_Toolbox_h
#define HCS_Toolbox_h



// *****************************************************************************
// Define
// *****************************************************************************


// --------------------------------------------------------------------------
// Math ---------------------------------------------------------------------
// --------------------------------------------------------------------------

#define HCS_abs(value)                   (((value) < 0) ? -(value) : (value))
#define HCS_constrain(value, min, max)   (value > max ? max : (value < min ? min : value))



// --------------------------------------------------------------------------
// Bit operations -----------------------------------------------------------
// --------------------------------------------------------------------------

#define HCS_readBit(data, bit_index)               ((data >> bit_index) & 0x1)
#define HCS_writeBit(data, bit_index, bit_value)   (bit_value ? (data |= (1UL << bit_index)) : (data &= ~(1UL << bit_index)))



// --------------------------------------------------------------------------
// Word operations ----------------------------------------------------------
// --------------------------------------------------------------------------

// - make a Word by assembling 2 Byte
#define HCS_createWord(HByte, LByte)     ((((uint16_t) HByte) << 8) | LByte)

// - get the low/high Byte of a Word
#define HCS_getLowByte(wordValue)        ((uint8_t) (wordValue & 0xFF))
#define HCS_getHighByte(wordValue)       ((uint8_t) (wordValue >> 8))



// --------------------------------------------------------------------------
// DWord operations ---------------------------------------------------------
// --------------------------------------------------------------------------

// - make a DWord by assembling 2 Word
#define HCS_createDWord(HWord, LWord)    ((((uint32_t) HWord) << 16) | LWord)

// - get the low/high Word of a DWord
#define HCS_getLowWord(dwordValue)       ((uint16_t) (dwordValue & 0xFFFF))
#define HCS_getHighWord(dwordValue)      ((uint16_t) (dwordValue >> 16))



// *****************************************************************************
// Method
// *****************************************************************************


// --------------------------------------------------------------------------
// Math ---------------------------------------------------------------------
// --------------------------------------------------------------------------

float HCS_map(float input_value, float inputRange_min, float inputRange_max, float outputRange_min, float outputRange_max);

#endif
