#include <Wire.h>
#include "MAX30105.h"
#include "spo2_algorithm.h"
#include "filters.h"

MAX30105 particleSensor;

// Spo2 calculation variables
#define MAX_BRIGHTNESS 255
#define SAMPLING_TIME_MS 15000 // 15 seconds

uint32_t irBuffer[500];  //infrared LED sensor data
uint32_t redBuffer[500]; //red LED sensor data
int32_t bufferLength;
int32_t spo2;
int8_t validSPO2;
int32_t heartRate;
int8_t validHeartRate;

void setup() {
  Serial.begin(115200);
  Wire.begin();

  // MAX30102 Initialization
  if (!particleSensor.begin(Wire, I2C_SPEED_STANDARD)) {
    Serial.println("MAX30102 not found. Check wiring/power.");
    while (1);
  }

  // Sensor configuration similar to your working test code
  particleSensor.setup(60, 4, 2, 400, 411, 4096);
  delay(1500);  // Warm-up time

  Serial.println("MAX30102 Initialized");
}

void loop() {
  // Wait before measuring
  Serial.println("Preparing to measure SpO2 and heart rate...");
  delay(1000); // Added warm-up delay

  measureSPO2();

  // Print results
  Serial.print("SpO2: ");
  Serial.print(spo2);
  Serial.print("%, HR: ");
  Serial.print(heartRate);
  Serial.println(" bpm");

  delay(5000); // Wait before next loop
}

void measureSPO2() {
  bufferLength = 500;
  int i;

  // Clear filters

  // Collect 500 samples (~15 seconds)
  for (i = 0; i < bufferLength; i++) {
    while (particleSensor.available() == false); // wait for data

    redBuffer[i] = particleSensor.getRed();
    irBuffer[i] = particleSensor.getIR();
    particleSensor.nextSample(); // advance to next sample
  }

  // Run algorithm
  maxim_heart_rate_and_oxygen_saturation(irBuffer, bufferLength, redBuffer,
                                         &spo2, &validSPO2, &heartRate, &validHeartRate);

  if (validSPO2 == 0 || validHeartRate == 0) {
    Serial.println("Could not get valid readings.");
    spo2 = -999;
    heartRate = -999;
  }
}
