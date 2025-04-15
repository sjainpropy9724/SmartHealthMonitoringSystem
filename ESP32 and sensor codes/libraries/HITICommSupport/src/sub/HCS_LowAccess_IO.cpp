/*
 * HITICommSupport
 * HCS_LowAccess_IO.cpp
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



#include "HCS_LowAccess_IO.h"



// *****************************************************************************
// Include dependencies
// *****************************************************************************

// Arduino
#include <Arduino.h>



// *****************************************************************************
// Define
// *****************************************************************************

// pin mode/output mode
#define MODE_UNKNOWN	0xA

// output mode
#define MODE_DIGITAL	0x0
#define MODE_PWM	    0x1



// *****************************************************************************
// variables
// *****************************************************************************


// Loop() cycle time -----------------------------------------------------------

// cycle time (in us)
static unsigned long g_cycleTime = 0;


// I/O Ports -------------------------------------------------------------------


#if defined ARDUINO_ARCH_AVR
// Values defined in Arduino.h
static uint8_t g_pin_port  = 0; // port  name   => Uno: PD (pin 0-7), PB (pin 8-13), PC (pin 14-19)
static uint8_t g_pin_mask  = 0; // bit   mask   => which bit controls the pin (ex: 0b00010000) on the I/O registers
static uint8_t g_pin_timer = 0; // timer name   => Uno: Timer0A, Timer0B, Timer1A, Timer1B, Timer2A, Timer2B
// I/O registers
static volatile uint8_t* g_pin_dir = 0; // DDR  register address   => direction
static volatile uint8_t* g_pin_out = 0; // PORT register address   => output control
static volatile uint8_t* g_pin_in  = 0; // PIN  register address   => input reading (also read the output)

#elif defined ARDUINO_ARCH_MEGAAVR
// Values defined in Arduino.h
static uint8_t g_pin_port  = 0; // port  name   => Uno: PD (pin 0-7), PB (pin 8-13), PC (pin 14-19)
static uint8_t g_pin_mask  = 0; // bit   mask   => which bit controls the pin (ex: 0b00010000) on the I/O registers
static uint8_t g_pin_timer = 0; // timer name   => Uno: Timer0A, Timer0B, Timer1A, Timer1B, Timer2A, Timer2B
static uint8_t g_pin_bit   = 0; // bit position => which bit controls the pin on the I/O registers
// I/O registers
static volatile uint8_t* g_pin_dir = 0; // DIR register address   => direction
static volatile uint8_t* g_pin_out = 0; // OUT register address   => output control
static volatile uint8_t* g_pin_in  = 0; // IN  register address   => input reading (also read the output)

#elif defined ARDUINO_ARCH_SAMD
// macro
#define pinConfigRegister(port, pin)    (&(port->PINCFG[g_APinDescription[pin].ulPin].reg))
// Values defined in Arduino.h and variant.h
static PortGroup* g_pin_port;    // port  group  => SAMD21G18: PORTA, PORTB, PORTC
static uint32_t g_pin_mask = 0;  // bit   mask   => which bit controls the pin (ex: 0b00010000) on the I/O registers
// I/O registers
static volatile uint32_t* g_pin_dir   = 0; // DIR    register address   => direction
static volatile uint32_t* g_pin_out   = 0; // OUT    register address   => output control
static volatile uint32_t* g_pin_in    = 0; // IN     register address   => input reading (also read the output)
static volatile uint8_t* g_pin_config = 0; // PINCFG register address   => config

// Analog IO resolution
static uint8_t g_PWM_Resolution = 8;
static uint8_t g_DAC_Resolution = 8;
static uint8_t g_ADC_Resolution = 8;

#elif defined ARDUINO_ARCH_SAM
// macro

// Values defined in Arduino.h and variant.h
static Pio* g_pin_port;          // port
static uint32_t g_pin_mask = 0;  // bit   mask   => which bit controls the pin (ex: 0b00010000) on the I/O registers
// I/O registers
static volatile uint32_t* g_pin_dir = 0; // DIR    register address   => direction
static volatile uint32_t* g_pin_out = 0; // OUT    register address   => output control
static volatile uint32_t* g_pin_in = 0; // IN     register address   => input reading (also read the output)
static volatile uint8_t* g_pin_config = 0; // PINCFG register address   => config

// Analog IO resolution
static uint8_t g_PWM_Resolution = 8;
static uint8_t g_DAC_Resolution = 8;
static uint8_t g_ADC_Resolution = 8;

#endif



// *****************************************************************************
// Methods
// *****************************************************************************


// --------------------------------------------------------------------------------
// Loop() Cycle Time --------------------------------------------------------------
// --------------------------------------------------------------------------------


// Calculate Cycle Time (us)
// must be called at each loop cycle => called by HC_communicate()
void HCS_calculateCycleTime()
{
	// get current time (in us)
	unsigned long currentTimestamp = micros();
	
    // previous timestamp (in us)
    static unsigned long g_previousTimestamp = 0;

	// calculate cycle time in us
	g_cycleTime = currentTimestamp - g_previousTimestamp;

	// record timestamp
	g_previousTimestamp = currentTimestamp;
}


// Get cycle time (us)
unsigned long HCS_getCycleTime()
{
	return g_cycleTime;
}



// -----------------------------------------------------------------------------
// I/O Ports -------------------------------------------------------------------
// -----------------------------------------------------------------------------


// Get pin data ************************************************************
bool getPinData(uint8_t pin)
{
#if defined ARDUINO_ARCH_AVR
    g_pin_port = digitalPinToPort(pin);         // port name

    // is it a valid pin?
    if(g_pin_port == NOT_A_PORT)
        return false;

    g_pin_mask  = digitalPinToBitMask(pin);     // bit mask
    g_pin_timer = digitalPinToTimer(pin);       // timer name

    g_pin_dir = portModeRegister(g_pin_port);   // DDR  register address
    g_pin_out = portOutputRegister(g_pin_port); // PORT register address
    g_pin_in  = portInputRegister(g_pin_port);  // PIN  register address
    return true;

#elif defined ARDUINO_ARCH_MEGAAVR
    g_pin_port = digitalPinToPort(pin);         // port name
    g_pin_bit  = digitalPinToBitPosition(pin);  // bit position

    // is it a valid pin?
    if ((g_pin_port == NOT_A_PORT) || (g_pin_bit == NOT_A_PIN))
        return false;

    g_pin_mask  = digitalPinToBitMask(pin);     // bit mask
    g_pin_timer = digitalPinToTimer(pin);       // timer name

    g_pin_dir = portModeRegister(g_pin_port);   // DIR register address
    g_pin_out = portOutputRegister(g_pin_port); // OUT register address
    g_pin_in  = portInputRegister(g_pin_port);  // IN  register address
    return true;

#elif defined ARDUINO_ARCH_SAMD
    g_pin_port = digitalPinToPort(pin);                // port name

    // is it a valid pin?
    if ((g_APinDescription[pin].ulPort == NOT_A_PORT) || (g_APinDescription[pin].ulPinType == PIO_NOT_A_PIN))
        return false;

    g_pin_mask  = digitalPinToBitMask(pin);            // bit mask
    //g_pin_timer = digitalPinToTimer(pin);            // timer name

    g_pin_dir    = portModeRegister(g_pin_port);       // DIR    register address
    g_pin_out    = portOutputRegister(g_pin_port);     // OUT    register address
    g_pin_in     = portInputRegister(g_pin_port);      // IN     register address
    g_pin_config = pinConfigRegister(g_pin_port, pin); // PINCFG register address

    return true;
/*
#elif defined ARDUINO_ARCH_SAM
    g_pin_port = digitalPinToPort(pin);                // port name

    // is it a valid pin?
    if ((g_APinDescription[pin].ulPort == NOT_A_PORT) || (g_APinDescription[pin].ulPinType == PIO_NOT_A_PIN))
        return false;

    g_pin_mask = digitalPinToBitMask(pin);            // bit mask
    //g_pin_timer = digitalPinToTimer(pin);            // timer name

    g_pin_dir = portModeRegister(g_pin_port);       // DIR    register address
    g_pin_out = portOutputRegister(g_pin_port);     // OUT    register address
    g_pin_in = portInputRegister(g_pin_port);      // IN     register address
    g_pin_config = pinConfigRegister(g_pin_port, pin); // PINCFG register address

    return true;*/
