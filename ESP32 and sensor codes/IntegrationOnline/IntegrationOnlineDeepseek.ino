#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "HX711.h"
#include <Adafruit_MLX90614.h>
#include "MAX30105.h"
#include "filters.h"

// ----- OLED Settings -----
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// ----- HX711 (Load Cell) Settings -----
const int LOADCELL_DOUT_PIN = 4;
const int LOADCELL_SCK_PIN = 5;
HX711 scale;
float calibration_factor = 205.8;

// ----- MLX90614 (Temperature Sensor) Settings -----
Adafruit_MLX90614 mlx = Adafruit_MLX90614();

// ----- MAX30102 (SpO2 & Pulse Sensor) Settings -----
MAX30105 particleSensor;
const float kSamplingFrequency = 400.0;
const unsigned long kFingerThreshold = 10000;

// ----- Filters and Signal Processing -----
LowPassFilter low_pass_filter_red(5.0, kSamplingFrequency);
LowPassFilter low_pass_filter_ir(5.0, kSamplingFrequency);
HighPassFilter high_pass_filter(0.5, kSamplingFrequency);
Differentiator differentiator(kSamplingFrequency);
MovingAverageFilter<5> averager_bpm;
MinMaxAvgStatistic stat_red;
MinMaxAvgStatistic stat_ir;

// SpO2 calibration constants
const float kSpO2_A = 1.5958422;
const float kSpO2_B = -34.6596622;
const float kSpO2_C = 112.6898759;

// ----- State Management -----
enum SensorState { WAIT_PATIENT, WEIGHT, SPO2, TEMP, ECG, IDLE };
SensorState currentState = WAIT_PATIENT;
unsigned long stateStartTime = 0;
String ecgData = "";
String patientID = "";
String patientName = "";

void showMessage(String msg, unsigned long delayTime = 2000) {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println(msg);
  display.display();
  delay(delayTime);
}

void setup() {
  Serial.begin(115200);
  Wire.begin();

  // Initialize OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("OLED allocation failed");
    while(1);
  }
  display.setTextSize(1);
  display.setTextColor(WHITE);
  showMessage("Enter patient ID\nand Name via PC", 3000);

  // Initialize Load Cell
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale.set_scale(calibration_factor);
  scale.tare();
  showMessage("Load Cell Tare Completed", 2000);

  // Initialize MLX90614
  if (!mlx.begin()) {
    showMessage("MLX90614 Error!", 3000);
    while(1);
  }

  // Initialize MAX30102
  if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) {
    showMessage("MAX30102 Error!", 3000);
    while(1);
  }
  particleSensor.setup(0x1F, 4, 2, 400, 411, 4096); // 400Hz sampling

  currentState = WAIT_PATIENT;
}

