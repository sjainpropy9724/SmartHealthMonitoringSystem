/*
 * HITICommSupport
 * HCS_ServoInterface.cpp
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


#include "HCS_ServoInterface.h"



// *****************************************************************************
// Include dependencies
// *****************************************************************************

#include <Servo.h>



// *****************************************************************************
// Global variables
// *****************************************************************************

// Servo Structure array (pointer)
servo_struct _servo_map[MAX_SERVOS];



// *****************************************************************************
// Class Methods
// *****************************************************************************


	// constructor ********************************************************
	
	HCS_ServoInterface::HCS_ServoInterface(bool enableServoManagement):
			mServo(NULL_POINTER),
			mEnableServoManagement(enableServoManagement)	// Enable/Disable Servo Management by HITIComm
	{
		instanciateServo();
	};

	
	// destructor *********************************************************
	
	HCS_ServoInterface::~HCS_ServoInterface()
	{
		clearServo();
	}	
		
		
	// getter *************************************************************
	
	Servo* HCS_ServoInterface::getServo() const					{ return mServo; }

	// in us
	unsigned int HCS_ServoInterface::getMinPulseWidth() const	{ return mMinPulseWidth; }
	unsigned int HCS_ServoInterface::getMaxPulseWidth() const	{ return mMaxPulseWidth; }
		
	bool HCS_ServoInterface::attached() const { return (mEnableServoManagement && (mServo != NULL_POINTER)) ? mServo->attached() : false; }


	// dynamic allocation *************************************************

	void HCS_ServoInterface::instanciateServo()
	{
		clearServo();

		if (mEnableServoManagement)
		{
			mServo = new Servo();
			mDynamicallyAllocated = true;
		}
	}

	void HCS_ServoInterface::clearServo()
	{
		// if dynamically allocated by HC_ServoInterface: clean memory
		if((mServo != NULL_POINTER) && mDynamicallyAllocated)
		{
			delete mServo;
			mServo = NULL_POINTER;
			mDynamicallyAllocated = false;
		}
	}


	// non-dynamic allocation *********************************************

	void HCS_ServoInterface::setServo(Servo* servo)
	{
		clearServo();

		mServo = (mEnableServoManagement) ? servo : NULL_POINTER;
	}


	// attach/detach ******************************************************
		
	uint8_t HCS_ServoInterface::attach(int pin)
	{
		return (mEnableServoManagement && (mServo != NULL_POINTER))? mServo->attach(pin) : 0;
	}
	
	uint8_t HCS_ServoInterface::attach(int pin, unsigned int min, unsigned int max)
	{
		// record min and max
		mMinPulseWidth = min;
		mMaxPulseWidth = max;
	
		return (mEnableServoManagement && (mServo != NULL_POINTER)) ? mServo->attach(pin, min, max) : 0;
	}
	
	void HCS_ServoInterface::detach()
	{
		if(mEnableServoManagement && (mServo != NULL_POINTER)) mServo->detach();
	}
	

	// read/write *********************************************************

	void HCS_ServoInterface::write(int value)
	{
		if(mEnableServoManagement && (mServo != NULL_POINTER)) mServo->write(value);
	}         
	
	void HCS_ServoInterface::writeMicroseconds(int value)
	{
		if(mEnableServoManagement && (mServo != NULL_POINTER)) mServo->writeMicroseconds(value);
	}
	
	int HCS_ServoInterface::read()
	{
		return (mEnableServoManagement && (mServo != NULL_POINTER)) ? mServo->read() : 0;
	}
	
	int HCS_ServoInterface::readMicroseconds()
	{
		return (mEnableServoManagement && (mServo != NULL_POINTER)) ? mServo->readMicroseconds() : 0;
	}