#else
    return false;
#endif
}


// Set pin mode ******************************************************
// - INPUT
// - OUTPUT
// - INPUT_PULLUP
// - INPUT_PULLDOWN
void HCS_pinMode(uint8_t pin, uint8_t mode)     { pinMode(pin, mode); }


// Get pin mode ******************************************************
// - INPUT
// - OUTPUT
// - INPUT_PULLUP
// - INPUT_PULLDOWN
// - MODE_UNKNOWN
uint8_t HCS_getPinMode(uint8_t pin)
{
#if defined ARDUINO_ARCH_AVR
    // get pin data
    // if UNKNOWN
    if (!getPinData(pin))
        return MODE_UNKNOWN;

    // if OUPUT
    else if (*g_pin_dir & g_pin_mask)
        return OUTPUT;

    // if INPUT with internal Pull-up AND if all Pull-ups are enabled
    else if ((*g_pin_out & g_pin_mask) && bit_is_clear(MCUCR, PUD))
        return INPUT_PULLUP;

    // if INPUT
    else
        return INPUT;

#elif defined ARDUINO_ARCH_MEGAAVR
    // get pin data
    // if UNKNOWN
    if (!getPinData(pin) || isDoubleBondedActive(pin))
        return MODE_UNKNOWN;

    // if OUPUT
    else if (*g_pin_dir & g_pin_mask)
        return OUTPUT;

    // if INPUT with internal Pull-up
    else if (((uint8_t)*getPINnCTRLregister(portToPortStruct(g_pin_port), g_pin_bit)) & PORT_PULLUPEN_bm)
        return INPUT_PULLUP;

    // if INPUT
    else
        return INPUT;

#elif defined ARDUINO_ARCH_SAMD
    // get pin data
    // if UNKNOWN
    if (!getPinData(pin))
        return MODE_UNKNOWN;

    // if INPUT buffer is activated
    if (*g_pin_config & (uint8_t)PORT_PINCFG_INEN)
    {
        // if INPUT with internal Pull-up or Pull-down
        if (*g_pin_config & (uint8_t)PORT_PINCFG_PULLEN)
            return (*g_pin_out & g_pin_mask) ? INPUT_PULLUP : INPUT_PULLDOWN;

        // if OUPUT
        else if (*g_pin_dir & g_pin_mask)
            return OUTPUT;

        // if INPUT
        else
            return INPUT;
    }
    else
        return MODE_UNKNOWN;

#else
    return MODE_UNKNOWN;
#endif
}