void loop() {
  if (currentState == WAIT_PATIENT) {
    if (Serial.available()) {
      if (patientID == "") {
        patientID = Serial.readStringUntil('\n');
        patientID.trim();
      } else if (patientName == "") {
        patientName = Serial.readStringUntil('\n');
        patientName.trim();
      }
      if (patientID != "" && patientName != "") {
        Serial.println("PATIENT_RECEIVED");
        showMessage("Patient Data Received.\nStarting Vitals...", 2000);
        currentState = WEIGHT;
      }
    }
  }
  else if (currentState == WEIGHT) {
    showMessage("Weight meas in 2 sec...", 2000);
    unsigned long startTime = millis();
    float weightSum = 0;
    int count = 0;
    while (millis() - startTime < 5000) {  // 5-second measurement window
      if (scale.is_ready()) {
        float weight = scale.get_units(5); // Average of 5 readings
        weightSum += weight;
        count++;
      }
      delay(100); // Sample every 100 ms
    }
    float avgWeight = (count > 0) ? weightSum / count : 0;
    
    // Display on OLED
    display.clearDisplay();
    display.setCursor(0, 0);
    display.setTextSize(2);
    display.print("Weight:");
    display.setCursor(0, 30);
    display.print(-avgWeight, 2);  // Invert sign if needed.
    display.print(" g");
    display.display();
    delay(3000);
    
    // Send weight data over Serial
    Serial.print("WEIGHT:");
    Serial.println(-avgWeight, 2);
    
    showMessage("Weight recorded.\nNext: SpO2 in 2 sec", 2000);
    currentState = SPO2;
    stateStartTime = millis();
  }
  else if (currentState == SPO2) {
    showMessage("SpO2 meas in 2 sec...", 2000);
    delay(2000);
    
    stat_red.reset();
    stat_ir.reset();
    averager_bpm.reset();
    long last_heartbeat = 0;
    float last_diff = NAN;
    bool crossed = false;
    unsigned long measurementStart = millis();

    while (millis() - measurementStart < 10000) {
      auto sample = particleSensor.readSample(1000);
      
      if (sample.red > kFingerThreshold) {
        float filtered_red = low_pass_filter_red.process(sample.red);
        float filtered_ir = low_pass_filter_ir.process(sample.ir);
        
        stat_red.process(filtered_red);
        stat_ir.process(filtered_ir);

        // Heartbeat detection
        float hp_filtered = high_pass_filter.process(filtered_red);
        float diff = differentiator.process(hp_filtered);

        if (!isnan(last_diff)) {
          if (last_diff > 0 && diff < 0) crossed = true;
          if (diff > 0) crossed = false;

          if (crossed && diff < -2000) {
            if (last_heartbeat != 0 && millis() - last_heartbeat > 300) {
              int bpm = 60000 / (millis() - last_heartbeat);
              if (bpm > 50 && bpm < 250) {
                averager_bpm.process(bpm);
              }
              last_heartbeat = millis();
            }
            crossed = false;
          }
        }
        last_diff = diff;
      }
      particleSensor.nextSample();
    }

    // Calculate SpO2
    float r_red = (stat_red.maximum()-stat_red.minimum())/stat_red.average();
    float r_ir = (stat_ir.maximum()-stat_ir.minimum())/stat_ir.average();
    float r = r_red/r_ir;
    float spo2 = kSpO2_A * r * r + kSpO2_B * r + kSpO2_C;
    int avg_bpm = averager_bpm.process(0); // Get average

    // Display results
    display.clearDisplay();
    display.setCursor(0, 0);
    display.setTextSize(2);
    if (spo2 > 0 && avg_bpm > 0) {
      display.print("HR:");
      display.print(avg_bpm);
      display.setCursor(0, 30);
      display.print("SpO2:");
      display.print(spo2, 1);
      Serial.print("SPO2:");
      Serial.println(spo2, 1);
      Serial.print("PULSE:");
      Serial.println(avg_bpm);
    } else {
      display.println("Invalid Reading");
    }
    display.display();
    delay(3000);

    currentState = TEMP;
    stateStartTime = millis();
  }
  else if (currentState == TEMP) {
    showMessage("Temp meas in 2 sec...", 2000);
    unsigned long startTime = millis();
    float tempSum = 0;
    int count = 0;
    while (millis() - startTime < 5000) {  // 5-second measurement window
      float tempC = mlx.readObjectTempC();
      tempSum += tempC;
      count++;
      delay(200); // Sample every 200 ms
    }
    float avgTemp = (count > 0) ? tempSum / count : 0;
    
    display.clearDisplay();
    display.setCursor(0, 0);
    display.setTextSize(2);
    display.print("Temp:");
    display.setCursor(0, 30);
    display.print(avgTemp, 1);
    display.print(" C");
    display.display();
    delay(3000);
    
    // Send temperature over Serial
    Serial.print("TEMP:");
    Serial.println(avgTemp, 1);
    
    showMessage("Temp recorded.\nNext: ECG in 2 sec", 2000);
    currentState = ECG;
    stateStartTime = millis();
  }
  else if (currentState == ECG) {
    showMessage("ECG meas in 2 sec...", 2000);
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("Recording ECG");
    display.display();
    
    // Notify Python script to start capturing ECG data
    Serial.println("ECG_START");
    
    ecgData = "";
    unsigned long startTime = millis();
    while (millis() - startTime < 30000) {  // 30-second measurement window
      int ecgValue = analogRead(ECG_PIN);
      Serial.println(ecgValue);
      ecgData += String(ecgValue) + ",";
      delay(20);  // ~50Hz sampling rate
    }
    if (ecgData.endsWith(",")) {
      ecgData.remove(ecgData.length() - 1);
    }
    
    // After ECG, send end marker along with final vitals (in case any update is needed)
    Serial.println("DATA_END");
    
    display.clearDisplay();
    display.setCursor(0, 0);
    display.setTextSize(1);
    display.println("Data uploaded.");
    display.println("Please unplug");
    display.println("device and replug");
    display.display();
    currentState = IDLE;
  }
  else if (currentState == IDLE) {
    while(true);
  }
}