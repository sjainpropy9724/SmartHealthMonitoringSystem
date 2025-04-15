/*!
 * @file MAX30105.cpp
 */

#include "MAX30105.h"

const uint8_t MAX30105::INT_CFG_REG[] = {
  0x02,                                     //!< FIFO Almost Full Interrupt Enable Register
  0x03,                                     //!< Temperature Ready Interrupt Enable Register
  0x02,                                     //!< PPG Ready Interrupt Enable Register    
  0x02,                                     //!< Ambient Light Cancellation Overflow Interrupt Enable Register
  0x02,                                     //!< Proximity Interrupt Enable Register
  0xFF                                      //!< Power Ready Status Interrupt Enable Register (not existing)
};

const uint8_t MAX30105::INT_CFG_BIT[] = {
  7,                                        //!< FIFO Almost Full Interrupt Enable Bit
  1,                                        //!< Temperature Ready Interrupt Enable Bit
  6,                                        //!< PPG Ready Interrupt Enable Bit    
  5,                                        //!< Ambient Light Cancellation Overflow Interrupt Enable Bit
  4,                                        //!< Proximity Interrupt Enable Bit
  255                                       //!< Power Ready Interrupt Enable Bit (not existing)
};

const uint8_t MAX30105::INT_ST_REG[] = {
  0x00,                                     //!< FIFO Almost Full Interrupt Status Register
  0x01,                                     //!< Temperature Ready Interrupt Status Register
  0x00,                                     //!< PPG Ready Interrupt Status Register    
  0x00,                                     //!< Ambient Light Cancellation Overflow Interrupt Status Register
  0x00,                                     //!< Proximity Interrupt Status Register
  0x00                                      //!< Power Ready Interrupt Status Register
};

const uint8_t MAX30105::INT_ST_BIT[] = {
  7,                                        //!< FIFO Almost Full Interrupt Status Bit
  1,                                        //!< Temperature Ready Interrupt Status Bit
  6,                                        //!< PPG Ready Interrupt Status Bit    
  5,                                        //!< Ambient Light Cancellation Overflow Interrupt Status Bit
  4,                                        //!< Proximity Interrupt Status Bit
  0                                         //!< Power Ready Interrupt Status Bit
};

/**
 * Constructor
 * Initializes a new sensor instance
 * 
 * @param addr Sensor Address (default 0x57)
 * @param wire TWI bus instance (default Wire)
 */
MAX30105::MAX30105(uint8_t addr, TwoWire& wire) : MAX3010xMultiLed(addr, wire) {
  
}

/**
 * Set Default Configuration
 * @returns true if successful, otherwise false
 */
bool MAX30105::setDefaultConfiguration() {
  MultiLedConfiguration cfg {};
  
  if(!setMultiLedConfiguration(cfg)) return false;
  if(!setLedCurrent(LED_RED, 90)) return false;
  if(!setLedCurrent(LED_IR, 80)) return false;
  if(!setLedCurrent(LED_GREEN, 100)) return false;
  if(!setProximityLedCurrent(100)) return false;
  if(!setResolution(RESOLUTION_18BIT_4110US)) return false;
  if(!setSamplingRate(SAMPLING_RATE_50SPS)) return false;
  if(!setSampleAveraging(SMP_AVE_NONE)) return false;
  if(!setADCRange(ADC_RANGE_16384NA)) return false;
  if(!enableFIFORollover()) return false;
  if(!setMode(MODE_SPO2)) return false;
  
  return true;
}

/**
 * Set LED Current
 * @param led LED
 * @param current LED Current in 0.2 mA steps
 * @returns true if successful, otherwise false
 */
bool MAX30105::setLedCurrent(MAX30105::Led led, uint8_t current) {
  return writeByte(LED_CFG_REG_BASE + static_cast<uint8_t>(led), current);
}

/**
 * Set LED Current for proximity mode
 * @param current LED Current in 0.2 mA steps
 * @returns true if successful, otherwise false
 */
bool MAX30105::setProximityLedCurrent(uint8_t current) {
  return writeByte(PILOT_LED_CFG_REG, current);
}

/**
 * Set Proximity Interrupt Threshold
 * @param threshold Threshold
 * @returns true if successful, otherwise false
 */
bool MAX30105::setProximityThreshold(uint8_t threshold) {
  return writeByte(PROX_INT_TRESH_REG, threshold);
}

/**
 * Set Multi LED Configuration
 * @param cfg Multi LED Configuration
 * @returns true if successful, otherwise false
 */
bool MAX30105::setMultiLedConfiguration(const MAX30105::MultiLedConfiguration& cfg) {
  
  // Count active slots and ensure that slots are enable in order
  uint8_t activeSlots = 0;
  for(int i = 0; i < 4; i++) {
    if(static_cast<uint8_t>(cfg.slot[i]) > 0b111) return false;
    
    if(cfg.slot[i] != SLOT_OFF && cfg.slot[i] != SLOT_PILOT_OFF) {
      if(activeSlots != i) return false;
      activeSlots++;
    }
  }
  
  uint8_t rawCfg[2] {};
  rawCfg[0] = static_cast<uint8_t>(cfg.slot[0]) | (static_cast<uint8_t>(cfg.slot[1]) << 4);
  rawCfg[1] = static_cast<uint8_t>(cfg.slot[2]) | (static_cast<uint8_t>(cfg.slot[3]) << 4);
  
  
  return setMultiLedConfigurationInternal(activeSlots, rawCfg);
}