// read Output Mode ***************************************************
// - MODE_DIGITAL
// - MODE_PWM (For output values 0 AND 255, return MODE_DIGITAL)
// - MODE_UNKNOWN
uint8_t getOutputMode(uint8_t pin)
{
    // get pin data, read pin mode
    if (HCS_getPinMode(pin) != OUTPUT)
        return MODE_UNKNOWN;

    bool flag = false;	


#if defined ARDUINO_ARCH_AVR
    // PWM is controlled by a timer (n).
    // There are 2 or 3 Output Compare pin (r) per timer (A, B, C)
    // Output Compare register OCRnr holds the duty cycle value
    // TCCRnA and TCCRnB => control the timer behavior:
    //   - WGMnx             - 3 bits                        => control Wave Generation Mode of the timer (normal, ctc, fast pwm, phase correct pwm)
    //   - COMnAx and COMnBx - 2 bits per Output Compare pin => control Compare Output Mode
    //
    // For Uno (see wiring.c)
    //   - Timer0 is set as fast pwm (WGM = B011) and Timer1 and 2 as phase correct pwm (WGM = B001)
    //   - COMnr0 = 0
    //   - COMnr1 enable pwm
    //
    // analogWrite() works that way:
    // - for value 0 and 255: call to digitalWrite(), COMnr1 is reset
    // - for value 1 to 254: set duty cycle in register OCRnr, COMnr1 is set
    // - digitalWrite() and digitalRead() reset COMnr1
    switch(g_pin_timer)
    {
        #if defined(TCCR0) && defined(COM00) && !defined(__AVR_ATmega8__)
        case TIMER0A:
                // XXX fix needed for atmega8
                // timer 0
                flag = bit_is_set(TCCR0, COM00);
                break;
        #endif

        #if defined(TCCR0A) && defined(COM0A1)
        case TIMER0A:
                // timer 0, channel A
                flag = bit_is_set(TCCR0A, COM0A1);
                break;
        #endif

        #if defined(TCCR0A) && defined(COM0B1)
        case TIMER0B:
                // timer 0, channel B
                flag = bit_is_set(TCCR0A, COM0B1);
                break;
        #endif


        #if defined(TCCR1A) && defined(COM1A1)
        case TIMER1A:
                // timer 1, channel B
                flag = bit_is_set(TCCR1A, COM1A1);
                break;
        #endif

        #if defined(TCCR1A) && defined(COM1B1)
        case TIMER1B:
                // timer 1, channel B
                flag = bit_is_set(TCCR1A, COM1B1);
                break;
        #endif

        #if defined(TCCR1A) && defined(COM1C1)
        case TIMER1C:
                // timer 1, channel C
                flag = bit_is_set(TCCR1A, COM1C1);
                break;
        #endif


        #if defined(TCCR2) && defined(COM21)
        case TIMER2:
                // timer 2
                flag = bit_is_set(TCCR2, COM21);
                break;
        #endif


        #if defined(TCCR2A) && defined(COM2A1)
        case TIMER2A:
                // timer 2, channel A
                flag = bit_is_set(TCCR2A, COM2A1);
                break;
        #endif

        #if defined(TCCR2A) && defined(COM2B1)
        case TIMER2B:
                // timer 2, channel B
                flag = bit_is_set(TCCR2A, COM2B1);
                break;
        #endif


        #if defined(TCCR3A) && defined(COM3A1)
        case TIMER3A:
                // timer 3, channel A
                flag = bit_is_set(TCCR3A, COM3A1);
                break;
        #endif

        #if defined(TCCR3A) && defined(COM3B1)
        case TIMER3B:
                // timer 3, channel B
                flag = bit_is_set(TCCR3A, COM3B1);
                break;
        #endif

        #if defined(TCCR3A) && defined(COM3C1)
        case TIMER3C:
                // timer 3, channel C
                flag = bit_is_set(TCCR3A, COM3C1);
                break;
        #endif


        #if defined(TCCR4A) && defined(COM4A1)
        case TIMER4A:
                // timer 4, channel A
                flag = bit_is_set(TCCR4A, COM4A1);
                break;
        #endif

        #if defined(TCCR4A) && defined(COM4B1)
        case TIMER4B:
                // timer 4, channel B
                flag = bit_is_set(TCCR4A, COM4B1);
                break;
        #endif

        #if defined(TCCR4A) && defined(COM4C1)
        case TIMER4C:
                // timer 4, channel C
                flag = bit_is_set(TCCR4A, COM4C1);
                break;
        #endif

        #if defined(TCCR4C) && defined(COM4D1)
        case TIMER4D:		
                // timer 4, channel D
                flag = bit_is_set(TCCR4C, COM4D1);		
                break;
        #endif


        #if defined(TCCR5A) && defined(COM5A1)
        case TIMER5A:
                // timer 5, channel A
                flag = bit_is_set(TCCR5A, COM5A1);		
                break;
        #endif

        #if defined(TCCR5A) && defined(COM5B1)
        case TIMER5B:
                // timer 5, channel B
                flag = bit_is_set(TCCR5A, COM5B1);	
                break;
        #endif

        #if defined(TCCR5A) && defined(COM5C1)
        case TIMER5C:
                // timer 5, channel C
                flag = bit_is_set(TCCR5A, COM5C1);	
                break;
        #endif

        case NOT_ON_TIMER:
        default:
                flag = false;
                break;
    }

#elif defined ARDUINO_ARCH_MEGAAVR
    // Pointer to a timer B
    TCB_t* timer_B;

    switch(g_pin_timer)
    {
        // TCA0
        // Normal mode (not split mode)
        // 3x Compare channels: 0 => pin9, 1 => pin10, 2 => pin5
        case TIMERA0:
                // Read Enable bit in Control register
                flag = bit_is_set(TCA0.SINGLE.CTRLB, TCA_SINGLE_CMP0EN_bp + g_pin_bit);
                //TCA0.SINGLE.CTRLB |= (1 << (TCA_SINGLE_CMP0EN_bp + bit_pos));
                break;

                // TCB0 => pin6, TCB1 => pin3
                // 8-bit PWM mode
                // 1x Compare channel:
                //    - Lower Bytes  L => control the period (max count)
                //    - Higher Bytes H => control the duty cycle
        case TIMERB0:
        case TIMERB1:
        case TIMERB2:
        case TIMERB3:
                // Get pointer to timer
                timer_B = ((TCB_t*)&TCB0 + (g_pin_timer - TIMERB0));

                // Read Enable bit in Control register
                flag = timer_B->CTRLB & TCB_CCMPEN_bm;
                break;

        case NOT_ON_TIMER:
        default:
                flag = false;
                break;
    }

#elif defined ARDUINO_ARCH_SAMD
    // pin description (see content in variant.cpp)
    PinDescription pin_description = g_APinDescription[pin];

    // if PWM pin
    if ((pin_description.ulPinAttribute & PIN_ATTR_PWM) == PIN_ATTR_PWM)
    {
        // for TCC (TCC0, TCC1, TCC2)
        //   TCC0: 0   => pin 18 (A3)
        //   TCC0: 1   => pin 19 (A4)
        //   TCC0: 2-5 => -
        //   TCC0: 6   => pin 6
        //   TCC0: 7   => pin 7

        //   TCC1: 0   => pin 2
        //   TCC1: 1   => pin 3
        //   TCC1: 2-3 => -

        //   TCC2: 0   => pin 8
        //   TCC2: 1-3 => -
        if (GetTCNumber(pin_description.ulPWMChannel) >= TCC_INST_NUM)
        {
            Tc* TCx = (Tc*)GetTC(pin_description.ulPWMChannel);
            flag = TCx->COUNT16.CTRLA.bit.ENABLE;
        }
        // for TC (TC3, TC4, TC5)
        //   TC3: 0    => -
        //   TC3: 1    => pin 10

        //   TC4: 0    => pin 0
        //   TC4: 1    => pin 1

        //   TC5: 0    => pin 4
        //   TC5: 1    => pin 5
        else
        {
            Tcc* TCCx = (Tcc*)GetTC(pin_description.ulPWMChannel);
            flag = TCCx->CTRLA.bit.ENABLE;
        }
    }
    else
        return MODE_DIGITAL;

#else
    return MODE_DIGITAL;
#endif

    if(flag)
        return MODE_PWM;
    else
        return MODE_DIGITAL;
}



