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
enum SensorState { WEIGHT, SPO2, TEMP, ECG, IDLE };
SensorState currentState = WEIGHT;
unsigned long stateStartTime = 0;
String ecgData = ""; // To accumulate ECG readings

// ----- Helper Function to Display Messages -----
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
  showMessage("System Initializing...", 2000);

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
  // Configure sensor; parameters may need tweaking
  particleSensor.setup(0x1F, 4, 2, 100, 411, 4096);

  // All sensors sharing I2C use: SDA (GPIO21) and SCL (GPIO22)
  currentState = WEIGHT;
  stateStartTime = millis();
}

void loop() {
  switch (currentState) {
    case WEIGHT: {
      // --- WEIGHT MEASUREMENT (5 sec) ---
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
      
      display.clearDisplay();
      display.setCursor(0, 0);
      display.setTextSize(2);
      display.print("Weight:");
      display.setCursor(0, 30);
      display.print(-avgWeight, 2);
      display.print(" g");
      display.display();
      delay(3000);
      
      showMessage("Weight recorded.\nNext: SpO2 in 2 sec", 2000);
      currentState = SPO2;
      stateStartTime = millis();
      break;
    }
    
    case SPO2: {
      // --- SpO2 & Pulse Measurement (10 sec) ---
      showMessage("SpO2 meas in 2 sec...", 2000);
      const int sampleCount = 100;  // Adjust as needed for duration
      uint32_t irBuffer[sampleCount], redBuffer[sampleCount];
      unsigned long startTime = millis();
      int index = 0;
      
      display.clearDisplay();
      display.setCursor(0, 0);
      display.setTextSize(1);
      while (millis() - startTime < 10000 && index < sampleCount) { // 10-second window
        redBuffer[index] = particleSensor.getRed();
        irBuffer[index] = particleSensor.getIR();
        particleSensor.nextSample();
        index++;
        delay(100); // Roughly 100 ms per sample (adjust if necessary)
      }
      
      int32_t spo2;
      int8_t validSPO2;
      int32_t heartRate;
      int8_t validHeartRate;
      
      maxim_heart_rate_and_oxygen_saturation(irBuffer, sampleCount, redBuffer,
                                               &spo2, &validSPO2,
                                               &heartRate, &validHeartRate);
      
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
      
      showMessage("SpO2 recorded.\nNext: Temp in 2 sec", 2000);
      currentState = TEMP;
      stateStartTime = millis();
      break;
    }
    
    case TEMP: {
      // --- Temperature Measurement (5 sec) ---
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
      
      showMessage("Temp recorded.\nNext: ECG in 2 sec", 2000);
      currentState = ECG;
      stateStartTime = millis();
      break;
    }
    
    case ECG: {
      // --- ECG Measurement (30 sec) ---
      showMessage("ECG meas in 2 sec...", 2000);
      display.clearDisplay();
      display.setCursor(0, 0);
      display.println("Recording ECG");
      display.display();
      
      // Notify Python script to start recording by sending a marker.
      Serial.println("ECG_START");
      
      ecgData = "";
      unsigned long startTime = millis();
      while (millis() - startTime < 30000) {  // 30-second measurement window
        int ecgValue = analogRead(ECG_PIN);
        // Send ECG value over Serial for capture on the laptop.
        Serial.println(ecgValue);
        ecgData += String(ecgValue) + ",";
        delay(20);  // ~50Hz sampling rate
      }
      // Remove trailing comma if it exists
      if (ecgData.endsWith(",")) {
        ecgData.remove(ecgData.length() - 1);
      }
      
      display.clearDisplay();
      display.setCursor(0, 0);
      display.println("ECG Complete!");
      display.println("Data sent to Serial");
      display.display();
      delay(3000);
      
      // Transition to IDLE state to halt further processing.
      currentState = IDLE;
      break;
    }
    
    case IDLE: {
      // --- Final State: Stop all processing ---
      display.clearDisplay();
      display.setCursor(0, 0);
      display.setTextSize(1);
      display.println("All readings");
      display.println("recorded.");
      display.println("Unplug device");
      display.display();
      
      // Freeze execution. (The ESP32 will only restart when unplugged and replugged.)
      while (true);
      break;
    }
  }
}
