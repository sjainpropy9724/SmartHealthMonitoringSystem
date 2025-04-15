/*
 * HITICommSupport
 * HCS_ServoInterface.h
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

#ifndef HCS_ServoManagerInterface_h
#define HCS_ServoManagerInterface_h



// *****************************************************************************
// Include dependencies
// *****************************************************************************

// HITICommSupport
#include "HITICommSupport.h"



// *****************************************************************************
// Forward declaration
// *****************************************************************************

class Servo; // because used as a pointer



// *****************************************************************************
// Class
// *****************************************************************************

class HCS_ServoInterface
{
	public:
		// constructor ****************************************************
		HCS_ServoInterface(bool enableServoManagement);
		
		// destructor *****************************************************
		~HCS_ServoInterface();
		
		// non-dynamic allocation *****************************************
		void setServo(Servo* servo);

		// getter *********************************************************
		Servo* getServo() const;
		unsigned int getMinPulseWidth() const; // in us
		unsigned int getMaxPulseWidth() const; // in us
		bool attached() const;
		
		// attach/detach **************************************************
		uint8_t attach(int pin);   
		uint8_t attach(int pin, unsigned int min, unsigned int max);
		void detach();

		// read/write *****************************************************
		void write(int value);          
		void writeMicroseconds(int value); 
		int read();
		int readMicroseconds();
		
	protected:
	
	private:
		// dynamic allocation *********************************************
		void instanciateServo();
		void clearServo();


		// variables ******************************************************
		Servo* mServo;						 // Servo (pointer to 1 Servo)
		
		bool mEnableServoManagement;		 // Flag (true if Servo Management by HC_ServoInterface is allowed. In this case, Servo dynamic allocation is performed by HC_ServoInterface)
		bool mDynamicallyAllocated = false;  // Flag (true if Servo has been dynamically allocated by HC_ServoInterface)

		unsigned int mMinPulseWidth = 544;   // Min and Max pulse width in microseconds
		unsigned int mMaxPulseWidth = 2400;  // Min and Max pulse width in microseconds
};



// *****************************************************************************
// Structure
// *****************************************************************************

// Servo Structure
// - pin field is initialized to 0
// - when a Servo is attached, pin field is updated
// - when a Servo is detached, pin field is reset to 0
struct servo_struct
{
	// Servo interface (pointer)
	HCS_ServoInterface* servo;

	// attached pin, if attached
	uint8_t pin = 0;
};


#endif