// --------------------------------------------------------------------------------
// ADC/DAC/PWM specificities ------------------------------------------------------
// --------------------------------------------------------------------------------


#if defined ARDUINO_ARCH_SAMD
uint32_t shiftBits(
    uint32_t valueToShift,
    uint32_t startPos,
    uint32_t endPos)
{
    if (startPos > endPos)
        return valueToShift >> (startPos - endPos);

    else if (startPos < endPos)
        return valueToShift << (endPos - startPos);

    else // equality
        return valueToShift;
}

// set ADC/DAC/PWM resolution (in bit). To call from setup()
void HCS_adcResolution(uint8_t resolution)
{
    g_ADC_Resolution = resolution; // used to catch resolution value entered by user (no getter in wiring_analog.c)
    analogReadResolution(resolution);
}

void HCS_dacResolution(uint8_t resolution)
{
    g_DAC_Resolution = resolution; // used to catch resolution value entered by user (no getter in wiring_analog.c)
    analogWriteResolution(resolution);
}

void HCS_pwmResolution(uint8_t resolution)
{
    g_PWM_Resolution = resolution; // used to catch resolution value entered by user (no getter in wiring_analog.c)
    analogWriteResolution(resolution);
}

// get ADC/DAC/PWM resolution
uint8_t HCS_getPwmResolution() { return g_PWM_Resolution; }
uint8_t HCS_getDacResolution() { return g_DAC_Resolution; }
uint8_t HCS_getAdcResolution() { return g_ADC_Resolution; }


