/*
 * HITICommSupport
 * HCS_Variant.cpp
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



#include "HCS_Variant.h"



// *****************************************************************************
// Include dependencies
// *****************************************************************************

#include <Arduino.h>
#include <Servo.h>



// --------------------------------------------------------------------------------
// Variants (from boards.txt) -----------------------------------------------------
// --------------------------------------------------------------------------------


// Adafruit Circuit Playground 32u4 and M0 ****************************************

#if HC_VARIANT == HC_VARIANT_CIRCUITPLAY
// DIO quantity (includes AI)
#define HC_DIO_QTY				NUM_DIGITAL_PINS	// 31
// AI quantity 
#define HC_AI_QTY				NUM_ANALOG_INPUTS	// 12

// DIO Mapping
#define HC_DIO_STARTINDEX		0						// 0
#define HC_DIO_ENDINDEX			13						// 13

// AI Mapping
// (AO to A5 : DIO pin 18 to 23)
// (A6 to A11 : DIO pin 4, 6, 8, 9, 10, 12)
#define HC_AI_STARTINDEX		0						// 0
#define HC_AI_ENDINDEX			(HC_AI_QTY - 1)			// 11

// PWM Mapping (DIO pin 3, 5, 6, 9, 10, 11, 13)
#define HC_PIN_HAS_PWM(pin)	digitalPinHasPWM(pin)

// Servo
#define HC_SERVO_QTY_MAX		MAX_SERVOS				// 12

// If one servo is enabled, PWM on pins 9 and 10 are disabled
#define HC_PWM_IS_ENABLED(pin, servo_qty)  \
      ((servo_qty == 0) || \
      ((servo_qty > 0) && (pin != 9) && (pin != 10)))

// Serial  => USB, serial monitor
// Serial1 => pin 0 (RX) / 1 (TX)

// SPI0    => pin 14 (MISO) / 16 (MOSI) / 15 (SCK) / 17 (SS)

// TWI0    => pin 2 (SDA) / 3 (SCL)


// Nano, Mini, Fio, BT, Pro, Pro Mini *********************************************

#elif HC_VARIANT == HC_VARIANT_EIGHTANALOGINPUTS
// DIO quantity (includes AI)
#define HC_DIO_QTY				(NUM_DIGITAL_PINS + 2)	// 22
// AI quantity 
#define HC_AI_QTY				NUM_ANALOG_INPUTS		// 8

// DIO Mapping
#define HC_DIO_STARTINDEX		0						// 0
#define HC_DIO_ENDINDEX			(HC_DIO_QTY - HC_AI_QTY - 1)	// 13

// AI Mapping (DIO pin 14 to 19)
#define HC_AI_STARTINDEX		0						// 0
#define HC_AI_ENDINDEX			(HC_AI_QTY - 1)			// 7

// PWM Mapping
// ATmega8 : DIO pin 9, 10, 11
// Others : DIO pin 3, 5, 6, 9, 10, 11
#define HC_PIN_HAS_PWM(pin)	digitalPinHasPWM(pin)

// Servo
#define HC_SERVO_QTY_MAX		MAX_SERVOS				// 12

// If one servo is enabled, PWM on pins 9 and 10 are disabled
#define HC_PWM_IS_ENABLED(pin, servo_qty)  \
      ((servo_qty == 0) || \
      ((servo_qty > 0) && (pin != 9) && (pin != 10)))

// Serial  => pin 0 (RX) / 1 (TX), serial monitor

// SPI0    => pin 12 (MISO) / 11 (MOSI) / 13 (SCK) / 10 (SS)

// TWI0    => pin 18 (SDA) / 19 (SCL)


// Ethernet ***********************************************************************

#elif HC_VARIANT == HC_VARIANT_ETHERNET
// DIO quantity (includes AI)
#define HC_DIO_QTY				NUM_DIGITAL_PINS		// 20
// AI quantity 
#define HC_AI_QTY				NUM_ANALOG_INPUTS		// 6

// DIO Mapping
#define HC_DIO_STARTINDEX		0						// 0
#define HC_DIO_ENDINDEX			(HC_DIO_QTY - HC_AI_QTY - 1)	// 13

// AI Mapping (DIO pin 14 to 19)
#define HC_AI_STARTINDEX		0						// 0
#define HC_AI_ENDINDEX			(HC_AI_QTY - 1)			// 5

// PWM Mapping
// ATmega8 : DIO pin 9, 10, 11
// Others : DIO pin 3, 5, 6, 9, 10, 11
#define HC_PIN_HAS_PWM(pin)	digitalPinHasPWM(pin)

// Servo
#define HC_SERVO_QTY_MAX		MAX_SERVOS				// 12

// If one servo is enabled, PWM on pins 9 and 10 are disabled
#define HC_PWM_IS_ENABLED(pin, servo_qty)  \
      ((servo_qty == 0) || \
      ((servo_qty > 0) && (pin != 9) && (pin != 10)))

// Serial  => pin 0 (RX) / 1 (TX), serial monitor

// SPI0    => pin 12 (MISO) / 11 (MOSI) / 13 (SCK) / 10 (SS)

// TWI0    => pin 18 (SDA) / 19 (SCL)


// Gemma **************************************************************************

#elif HC_VARIANT == HC_VARIANT_GEMMA
// DIO quantity (includes AI)
#define HC_DIO_QTY				NUM_DIGITAL_PINS		// 3
// AI quantity 
#define HC_AI_QTY				NUM_ANALOG_INPUTS		// 1

// DIO Mapping
#define HC_DIO_STARTINDEX		0						// 0
#define HC_DIO_ENDINDEX			2						// 2

// AI Mapping (DIO pin 2)
#define HC_AI_STARTINDEX		0						// 0
#define HC_AI_ENDINDEX			0						// 0

// PWM Mapping (DIO pin 0, 1)
#define HC_PIN_HAS_PWM(pin)	    ((pin) == 0 || (pin) == 1)

// Servo
#define HC_SERVO_QTY_MAX		MAX_SERVOS				// 12

// If one servo is enabled, unknown behavior
#define HC_PWM_IS_ENABLED(pin, servo_qty)  false

// TWI0    => pin 0 (SDA) / 1 (SCL)


// Leonardo, Leonardo Ethernet, Esplora, Lilypad Usb ******************************

#elif HC_VARIANT == HC_VARIANT_LEONARDO
// DIO quantity (includes AI)
#define HC_DIO_QTY				NUM_DIGITAL_PINS		// 31
// AI quantity 
#define HC_AI_QTY				NUM_ANALOG_INPUTS		// 12

// DIO Mapping
#define HC_DIO_STARTINDEX		0						// 0
#define HC_DIO_ENDINDEX			13						// 13

// AI Mapping (DIO pin 18 to 29)
#define HC_AI_STARTINDEX		0						// 0
#define HC_AI_ENDINDEX			(HC_AI_QTY - 1)			// 11

// PWM Mapping (DIO pin 3, 5, 6, 9, 10, 11, 13)
#define HC_PIN_HAS_PWM(pin)	digitalPinHasPWM(pin)

// Servo
#define HC_SERVO_QTY_MAX		MAX_SERVOS				// 12

// If one servo is enabled, PWM on pins 9 and 10 are disabled
#define HC_PWM_IS_ENABLED(pin, servo_qty)  \
      ((servo_qty == 0) || \
      ((servo_qty > 0) && (pin != 9) && (pin != 10)))

// Serial  => USB, serial monitor
// Serial1 => pin 0 (RX) / 1 (TX)

// SPI0    => pin 14 (MISO) / 16 (MOSI) / 15 (SCK) / 17 (SS)

// TWI0    => pin 2 (SDA) / 3 (SCL)


// Mega 2560, Mega, Mega Adk ******************************************************

#elif HC_VARIANT == HC_VARIANT_MEGA
// DIO quantity (includes AI)
#define HC_DIO_QTY				NUM_DIGITAL_PINS		// 70
// AI quantity 
#define HC_AI_QTY				NUM_ANALOG_INPUTS		// 16

// DIO Mapping
#define HC_DIO_STARTINDEX		0						// 0
#define HC_DIO_ENDINDEX			(HC_DIO_QTY - HC_AI_QTY - 1)	// 53

// AI Mapping (DIO pin 54 to 69)
#define HC_AI_STARTINDEX		0						// 0
#define HC_AI_ENDINDEX			(HC_AI_QTY - 1)			// 15

// PWM Mapping (DIO pin 2 to 13 and 44 to 46)
#define HC_PIN_HAS_PWM(pin)	digitalPinHasPWM(pin)

// Servo
#define HC_SERVO_QTY_MAX		MAX_SERVOS				// 48
// If 0        servos are enabled, all PWM are enabled
// If 1  to 12 servos are enabled, PWM on pins                           44, 45, 46 are disabled (timer 5)
// If 13 to 24 servos are enabled, PWM on pins                   11, 12, 44, 45, 46 are disabled (timers 5 + 1)
// If 24 to 36 servos are enabled, PWM on pins 2, 3, 5,          11, 12, 44, 45, 46 are disabled (timers 5 + 1 + 3)
// If 37 to 48 servos are enabled, PWM on pins 2, 3, 5, 6, 7, 8, 11, 12, 44, 45, 46 are disabled (timers 5 + 1 + 3 + 4)
#define HC_PWM_IS_ENABLED(pin, servo_qty)	\
			((servo_qty == 0) || \
			((servo_qty > 0)  && (servo_qty <= 12) &&                                                                                                                   (pin != 44) && (pin != 45) && (pin != 46)) || \
			((servo_qty > 12) && (servo_qty <= 24) &&                                                                                     (pin != 11) && (pin != 12) && (pin != 44) && (pin != 45) && (pin != 46)) || \
			((servo_qty > 24) && (servo_qty <= 36) && (pin != 2) && (pin != 3) && (pin != 5) &&                                           (pin != 11) && (pin != 12) && (pin != 44) && (pin != 45) && (pin != 46)) || \
			((servo_qty > 36) && (servo_qty <= 48) && (pin != 2) && (pin != 3) && (pin != 5) && (pin != 6) && (pin != 7) && (pin != 8) && (pin != 11) && (pin != 12) && (pin != 44) && (pin != 45) && (pin != 46))) \

// Serial  => pin 0 (RX) / 1 (TX), serial monitor
// Serial1 => pin 19 (RX) / 18 (TX)
// Serial2 => pin 17 (RX) / 16 (TX)
// Serial3 => pin 15 (RX) / 14 (TX)

// SPI0    => pin 50 (MISO) / 51 (MOSI) / 52 (SCK) / 53 (SS)

// TWI0    => pin 20 (SDA) / 21 (SCL)


// Micro **************************************************************************

#elif HC_VARIANT == HC_VARIANT_MICRO
// DIO quantity (includes AI)
#define HC_DIO_QTY				NUM_DIGITAL_PINS		// 31
// AI quantity 
#define HC_AI_QTY				NUM_ANALOG_INPUTS		// 12

// DIO Mapping
#define HC_DIO_STARTINDEX		0						// 0
#define HC_DIO_ENDINDEX			13						// 13

// AI Mapping (DIO pin 18 to 29) 
#define HC_AI_STARTINDEX		0						// 0
#define HC_AI_ENDINDEX			(HC_AI_QTY - 1)			// 11

// PWM Mapping (DIO pin 3, 5, 6, 9, 10, 11, 13)
#define HC_PIN_HAS_PWM(pin)	digitalPinHasPWM(pin)

// Servo
#define HC_SERVO_QTY_MAX		MAX_SERVOS				// 12
// If one servo is enabled, PWM on pins 9 and 10 are disabled
#define HC_PWM_IS_ENABLED(pin, servo_qty)  \
      ((servo_qty == 0) || \
      ((servo_qty > 0) && (pin != 9) && (pin != 10)))

// Serial  => USB, serial monitor
// Serial1 => pin 0 (RX) / 1 (TX)

// SPI0    => pin 14 (MISO) / 16 (MOSI) / 15 (SCK) / 17 (SS)

// TWI0    => pin 2 (SDA) / 3 (SCL)


// Robot Control ******************************************************************

#elif HC_VARIANT == HC_VARIANT_ROBOTCONTROL
// DIO quantity (includes AI)
#define HC_DIO_QTY				14		                // 14
// AI quantity 
#define HC_AI_QTY				12		                // 12

// DIO Mapping
#define HC_DIO_STARTINDEX		0						// 0
#define HC_DIO_ENDINDEX			13						// 13

// AI Mapping
// (AO to A5 : DIO pin 18 to 23)
// (A6 to A11 : DIO pin 4, 6, 8, 9, 10, 12)
#define HC_AI_STARTINDEX		0						// 0
#define HC_AI_ENDINDEX			(HC_AI_QTY - 1)			// 11

// PWM Mapping (DIO pin 3, 5, 6, 9, 10, 11, 13)
#define HC_PIN_HAS_PWM(pin)     ((pin) == 3 || (pin) == 5 || (pin) == 6 || (pin) == 9 || (pin) == 10 || (pin) == 11 || (pin) == 13)

// Servo
#define HC_SERVO_QTY_MAX		MAX_SERVOS				// 12

// If one servo is enabled, PWM on pins 9 and 10 are disabled
#define HC_PWM_IS_ENABLED(pin, servo_qty)  \
      ((servo_qty == 0) || \
      ((servo_qty > 0) && (pin != 9) && (pin != 10)))

// Serial  => USB, serial monitor
// Serial1 => pin 0 (RX) / 1 (TX)

// SPI0    => pin 14 (MISO) / 16 (MOSI) / 15 (SCK) / 17 (SS)

// TWI0    => pin 2 (SDA) / 3 (SCL)


// Robot Motor ********************************************************************

#elif HC_VARIANT == HC_VARIANT_ROBOTMOTOR
// DIO quantity (includes AI)
#define HC_DIO_QTY				14		                // 14
// AI quantity 
#define HC_AI_QTY				12		                // 12

// DIO Mapping
#define HC_DIO_STARTINDEX		0						// 0
#define HC_DIO_ENDINDEX			13						// 13

// AI Mapping
// (AO to A5 : DIO pin 18 to 23)
// (A6 to A11 : DIO pin 4, 6, 8, 9, 10, 12)
#define HC_AI_STARTINDEX		0						// 0
#define HC_AI_ENDINDEX			(HC_AI_QTY - 1)			// 11

// PWM Mapping (DIO pin 3, 5, 6, 9, 10, 11, 13)
#define HC_PIN_HAS_PWM(pin)     ((pin) == 3 || (pin) == 5 || (pin) == 6 || (pin) == 9 || (pin) == 10 || (pin) == 11 || (pin) == 13)

// Servo
#define HC_SERVO_QTY_MAX		MAX_SERVOS				// 12

// If one servo is enabled, PWM on pins 9 and 10 are disabled
#define HC_PWM_IS_ENABLED(pin, servo_qty)  \
      ((servo_qty == 0) || \
      ((servo_qty > 0) && (pin != 9) && (pin != 10)))

// Serial  => USB, serial monitor
// Serial1 => pin 0 (RX) / 1 (TX)

// SPI0    => pin 14 (MISO) / 16 (MOSI) / 15 (SCK) / 17 (SS)

// TWI0    => pin 2 (SDA) / 3 (SCL)


// Uno, Duemilanove, Diecimila, Lilypad, NG (or older), Uno Wifi ******************

#elif HC_VARIANT == HC_VARIANT_STANDARD
// DIO quantity (includes AI)
#define HC_DIO_QTY				NUM_DIGITAL_PINS		// 20
// AI quantity 
#define HC_AI_QTY				NUM_ANALOG_INPUTS		// 6

// DIO Mapping
#define HC_DIO_STARTINDEX		0						// 0
#define HC_DIO_ENDINDEX			(HC_DIO_QTY - HC_AI_QTY - 1)	// 13

// AI Mapping (DIO pin 14 to 19)
#define HC_AI_STARTINDEX		0						// 0
#define HC_AI_ENDINDEX			(HC_AI_QTY - 1)			// 5

// PWM Mapping
// ATmega8 : DIO pin 9, 10, 11
// Others : DIO pin 3, 5, 6, 9, 10, 11
#define HC_PIN_HAS_PWM(pin)	digitalPinHasPWM(pin)

// Servo
#define HC_SERVO_QTY_MAX		MAX_SERVOS				// 12

// If one servo is enabled, PWM on pins 9 and 10 are disabled
#define HC_PWM_IS_ENABLED(pin, servo_qty)  \
      ((servo_qty == 0) || \
      ((servo_qty > 0) && (pin != 9) && (pin != 10)))

// Serial  => pin 0 (RX) / 1 (TX), serial monitor

// SPI0    => pin 12 (MISO) / 11 (MOSI) / 13 (SCK) / 10 (SS)

// TWI0    => pin 18 (SDA) / 19 (SCL)


// Yun, Yun Mini, Industrial 101, Linino One **************************************

#elif HC_VARIANT == HC_VARIANT_YUN
// DIO quantity (includes AI)
#define HC_DIO_QTY				NUM_DIGITAL_PINS		// 31
// AI quantity 
#define HC_AI_QTY				NUM_ANALOG_INPUTS		// 12

// DIO Mapping
#define HC_DIO_STARTINDEX		0						// 0
#define HC_DIO_ENDINDEX			13						// 13

// AI Mapping (DIO pin 18 to 29)
#define HC_AI_STARTINDEX		0						// 0
#define HC_AI_ENDINDEX			(HC_AI_QTY - 1)			// 11

// PWM Mapping (DIO pin 3, 5, 6, 9, 10, 11, 13)
#define HC_PIN_HAS_PWM(pin)	digitalPinHasPWM(pin)

// Servo
#define HC_SERVO_QTY_MAX		MAX_SERVOS				// 12

// If one servo is enabled, PWM on pins 9 and 10 are disabled
#define HC_PWM_IS_ENABLED(pin, servo_qty)  \
      ((servo_qty == 0) || \
      ((servo_qty > 0) && (pin != 9) && (pin != 10)))

// Serial  => USB, serial monitor
// Serial1 => Linux system, pin 0 (RX) / 1 (TX)

// SPI0    => pin 14 (MISO) / 16 (MOSI) / 15 (SCK) / 17 (SS)

// TWI0    => pin 2 (SDA) / 3 (SCL)


// Uno Wifi Rev2 ********************************************************************

#elif HC_VARIANT == HC_VARIANT_UNO2018
// DIO quantity (includes AI)
#define HC_DIO_QTY				NUM_DIGITAL_PINS		// 20
// AI quantity 
#define HC_AI_QTY				NUM_ANALOG_INPUTS		// 6

// DIO Mapping
#define HC_DIO_STARTINDEX		0						// 0
#define HC_DIO_ENDINDEX			(HC_DIO_QTY - HC_AI_QTY - 1)	// 13

// AI Mapping (DIO pin 14 to 19)
#define HC_AI_STARTINDEX		0						// 0
#define HC_AI_ENDINDEX			(HC_AI_QTY - 1)			// 5

// PWM Mapping
// DIO pin 3, 5, 6, 9, 10
#define HC_PIN_HAS_PWM(pin)		digitalPinHasPWM(pin)

// Servo
#define HC_SERVO_QTY_MAX		MAX_SERVOS				// 12

// If one or more servos are enabled, no PWM pins are disabled
#define HC_PWM_IS_ENABLED(pin, servo_qty)  true

// Serial  => USB, serial monitor
// Serial1 => pin 0 (RX) / 1 (TX)
// Serial2 => Wifi module (Nina-W102)
// Serial3 => Debug (via EDBG virtual COM port)

// SPI0    => pin 33 (MISO) / 32 (MOSI) / 34 (SCK) / 22 (SS)

// TWI0    => pin 20 (SDA) / 21 (SCL)


// Nano Every ***********************************************************************

#elif HC_VARIANT == HC_VARIANT_NONA4809
// DIO quantity (includes AI)
#define HC_DIO_QTY				NUM_DIGITAL_PINS	    // 22
// AI quantity 
#define HC_AI_QTY				(NUM_ANALOG_INPUTS - 6) // 14 - 6 = 8

// DIO Mapping
#define HC_DIO_STARTINDEX		0						// 0
#define HC_DIO_ENDINDEX			(HC_DIO_QTY - HC_AI_QTY - 1)	// 13

// AI Mapping (DIO pin 14 to 19)
#define HC_AI_STARTINDEX		0						// 0
#define HC_AI_ENDINDEX			(HC_AI_QTY - 1)		    // 7

// PWM Mapping
// DIO pin 3, 5, 6, 9, 10
#define HC_PIN_HAS_PWM(pin)		digitalPinHasPWM(pin)

// Servo
#define HC_SERVO_QTY_MAX		MAX_SERVOS				// 12

// If one or more servos are enabled, no PWM pins are disabled
#define HC_PWM_IS_ENABLED(pin, servo_qty)  true

// Serial  => USB, serial monitor
// Serial1 => pin 0 (RX) / 1 (TX)
// Serial3 => Debug (via EDBG virtual COM port)

// SPI0    => pin 12 (MISO) / 11 (MOSI) / 13 (SCK) / 8 (SS)

// TWI0    => pin 22 (SDA) / 23 (SCL)


// Zero ***************************************************************************

#elif HC_VARIANT == HC_VARIANT_ZERO
// DIO quantity (includes AI)
#define HC_DIO_QTY				NUM_DIGITAL_PINS	// 20
// AI quantity 
#define HC_AI_QTY				NUM_ANALOG_INPUTS	// 6

// DIO Mapping
#define HC_DIO_STARTINDEX		0					// 0
#define HC_DIO_ENDINDEX			(HC_DIO_QTY - HC_AI_QTY - 1)	// 13

// AI Mapping (DIO pin 14 to 19)
#define HC_AI_STARTINDEX		0					// 0
#define HC_AI_ENDINDEX			(HC_AI_QTY - 1)		// 5

// PWM Mapping (DIO pin 3 to 13 + 15, 16, 38, 39, 40, 41)
#define HC_PIN_HAS_PWM(pin)		digitalPinHasPWM(pin)

// Servo
#define HC_SERVO_QTY_MAX		MAX_SERVOS			// 12

// If one servo is enabled, PWM on pin 0 and 1 are disabled
#define HC_PWM_IS_ENABLED(pin, servo_qty)  \
		((servo_qty == 0) || \
		((servo_qty > 0) && (pin != 0) && (pin != 1)))

// SerialUSB => virtual serial port (native USB port, connected to SAMD21 mcu)
// Serial  => serial monitor (programming port, connected to EDBG usb-to-serial converter)
// Serial1 => pin 0 (RX) / 1 (TX)

// SPI0    => pin 22 (MISO) / 23 (MOSI) / 24 (SCK)

// TWI0    => pin 20 (SDA) / 21 (SCL)

// I2S0    => pin 9 (SD) / 0 (FS) / 1 (SCK)
	  

// MKR ZERO ****************************************************************************

#elif HC_VARIANT == HC_VARIANT_MKRZERO
// DIO quantity (includes AI)
#define HC_DIO_QTY				NUM_DIGITAL_PINS	// 15
// AI quantity 
#define HC_AI_QTY				NUM_ANALOG_INPUTS	// 7

// DIO Mapping
#define HC_DIO_STARTINDEX		0					// 0
#define HC_DIO_ENDINDEX			(HC_DIO_QTY - 1)	// 14

// AI Mapping (DIO pin 15 to 20)
#define HC_AI_STARTINDEX		0					// 0
#define HC_AI_ENDINDEX			(HC_AI_QTY - 1)		// 6

// PWM Mapping (DIO pin 0 to 8 + 10, 18, 19, 33)
#define HC_PIN_HAS_PWM(pin)  digitalPinHasPWM(pin)

// Servo
#define HC_SERVO_QTY_MAX		MAX_SERVOS			// 12

// If one servo is enabled, PWM on pin 0 and 1 are disabled
#define HC_PWM_IS_ENABLED(pin, servo_qty)  \
		((servo_qty == 0) || \
		((servo_qty > 0) && (pin != 0) && (pin != 1)))

// Serial  => USB, serial monitor
// Serial1 => pin 13 (RX) / 14 (TX)
 
// SPI0    => pin 10 (MISO) / 8 (MOSI) / 9 (SCK)
// SPI1    => connected to SD
 
// TWI0    => pin 11 (SDA) / 12 (SCL)

// I2S0    => pin 21 (SD) / 3 (FS) / 2 (SCK)

	  
// MKR 1000 ****************************************************************************

#elif HC_VARIANT == HC_VARIANT_MKR1000
// DIO quantity (includes AI)
#define HC_DIO_QTY				NUM_DIGITAL_PINS		// 22
// AI quantity 
#define HC_AI_QTY				NUM_ANALOG_INPUTS		// 7

// DIO Mapping
#define HC_DIO_STARTINDEX		0						// 0
#define HC_DIO_ENDINDEX			(HC_DIO_QTY - HC_AI_QTY - 1)	// 14

// AI Mapping (DIO pin 15 to 20)
#define HC_AI_STARTINDEX		0						// 0
#define HC_AI_ENDINDEX			(HC_AI_QTY - 1)			// 6

// PWM Mapping (DIO pin 0 to 8 + 10, 11, 18, 19)
#define HC_PIN_HAS_PWM(pin)  digitalPinHasPWM(pin)

// Servo
#define HC_SERVO_QTY_MAX		MAX_SERVOS			// 12

// If one servo is enabled, PWM on pin 0 and 1 are disabled
#define HC_PWM_IS_ENABLED(pin, servo_qty)  \
		((servo_qty == 0) || \
		((servo_qty > 0) && (pin != 0) && (pin != 1)))

// Serial  => USB, serial monitor
// Serial1 => pin 13 (RX) / 14 (TX)
 
// SPI0    => pin 10 (MISO) / 8 (MOSI) / 9 (SCK)
// SPI1    => WINC1501B module

// TWI0    => pin 11 (SDA) / 12 (SCL)
 
// I2S0    => pin 21 (SD) / 3 (FS) / 2 (SCK)
 

// MKR WIFI 1010 *******************************************************************

#elif HC_VARIANT == HC_VARIANT_MKR1010
// DIO quantity (includes AI)
#define HC_DIO_QTY				NUM_DIGITAL_PINS		// 15
// AI quantity 
#define HC_AI_QTY				NUM_ANALOG_INPUTS		// 7

// DIO Mapping
#define HC_DIO_STARTINDEX		0						// 0
#define HC_DIO_ENDINDEX			(HC_DIO_QTY - 1)	    // 14

// AI Mapping (DIO pin 15 to 20)
#define HC_AI_STARTINDEX		0						// 0
#define HC_AI_ENDINDEX			(HC_AI_QTY - 1)			// 6

// PWM Mapping (DIO pin 0 to 8 + 10, 18, 19, 32)
#define HC_PIN_HAS_PWM(pin)		digitalPinHasPWM(pin)

// Servo
#define HC_SERVO_QTY_MAX		MAX_SERVOS				// 12

// If one servo is enabled, PWM on pin 0 and 1 are disabled
#define HC_PWM_IS_ENABLED(pin, servo_qty)  \
		((servo_qty == 0) || \
		((servo_qty > 0) && (pin != 0) && (pin != 1)))

// Serial  => USB, serial monitor
// Serial1 => pin 13 (RX) / 14 (TX)

// SPI0    => pin 10 (MISO) / 8 (MOSI) / 9 (SCK)
// SPI1    => Wifi module (Nina-W102)

// TWI0    => pin 11 (SDA) / 12 (SCL)

// I2S0    => pin 21 (SD) / 3 (FS) / 2 (SCK)


// MKR FOX 1200 ***************************************************************************

#elif HC_VARIANT == HC_VARIANT_MKR1200
// DIO quantity (includes AI)
#define HC_DIO_QTY				NUM_DIGITAL_PINS	// 15
// AI quantity 
#define HC_AI_QTY				NUM_ANALOG_INPUTS	// 7

// DIO Mapping
#define HC_DIO_STARTINDEX		0					// 0
#define HC_DIO_ENDINDEX			(HC_DIO_QTY - 1)	// 14

// AI Mapping (DIO pin 15 to 20)
#define HC_AI_STARTINDEX		0					// 0
#define HC_AI_ENDINDEX			(HC_AI_QTY - 1)		// 6

// PWM Mapping (DIO pin 0 to 8 + 10, 18, 19, 33)
#define HC_PIN_HAS_PWM(pin)		digitalPinHasPWM(pin)

// Servo
#define HC_SERVO_QTY_MAX		MAX_SERVOS			// 12

// If one servo is enabled, PWM on pin 0 and 1 are disabled
#define HC_PWM_IS_ENABLED(pin, servo_qty)  \
		((servo_qty == 0) || \
		((servo_qty > 0) && (pin != 0) && (pin != 1)))

// Serial  => USB, serial monitor
// Serial1 => pin 13 (RX) / 14 (TX)

// SPI0    => pin 10 (MISO) / 8 (MOSI) / 9 (SCK)
// SPI1    => Sigfox module

// TWI0    => pin 11 (SDA) / 12 (SCL)

// I2S0    => pin 21 (SD) / 3 (FS) / 2 (SCK)
 

// MKR WAN 1300 ***************************************************************************

#elif HC_VARIANT == HC_VARIANT_MKR1300
// DIO quantity (includes AI)
#define HC_DIO_QTY				NUM_DIGITAL_PINS	// 15
// AI quantity 
#define HC_AI_QTY				NUM_ANALOG_INPUTS	// 7

// DIO Mapping
#define HC_DIO_STARTINDEX		0					// 0
#define HC_DIO_ENDINDEX			(HC_DIO_QTY - 1)	// 14

// AI Mapping (DIO pin 15 to 20)
#define HC_AI_STARTINDEX		0					// 0
#define HC_AI_ENDINDEX			(HC_AI_QTY - 1)		// 6

// PWM Mapping (DIO pin 0 to 8 + 10, 18, 19, 33)
#define HC_PIN_HAS_PWM(pin)		digitalPinHasPWM(pin)

// Servo
#define HC_SERVO_QTY_MAX		MAX_SERVOS			// 12

// If one servo is enabled, PWM on pin 0 and 1 are disabled
#define HC_PWM_IS_ENABLED(pin, servo_qty)  \
		((servo_qty == 0) || \
		((servo_qty > 0) && (pin != 0) && (pin != 1)))

// Serial  => USB, serial monitor
// Serial1 => pin 13 (RX) / 14 (TX)
// Serial2 => Lora module

// SPI0    => pin 10 (MISO) / 8 (MOSI) / 9 (SCK)

// TWI0    => pin 11 (SDA) / 12 (SCL)

// I2S0    => pin 21 (SD) / 3 (FS) / 2 (SCK)


// MKR GSM 1400 ***************************************************************************

#elif HC_VARIANT == HC_VARIANT_MKR1400
// DIO quantity (includes AI)
#define HC_DIO_QTY				NUM_DIGITAL_PINS	// 15
// AI quantity 
#define HC_AI_QTY				NUM_ANALOG_INPUTS	// 7

// DIO Mapping
#define HC_DIO_STARTINDEX		0					// 0
#define HC_DIO_ENDINDEX			(HC_DIO_QTY - 1)	// 14

// AI Mapping (DIO pin 15 to 20)
#define HC_AI_STARTINDEX		0					// 0
#define HC_AI_ENDINDEX			(HC_AI_QTY - 1)		// 6

// PWM Mapping (DIO pin 0 to 8 + 10, 18, 19, 32)
#define HC_PIN_HAS_PWM(pin)		digitalPinHasPWM(pin)

// Servo
#define HC_SERVO_QTY_MAX		MAX_SERVOS			// 12

// If one servo is enabled, PWM on pin 0 and 1 are disabled
#define HC_PWM_IS_ENABLED(pin, servo_qty)  \
		((servo_qty == 0) || \
		((servo_qty > 0) && (pin != 0) && (pin != 1)))

// Serial  => USB, serial monitor
// Serial1 => pin 13 (RX) / 14 (TX)
// Serial2 => GSM module (Sara)

// SPI0    => pin 10 (MISO) / 8 (MOSI) / 9 (SCK)

// TWI0    => pin 11 (SDA) / 12 (SCL)

// I2S0    => pin 21 (SD) / 3 (FS) / 2 (SCK)


// MKR NB 1500 ***************************************************************************

#elif HC_VARIANT == HC_VARIANT_MKR1500
// DIO quantity (includes AI)
#define HC_DIO_QTY				NUM_DIGITAL_PINS	// 15
// AI quantity 
#define HC_AI_QTY				NUM_ANALOG_INPUTS	// 7

// DIO Mapping
#define HC_DIO_STARTINDEX		0					// 0
#define HC_DIO_ENDINDEX			(HC_DIO_QTY - 1)	// 14

// AI Mapping (DIO pin 15 to 20)
#define HC_AI_STARTINDEX		0					// 0
#define HC_AI_ENDINDEX			(HC_AI_QTY - 1)		// 6

// PWM Mapping (DIO pin 0 to 8 + 10, 18, 19, 32)
#define HC_PIN_HAS_PWM(pin)		digitalPinHasPWM(pin)

// Servo
#define HC_SERVO_QTY_MAX		MAX_SERVOS			// 12

// If one servo is enabled, PWM on pin 0 and 1 are disabled
#define HC_PWM_IS_ENABLED(pin, servo_qty)  \
		((servo_qty == 0) || \
		((servo_qty > 0) && (pin != 0) && (pin != 1)))

// Serial  => USB, serial monitor
// Serial1 => pin 13 (RX) / 14 (TX)
// Serial2 => NB module (Sara)

// SPI0    => pin 10 (MISO) / 8 (MOSI) / 9 (SCK)

// TWI0    => pin 11 (SDA) / 12 (SCL)

// I2S0    => pin 21 (SD) / 3 (FS) / 2 (SCK)


// MKR Vidor 4000 ******************************************************************

#elif HC_VARIANT == HC_VARIANT_MKR4000
// DIO quantity (includes AI)
#define HC_DIO_QTY				NUM_DIGITAL_PINS	// 15
// AI quantity 
#define HC_AI_QTY				NUM_ANALOG_INPUTS	// 7

// DIO Mapping
#define HC_DIO_STARTINDEX		0					// 0
#define HC_DIO_ENDINDEX			(HC_DIO_QTY - 1)	// 14

// AI Mapping (DIO pin 15 to 20)
#define HC_AI_STARTINDEX		0					// 0
#define HC_AI_ENDINDEX			(HC_AI_QTY - 1)		// 6

// PWM Mapping (DIO pin 0 to 8 + 10, 18, 19, 32)
#define HC_PIN_HAS_PWM(pin)		digitalPinHasPWM(pin)

// Servo
#define HC_SERVO_QTY_MAX		MAX_SERVOS			// 12

// If one servo is enabled, PWM on pin 0 and 1 are disabled
#define HC_PWM_IS_ENABLED(pin, servo_qty)  \
		((servo_qty == 0) || \
		((servo_qty > 0) && (pin != 0) && (pin != 1)))

// Serial  => USB, serial monitor
// Serial1 => pin 13 (RX) / 14 (TX)
// Serial2 => NB module (Sara)

// SPI0    => pin 10 (MISO) / 8 (MOSI) / 9 (SCK)

// TWI0    => pin 11 (SDA) / 12 (SCL)

// I2S0    => pin 21 (SD) / 3 (FS) / 2 (SCK)


// Nano 33 IOT *******************************************************************

#elif HC_VARIANT == HC_VARIANT_NANO33IOT
// DIO quantity (includes AI)
#define HC_DIO_QTY				NUM_DIGITAL_PINS	// 31
// AI quantity 
#define HC_AI_QTY				NUM_ANALOG_INPUTS	// 8

// DIO Mapping
#define HC_DIO_STARTINDEX		0					// 0
#define HC_DIO_ENDINDEX			13					// 13

// AI Mapping (DIO pin 15 to 20)
#define HC_AI_STARTINDEX		0					// 0
#define HC_AI_ENDINDEX			(HC_AI_QTY - 1)		// 7

// PWM Mapping (DIO pin 2, 3, 5, 6, 9, 10, 11, 12, 16 (A2), 17 (A3), 19 (A5)
#define HC_PIN_HAS_PWM(pin)		digitalPinHasPWM(pin)

// Servo
#define HC_SERVO_QTY_MAX		MAX_SERVOS			// 12

// If one servo is enabled, PWM on pin 0 and 1 are disabled
#define HC_PWM_IS_ENABLED(pin, servo_qty)  \
		((servo_qty == 0) || \
		((servo_qty > 0) && (pin != 0) && (pin != 1)))

// Serial  => USB, serial monitor
// Serial1 => pin 0 (RX) / 1 (TX)
// Serial2 => NB module (Sara)

// SPI0    => pin 12 (MISO) / 11 (MOSI) / 13 (SCK)

// TWI0    => pin 18 (SDA) / 19 (SCL)

// I2S0    => pin 4 (SD) / 16 (FS) / 17 (SCK)


// Due ****************************************************************************

#elif HC_VARIANT == HC_VARIANT_DUE
// DIO quantity (includes AI)
#define HC_DIO_QTY				NUM_DIGITAL_PINS		// 66
// AI quantity 
#define HC_AI_QTY				NUM_ANALOG_INPUTS		// 12

// DIO Mapping
#define HC_DIO_STARTINDEX		0						// 0
#define HC_DIO_ENDINDEX			(HC_DIO_QTY - HC_AI_QTY - 1)	// 53

// AI Mapping
#define HC_AI_STARTINDEX		0						// 0
#define HC_AI_ENDINDEX			(HC_AI_QTY - 1)			// 11

// PWM Mapping
#define HC_PIN_HAS_PWM(pin)     digitalPinHasPWM(pin)

// Servo
#define HC_SERVO_QTY_MAX		MAX_SERVOS				// 60

// If 0        servos are enabled, all PWM are enabled
// If 1  to 12 servos are enabled, all PWM are enabled					(timer1-ch0)
// If 13 to 24 servos are enabled, all PWM are enabled					(timer1-ch0 + timer1-ch1)
// If 24 to 36 servos are enabled, all PWM are enabled					(timer1-ch0 + timer1-ch1 + timer1-ch2)
// If 37 to 48 servos are enabled, all PWM are enabled					(timer1-ch0 + timer1-ch1 + timer1-ch2 + timer0-ch2)
// If 49 to 60 servos are enabled, PWM on pins 2 and 13 are disabled	(timer1-ch0 + timer1-ch1 + timer1-ch2 + timer0-ch2 + timer0-ch0)
#define HC_PWM_IS_ENABLED(pin, servo_qty)	\
			(((servo_qty >= 0)  && (servo_qty <= 48))	||	\
			 ((servo_qty > 48)  && (servo_qty <= 60)  && (pin != 2) && (pin != 13)))	\

// SerialUSB => virtual serial port (native USB port, connected to SAM3X mcu)
// Serial  => pin 0 (RX) / 1 (TX), serial monitor (programming port, connected to Atmega16u2 usb-to-serial converter)
// Serial1 => pin 19 (RX) / 18 (TX)
// Serial2 => pin 17 (RX) / 16 (TX)
// Serial3 => pin 15 (RX) / 14 (TX)

// SPI0    => pin 22 (MISO) / 23 (MOSI) / 24 (SCK)

// TWI0    => pin 20 (SDA) / 21 (SCL)

// I2S0    => pin 9 (SD) / 0 (FS) / 1 (SCK)


// Unknown ************************************************************************
#else
// DIO quantity (includes AI)
#define HC_DIO_QTY			NUM_DIGITAL_PINS			// 15
// AI quantity 
#define HC_AI_QTY			NUM_ANALOG_INPUTS			// 7

// DIO Mapping
#define HC_DIO_STARTINDEX		0						// 0
#define HC_DIO_ENDINDEX			(HC_DIO_QTY - 1)	    // 14

// AI Mapping (DIO pin 14 to 19)
#define HC_AI_STARTINDEX		0						// 0
#define HC_AI_ENDINDEX			(HC_AI_QTY - 1)			// 6

// PWM Mapping
#define HC_PIN_HAS_PWM(pin)	digitalPinHasPWM(pin)

// Servo
#define HC_SERVO_QTY_MAX		MAX_SERVOS				// 12

// If one servo is enabled, PWM on pins 9 and 10 are disabled
#define HC_PWM_IS_ENABLED(pin, servo_qty)  \
      ((servo_qty == 0) || \
      ((servo_qty > 0) && (pin != 9) && (pin != 10)))

#endif



// *****************************************************************************
// Getters
// *****************************************************************************

uint8_t HCS_getDIO_qty()           { return HC_DIO_QTY; }
uint8_t HCS_getAI_qty()            { return HC_AI_QTY; }

uint8_t HCS_getDIO_startIndex()    { return HC_DIO_STARTINDEX; }
uint8_t HCS_getDIO_endIndex()      { return HC_DIO_ENDINDEX; }

uint8_t HCS_getAI_startIndex()     { return HC_AI_STARTINDEX; }
uint8_t HCS_getAI_endIndex()       { return HC_AI_ENDINDEX; }

bool HCS_hasPWM(uint8_t pin)       { return HC_PIN_HAS_PWM(pin); }

uint8_t HCS_getServo_qty()         { return HC_SERVO_QTY_MAX; }

bool HCS_isPWMEnabled(uint8_t pin, uint8_t servo_qty) { return HC_PWM_IS_ENABLED(pin, servo_qty); }

#ifdef ARDUINO_ARCH_SAMD
uint8_t HCS_getDacPin(uint8_t index)  { return (index == 0) ? A0 : 0; }
#endif
