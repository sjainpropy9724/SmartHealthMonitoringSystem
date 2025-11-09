#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "HX711.h"
#include <Adafruit_MLX90614.h>
#include "MAX30105.h"
#include "spo2_algorithm.h"

// ----- OLED Settings -----
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1  // Use -1 if no reset pin is used
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// ----- HX711 (Load Cell) Settings -----
const int LOADCELL_DOUT_PIN = 4;
const int LOADCELL_SCK_PIN = 5;
HX711 scale;
float calibration_factor = 205.8;  // Calibration factor for your 10kg load cell

// ----- MLX90614 (Temperature Sensor) Settings -----
Adafruit_MLX90614 mlx = Adafruit_MLX90614();

// ----- MAX30102 (SpO2 & Pulse Sensor) Settings -----
// Using the MAX30105 library which supports the MAX30102
MAX30105 particleSensor;

// ----- AD8232 (ECG) Settings -----
const int ECG_PIN = 35; // Analog input for ECG signal

// ----- State Management -----
enum SensorState { WAIT_PATIENT, WEIGHT, SPO2, TEMP, ECG, IDLE };
SensorState currentState = WAIT_PATIENT;
unsigned long stateStartTime = 0;
String ecgData = ""; // To accumulate ECG readings

// Variables to store patient data
String patientID = "";
String patientName = "";

// ----- Helper Function to Display Messages on OLED -----
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

  // Initialize OLED display
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("OLED allocation failed");
    while (1);
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  showMessage("Enter patient ID\nand Name via PC", 3000);

  // Initialize HX711 (Load Cell)
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale.set_scale(calibration_factor);
  scale.tare();
  showMessage("Load Cell Tare Completed", 2000);

  // Initialize MLX90614 (Temperature Sensor)
  if (!mlx.begin()) {
    showMessage("MLX90614 Error!", 3000);
    while (1);
  }

  // Initialize MAX30102 (SpO2 & Pulse Sensor)
  if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) {
    showMessage("MAX30102 Error!", 3000);
    while (1);
  }
  // Configure MAX30102; parameters may need tweaking for your setup.
  particleSensor.setup(0x1F, 4, 2, 100, 411, 4096);

  currentState = WAIT_PATIENT;
  stateStartTime = millis();
}

void loop() {
  // ----- WAIT FOR PATIENT INFO -----
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
        // Notify PC that patient info is received
        Serial.println("PATIENT_RECEIVED");
        showMessage("Patient Data Received.\nStarting Vitals...", 2000);
        currentState = WEIGHT;
        stateStartTime = millis();
      }
    }
  }
  
  // ----- WEIGHT MEASUREMENT (5 sec) -----
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
  
  // ----- SpO2 & Pulse Measurement (10 sec) -----
  else if (currentState == SPO2) {
    showMessage("SpO2 meas in 2 sec...", 2000);
    const int sampleCount = 100;  // Adjust as needed
    uint32_t irBuffer[sampleCount], redBuffer[sampleCount];
    unsigned long startTime = millis();
    int index = 0;
    
    while (millis() - startTime < 10000 && index < sampleCount) { // 10-second window
      redBuffer[index] = particleSensor.getRed();
      irBuffer[index] = particleSensor.getIR();
      particleSensor.nextSample();
      index++;
      delay(100); // Roughly 100 ms per sample
    }
    
    int32_t spo2;
    int8_t validSPO2;
    int32_t heartRate;
    int8_t validHeartRate;
    
    maxim_heart_rate_and_oxygen_saturation(irBuffer, sampleCount, redBuffer,
                                             &spo2, &validSPO2,
                                             &heartRate, &validHeartRate);
    
    // Display on OLED: Show both SpO2 and Heart Rate together
    display.clearDisplay();
    display.setCursor(0, 0);
    display.setTextSize(2);
    if (spo2 && heartRate) {
      display.print("HR:");
      display.print(heartRate);
      display.setCursor(0, 30);
      display.print("SpO2:");
      display.print(spo2);
    } else {
      display.println("SpO2 reading");
      display.println("invalid");
    }
    display.display();
    delay(3000);
    
    // Send SpO2 and Pulse data over Serial
    Serial.print("SPO2:");
    Serial.println(spo2);
    Serial.print("PULSE:");
    Serial.println(heartRate);
    
    showMessage("SpO2 recorded.\nNext: Temp in 2 sec", 2000);
    currentState = TEMP;
    stateStartTime = millis();
  }
  
  // ----- Temperature Measurement (5 sec) -----
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
  
  // ----- ECG Measurement (30 sec) -----
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
  
  // ----- IDLE: Halt further processing -----
  else if (currentState == IDLE) {
    while (true);  // System halts here until power cycle.
  }
}