// DAC is enabled
bool HCS_DacIsEnabled(uint8_t pin)
{
    // pin description (see content in variant.cpp)
    PinDescription pin_description = g_APinDescription[pin];

    // if DAC pin
    if ((pin_description.ulPinAttribute & PIN_ATTR_ANALOG) == PIN_ATTR_ANALOG)
    {
        // if DAC0 (on pin A0)
        if ((pin_description.ulADCChannelNumber == ADC_Channel0) || (pin_description.ulADCChannelNumber == DAC_Channel0))
            return DAC->CTRLA.bit.ENABLE != 0;
    }

    return false;
}
#endif



// --------------------------------------------------------------------------------
// read DIO (Digital state, Analog value) -----------------------------------------
// --------------------------------------------------------------------------------


// read Digital IO ***********************************************************
bool HCS_readDI_LA(uint8_t pin)
{
    HCS_getPinMode(pin);
    return (*g_pin_in & g_pin_mask); // read input register
}

bool HCS_readDO_LA(uint8_t pin)
{
    return HCS_readDI_LA(pin); // read input register instead of output register => real value outputed on pin
}


// read Analog IO ************************************************************
#if defined ARDUINO_ARCH_SAMD
uint16_t HCS_readAI_LA(uint8_t pin, bool forceReading)
{
    // add offset, required for pin description
    if (pin < A0)
        pin += A0;

    // if DAC is enabled on this pin, don't read AI, except if forced to
    if(!forceReading && HCS_DacIsEnabled(pin))
        return 0;

    // analog input reading also disables DAC
    return (uint16_t)analogRead(pin);
}

uint16_t HCS_readAI_LA(uint8_t pin)
{
    HCS_readAI_LA(pin, false);  
}
#else
uint16_t HCS_readAI_LA(uint8_t pin)
{
    return (uint16_t)analogRead(pin);
}
#endif

#if defined ARDUINO_ARCH_SAMD
uint16_t HCS_readPWM_LA(uint8_t pin) // 0-4095 (12-bit)
#else
uint8_t HCS_readPWM_LA(uint8_t pin)  // 0-255 (8-bit)
#endif
{
    // get pin data, read pin mode, read pwm mode
    uint8_t outputMode = getOutputMode(pin);

    // Mode UNKNOWN
    if (outputMode == MODE_UNKNOWN)
        return 0;

#if defined ARDUINO_ARCH_AVR
    // Mode DIGITAL: for values 0 and 255
    else if (outputMode == MODE_DIGITAL)
        return HCS_readDO_LA(pin) ? 255 : 0;

    // Mode ANALOG: for values 1 to 254
    else
    {
        // PWM is controlled by a timer (n).
        // There are 2 or 3 Output Compare pin (r) per timer (A, B, C)
        // Output Compare register OCRnr holds the duty cycle value
        // TCCRnA and TCCRnB => control the timer behavior:
        //   - WGMnx             - 3 bits                        => control Wave Generation Mode of the timer (normal, ctc, fast pwm, phase correct pwm)
        //   - COMnAx and COMnBx - 2 bits per Output Compare pin => control Compare Output Mode
        //
        // For Uno (see wiring.c)
        //   - Timer0 is set as fast pwm (WGM = B011) and Timer1 and 2 as phase correct pwm (WGM = B001)
        //   - COMnr0 = 0
        //   - COMnr1 enable pwm
        //
        // analogWrite() works that way:
        // - for value 0 and 255: call to digitalWrite(), COMnr1 is reset
        // - for value 1 to 254: set duty cycle in register OCRnr, COMnr1 is set
        // - digitalWrite() and digitalRead() reset COMnr1
        switch(g_pin_timer)
        {
            #if defined(TCCR0) && defined(COM00) && !defined(__AVR_ATmega8__)
            case TIMER0A:
                    // XXX fix needed for atmega8
                    // timer 0
                    return OCR0;
            #endif
    
            #if defined(TCCR0A) && defined(COM0A1)
            case TIMER0A:
                    // timer 0, channel A
                    return OCR0A;
            #endif
    
            #if defined(TCCR0A) && defined(COM0B1)
            case TIMER0B:
                    // timer 0, channel B
                    return OCR0B;
            #endif

    
            #if defined(TCCR1A) && defined(COM1A1)
            case TIMER1A:
                    // timer 1, channel A
                    return OCR1A;
            #endif
    
            #if defined(TCCR1A) && defined(COM1B1)
            case TIMER1B:
                    // timer 1, channel B
                    return OCR1B;
            #endif
    
            #if defined(TCCR1A) && defined(COM1C1)
            case TIMER1C:
                    // timer 1, channel C
                    return OCR1C;
            #endif

    
            #if defined(TCCR2) && defined(COM21)
            case TIMER2:
                    // timer 2
                    return OCR2;
            #endif
    

            #if defined(TCCR2A) && defined(COM2A1)
            case TIMER2A:
                    // timer 2, channel A
                    return OCR2A;
            #endif
    
            #if defined(TCCR2A) && defined(COM2B1)
            case TIMER2B:
                    // timer 2, channel B
                    return OCR2B;
            #endif

    
            #if defined(TCCR3A) && defined(COM3A1)
            case TIMER3A:
                    // timer 3, channel A
                    return OCR3A;
            #endif
    
            #if defined(TCCR3A) && defined(COM3B1)
            case TIMER3B:
                    // timer 3, channel B
                    return OCR3B;
            #endif
    
            #if defined(TCCR3A) && defined(COM3C1)
            case TIMER3C:
                    // timer 3, channel C
                    return OCR3C;
            #endif
    

            #if defined(TCCR4A) && defined(COM4A1)
            case TIMER4A:
                    // timer 4, channel A
                    return OCR4A;
            #endif
    
            #if defined(TCCR4A) && defined(COM4B1)
            case TIMER4B:
                    // timer 4, channel B
                    return OCR4B;
            #endif
    
            #if defined(TCCR4A) && defined(COM4C1)
            case TIMER4C:
                    // timer 4, channel C
                    return OCR4C;
            #endif
    
            #if defined(TCCR4C) && defined(COM4D1)
            case TIMER4D:		
                    // timer 4, channel D
                    return OCR4D;
            #endif
    
    
            #if defined(TCCR5A) && defined(COM5A1)
            case TIMER5A:
                    // timer 5, channel A
                    return OCR5A;
            #endif
    
            #if defined(TCCR5A) && defined(COM5B1)
            case TIMER5B:
                    // timer 5, channel B
                    return OCR5B;
            #endif
    
            #if defined(TCCR5A) && defined(COM5C1)
            case TIMER5C:
                    // timer 5, channel C
                    return OCR5C;
            #endif

            case NOT_ON_TIMER:
            default:
                    return 0;
        }
    }

#elif defined ARDUINO_ARCH_MEGAAVR
    // Mode DIGITAL: for values 0 and 255
    else if (outputMode == MODE_DIGITAL)
        return HCS_readDO_LA(pin) ? 255 : 0;

    // Mode ANALOG: for values 1 to 254
    else
    {
        // TCA0 Compare 0 Buffer
        uint16_t* timer_cmp_out;

        // TCBx Pointer
        TCB_t* timer_B;

        // Saved Status Register (= saved Interrupt flag)
        uint8_t savedSREG;

        // Read value
        uint8_t value = 0;

        switch(g_pin_timer)
        {
            // TCA0
            // Normal mode (not split mode)
            // 3x Compare channels: 0 => pin9, 1 => pin10, 2 => pin5
            case TIMERA0:
                    // Get Compare 0 Buffer register
				    timer_cmp_out = ((uint16_t*) (&TCA0.SINGLE.CMP0BUF)) + g_pin_bit;

				    // read duty cycle from Compare 0 Buffer
                    // (16-bit operations are non atomic)
                    savedSREG = SREG;
                    cli();
                    value = (uint8_t) *timer_cmp_out;
                    SREG = savedSREG;

				    return value;

            // TCB0 => pin6, TCB1 => pin3
            // 8-bit PWM mode
            // 1x Compare channel:
            //    - Lower Bytes  L => control the period (max count)
            //    - Higher Bytes H => control the duty cycle
			case TIMERB0:
			case TIMERB1:
			case TIMERB2:
			case TIMERB3:
				    // Get pointer to timer
				    timer_B = ((TCB_t *)&TCB0 + (g_pin_timer - TIMERB0));

                    // read duty cycle from Compare channel (Higher bytes)
                    // (16-bit operations are non atomic and use a temporary register)
                    savedSREG = SREG;
                    cli();
                    timer_B->CCMPL;         // copy CCMPH into temporary register
                    value = timer_B->CCMPH; // copy temporary register content into value
                    SREG = savedSREG;

				    return value;

            case NOT_ON_TIMER:
            default:
                    return 0;
        }
    }

#elif defined ARDUINO_ARCH_SAMD
    // if PWM pin
    else if (outputMode == MODE_PWM)
    {
        // pin description (see content in variant.cpp)
        PinDescription pin_description = g_APinDescription[pin];

        // Timer Counter channel
        uint8_t channel = GetTCChannelNumber(pin_description.ulPWMChannel);

        // for TCC (TCC0, TCC1, TCC2)
        //   TCC0: 0   => pin 18 (A3)
        //   TCC0: 1   => pin 19 (A4)
        //   TCC0: 2-5 => -
        //   TCC0: 6   => pin 6
        //   TCC0: 7   => pin 7

        //   TCC1: 0   => pin 2
        //   TCC1: 1   => pin 3
        //   TCC1: 2-3 => -

        //   TCC2: 0   => pin 8
        //   TCC2: 1-3 => -
        if (GetTCNumber(pin_description.ulPWMChannel) >= TCC_INST_NUM)
        {
            Tc* TCx = (Tc*)GetTC(pin_description.ulPWMChannel);

            // read register (wait for synchronization before and after reading)
            while (TCx->COUNT16.STATUS.bit.SYNCBUSY);
            uint16_t value = TCx->COUNT16.CC[channel].reg;
            while (TCx->COUNT16.STATUS.bit.SYNCBUSY);

            // 16-bit counter => adapt resolution
            return shiftBits(value, 16, g_PWM_Resolution);
        }
        // for TC (TC3, TC4, TC5)
        //   TC3: 0    => -
        //   TC3: 1    => pin 10

        //   TC4: 0    => pin 0
        //   TC4: 1    => pin 1

        //   TC5: 0    => pin 4
        //   TC5: 1    => pin 5
        else
        {
            Tcc* TCCx = (Tcc*)GetTC(pin_description.ulPWMChannel);

            // read register (wait for synchronization before and after reading)
            while (TCCx->SYNCBUSY.reg & TCC_SYNCBUSY_MASK);
            uint32_t value = TCCx->CCB[channel].reg;
            while (TCCx->SYNCBUSY.reg & TCC_SYNCBUSY_MASK);

            // 16-bit counter => adapt resolution
            return shiftBits(value, 16, g_PWM_Resolution);
        }
    }   
    else
        return 0;

#else
    return 0;
#endif
}


#if defined ARDUINO_ARCH_SAMD
uint16_t HCS_readDAC_LA(uint8_t pin) // 0-1023 (10-bit)
{
    if (HCS_DacIsEnabled(pin))
    {
        // read register (wait for synchronization before and after reading)
        while (ADC->STATUS.bit.SYNCBUSY == 1);
        uint16_t value = DAC->DATA.reg;
        while (ADC->STATUS.bit.SYNCBUSY == 1);

        // 10-bit DAC => adapt resolution
        return shiftBits(value, 10, g_DAC_Resolution);
    }   

    return 0;
}
#endif



// --------------------------------------------------------------------------------
// write DIO (Digital state, Analog value) ----------------------------------------
// --------------------------------------------------------------------------------

// write Digital Output
void HCS_writeDO_LA(uint8_t pin, uint8_t val) 
{ 
    digitalWrite(pin, val); 
}

// write Analog Output
#if defined ARDUINO_ARCH_SAMD
void HCS_writePWM_LA(uint8_t pin, uint16_t value)
{
    // set PWM resolution back (value initialized during setup()).
    // => required because resolution can be lost when changing resolution for DAC
    // => resolution must be the same for all PWM
    HCS_pwmResolution(HCS_getPwmResolution());
    analogWrite(pin, value);
}

void HCS_writeDAC_LA(uint8_t pin, uint16_t value)
{
    // set DAC resolution back (value initialized during setup()).
    // => required because resolution can be lost when changing resolution for PWM
    // => resolution must be the same for all DAC
    HCS_dacResolution(HCS_getDacResolution());
    analogWrite(pin, value);
}
#else
void HCS_writePWM_LA(uint8_t pin, uint8_t value)
{
    analogWrite(pin, value);
}
#endif
