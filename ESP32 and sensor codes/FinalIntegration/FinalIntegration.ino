#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "HX711.h"
#include <Adafruit_MLX90614.h>
#include <MAX3010x.h>
#include "filters.h"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define BOOT_DELAY 500

#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Load Cell
const int LOADCELL_DOUT_PIN = 4;
const int LOADCELL_SCK_PIN = 5;
HX711 scale;
float calibration_factor = 205.8;

// Temperature Sensor
Adafruit_MLX90614 mlx = Adafruit_MLX90614();

// MAX30102 Configuration
MAX30105 sensor;
const auto kSamplingRate = sensor.SAMPLING_RATE_400SPS;
const float kSamplingFrequency = 400.0;

const unsigned long kFingerThreshold = 10000;
const unsigned int kFingerCooldownMs = 500;

const float kEdgeThreshold = -2000.0;

const float kLowPassCutoff = 5.0;
const float kHighPassCutoff = 0.5;

const bool kEnableAveraging = true;
const int kAveragingSamples = 5;
const int kSampleThreshold = 5;

// State Management
enum SensorState
{
  WAIT_PATIENT,
  WEIGHT,
  SPO2,
  TEMP,
  ECG,
  IDLE
};
SensorState currentState = WAIT_PATIENT;
unsigned long stateStartTime = 0;
String ecgData = "";
String patientID = "";
String patientName = "";
const int ECG_PIN = 35;

void showMessage(String msg, unsigned long delayTime = 2000)
{
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println(msg);
  display.display();
  if (delayTime > 0) {
    delay(delayTime);
  }
}

void setup()
{
  delay(BOOT_DELAY);
  Serial.begin(115200);
  Wire.begin();
  while (!Serial);

  Serial.println("\n\nBOOT:Initializing system...");
  Serial.flush();

  // OLED Initialization
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
  {
    Serial.println("OLED allocation failed");
    while (1) { delay(100); }
  }

  display.setTextSize(1);
  display.setTextColor(WHITE);
  showMessage("Welcome to Smart Health Monitoring System.", 1000);
  showMessage("Enter patient ID\nand Name via PC", 0);

  // Load Cell Initialization
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale.set_scale(calibration_factor);
  if (!scale.wait_ready_timeout(3000)) {
    Serial.println("ERROR:LOADCELL_FAIL");
    while (1) { delay(100); }
  }
  scale.tare();

  // Temperature Sensor Initialization
  if (!mlx.begin())
  {
    showMessage("MLX90614 Error!", 3000);
    while (1) { delay(100); }
  }

  // MAX30102 Initialization (Daniel Wiese's library)
  if (sensor.begin() && sensor.setSamplingRate(kSamplingRate))
  {
    Serial.println("Sensor initialized");
  }
  else
  {
    showMessage("MAX30102 Error!", 3000);
    while (1) { delay(100); }
  }

  Serial.flush();
  while (Serial.available()) { Serial.read(); }

  Serial.println("READY");
  Serial.flush();
  delay(100);
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("System Ready");
  display.println("Awaiting Data...");
  display.display();
  currentState = WAIT_PATIENT;
  stateStartTime = millis();
}

LowPassFilter low_pass_filter_red(kLowPassCutoff, kSamplingFrequency);
LowPassFilter low_pass_filter_ir(kLowPassCutoff, kSamplingFrequency);
HighPassFilter high_pass_filter(kHighPassCutoff, kSamplingFrequency);
Differentiator differentiator(kSamplingFrequency);
MovingAverageFilter<kAveragingSamples> averager_bpm;
MovingAverageFilter<kAveragingSamples> averager_r;
MovingAverageFilter<kAveragingSamples> averager_spo2;

// Statistic for pulse oximetry
MinMaxAvgStatistic stat_red;
MinMaxAvgStatistic stat_ir;

// R value to SpO2 calibration factors
// See https://www.maximintegrated.com/en/design/technical-documents/app-notes/6/6845.html
float kSpO2_A = 1.5958422;
float kSpO2_B = -34.6596622;
float kSpO2_C = 112.6898759;

// Timestamp of the last heartbeat
long last_heartbeat = 0;

// Timestamp for finger detection
long finger_timestamp = 0;
bool finger_detected = false;

// Last diff to detect zero crossing
float last_diff = NAN;
bool crossed = false;
long crossed_time = 0;

unsigned long spo2StartTime = 0;

void handleWaitPatient()
{
  showMessage("Enter patient ID\nand Name via PC", 0);
  if (Serial.available())
  {
    if (patientID == "")
    {
      patientID = Serial.readStringUntil('\n');
      patientID.trim();
    }
    else if (patientName == "")
    {
      patientName = Serial.readStringUntil('\n');
      patientName.trim();
    }

    if (patientID != "" && patientName != "")
    {
      Serial.println("PATIENT_RECEIVED");
      showMessage("Patient Data Received.\nStarting Vitals...", 2000);
      currentState = WEIGHT;
      stateStartTime = millis();
    }
  }
}

void handleWeight()
{
  showMessage("Weight meas in 2 sec...", 2000);
  delay(2000);
  showMessage("Measuring....", 0);
  unsigned long startTime = millis();
  float weightSum = 0;
  int count = 0;

  while (millis() - startTime < 5000)
  {
    if (scale.is_ready())
    {
      weightSum += scale.get_units(5);
      count++;
    }
    delay(100);
  }

  float avgWeight = count > 0 ? weightSum / count : 0;
  avgWeight *= 240;
  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextSize(2);
  display.print("Weight:");
  display.setCursor(0, 30);
  display.print(-avgWeight/1000, 2);
  display.print(" g");
  display.display();
  delay(3000);

  Serial.print("WEIGHT:");
  Serial.println(-avgWeight/1000, 2);
  showMessage("Weight recorded.\nNext: SpO2 in 2 sec", 2000);
  currentState = SPO2;
  stateStartTime = millis();
}

void initDrawScreen(void)
{
  display.clearDisplay();

  display.setTextSize(1);      // Normal 1:1 pixel scale
  display.setTextColor(WHITE); // Draw white text
  display.setCursor(0, 0);     // Start at top-left corner
  display.println(F(""));
  display.setCursor(5, display.getCursorY());
  display.setTextSize(2);
  display.println(F("BPM  %SpO2"));
  display.display();
}
bool display_reset = true;
void displayMeasuredValues(bool no_finger, int32_t beatAvg, int32_t spo2)
{
  display.setCursor(5, 35);
  display.setTextColor(WHITE, BLACK);
  if (no_finger)
  {
    display.setTextSize(2);
    display.println(F("NO Finger            "));
    display_reset = true;
    display.display();
  }
  else if (beatAvg < 30 && display_reset)
  {
    display.setTextSize(2);
    display.println(F("Pls.  Wait             "));
    display_reset = false;
    display.display();
  }
  else if (beatAvg >= 30)
  {
    display.setTextSize(2);
    display.println(F("             "));
    display.setCursor(5, 35);
    display.setTextSize(3);
    display.print(beatAvg);
    display.print(F(" "));
    if (spo2 >= 20 && spo2 < 100)
    {
      display.print(spo2);
    }
    else if (spo2 >= 100) {
      display.print(F("99"));
    }
    else
    {
      display.print(F("--"));
    }
    display.println(F("    "));
    display.display();
  }
}

int average_bpm;
int average_r;
int average_spo2;

void handleSPO2()
{
  if (spo2StartTime == 0)
  {
    spo2StartTime = millis();
    display.clearDisplay();
    initDrawScreen();
  }

  // 20-second measurement window
  while (millis() - spo2StartTime < 20000)
  {
    auto sample = sensor.readSample(1000);
    float current_value_red = sample.red;
    float current_value_ir = sample.ir;

    // Original finger detection and processing logic
    if (sample.red > kFingerThreshold)
    {
      if (millis() - finger_timestamp > kFingerCooldownMs)
      {
        finger_detected = true;
      }
    }
    else
    {
      differentiator.reset();
      averager_bpm.reset();
      averager_r.reset();
      averager_spo2.reset();
      low_pass_filter_red.reset();
      low_pass_filter_ir.reset();
      high_pass_filter.reset();
      stat_red.reset();
      stat_ir.reset();
      finger_detected = false;
      finger_timestamp = millis();
    }

    if (finger_detected)
    {
      displayMeasuredValues(false, 0, 0);
      current_value_red = low_pass_filter_red.process(current_value_red);
      current_value_ir = low_pass_filter_ir.process(current_value_ir);
      stat_red.process(current_value_red);
      stat_ir.process(current_value_ir);

      float current_value = high_pass_filter.process(current_value_red);
      float current_diff = differentiator.process(current_value);

      if (!isnan(current_diff) && !isnan(last_diff))
      {
        if (last_diff > 0 && current_diff < 0)
        {
          crossed = true;
          crossed_time = millis();
        }
        if (current_diff > 0)
          crossed = false;

        if (crossed && current_diff < kEdgeThreshold)
        {
          if (last_heartbeat != 0 && crossed_time - last_heartbeat > 300)
          {
            int bpm = 60000 / (crossed_time - last_heartbeat);
            float rred = (stat_red.maximum() - stat_red.minimum()) / stat_red.average();
            float rir = (stat_ir.maximum() - stat_ir.minimum()) / stat_ir.average();
            float r = rred / rir;
            float spo2 = kSpO2_A * r * r + kSpO2_B * r + kSpO2_C;

            if (bpm > 50 && bpm < 250)
            {
              if (kEnableAveraging)
              {
                average_bpm = averager_bpm.process(bpm);
                average_r = averager_r.process(r);
                average_spo2 = averager_spo2.process(spo2);
                if (averager_bpm.count() >= kSampleThreshold)
                {
                  displayMeasuredValues(false, average_bpm, average_spo2);
                }
              }
              else
              {
                displayMeasuredValues(false, bpm, spo2);
              }
            }
            stat_red.reset();
            stat_ir.reset();
          }
          crossed = false;
          last_heartbeat = crossed_time;
        }
      }
      last_diff = current_diff;
    } else {
      displayMeasuredValues(true, 0, 0);
    }
  }

  // After 20 seconds, finalize and move to next state
  Serial.print("SPO2:");
  if(average_spo2 > 99) {average_spo2 = 99;}
  Serial.println(average_spo2, 1);
  Serial.print("PULSE:");
  Serial.println(average_bpm);
  displayMeasuredValues(true, average_bpm, average_spo2);
  currentState = TEMP;
  spo2StartTime = 0;
  stateStartTime = millis();
}

void handleTemperature()
{
  display.setTextSize(2);
  showMessage("Temp sense in 2 sec", 2000);
  delay(2000);
  showMessage("Sensing Temp: ");
  delay(1000);

  unsigned long startTime = millis();
  float tempSum = 0;
  int count = 0;

  while (millis() - startTime < 10000)
  {
    float tempC = mlx.readObjectTempC();
    display.clearDisplay();
    display.setCursor(0, 0);
    display.setTextSize(2);
    display.print("Temp:");
    display.setCursor(0, 30);
    display.print(tempC, 1);
    display.print(" C");
    display.display();
    tempSum += tempC;
    count++;
    delay(50);
  }

  float avgTemp = count > 0 ? tempSum / count : 0;

  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextSize(2);
  display.print("Final Temp:");
  display.setCursor(0, 30);
  display.print(avgTemp, 1);
  display.print(" C");
  display.display();
  delay(2000);

  Serial.print("TEMP:");
  Serial.println(avgTemp, 1);
  showMessage("Temp recorded.\nNext: ECG in 2 sec", 2000);
  currentState = ECG;
  stateStartTime = millis();
}

void handleECG()
{
  showMessage("ECG meas in 2 sec...", 2000);
  delay(2000);

  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("Recording ECG");
  display.display();

  Serial.println("ECG_START");
  ecgData = "";
  unsigned long startTime = millis();

  while (millis() - startTime < 30000)
  {
    int ecgValue = analogRead(ECG_PIN) / 4;
    Serial.println(ecgValue);
    ecgData += String(ecgValue) + ",";
    delay(20);
  }

  if (ecgData.endsWith(","))
  {
    ecgData.remove(ecgData.length() - 1);
  }

  Serial.println("DATA_END");

  // Wait for PC confirmation
  bool uploadConfirmed = false;
  unsigned long waitStart = millis();
  while (millis() - waitStart < 30000) { // 30-second timeout
    if (Serial.available()) {
      String response = Serial.readStringUntil('\n');
      response.trim();
      response.trim();
      Serial.print("Received: "); // Debug statement
      Serial.println(response);   // Debug statement
      if (response == "DATA_UPLOADED") {
        uploadConfirmed = true;
        break;
      }
    }
    delay(100);
  }

  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextSize(1);
  if (uploadConfirmed) {
    display.println("Data uploaded.");
    display.println("Please unplug");
    display.println("device and replug");
  } else {
    display.println("Upload failed!");
    display.println("Check connection");
  }
  display.display();
  currentState = IDLE;
}

void loop()
{
  switch (currentState)
  {
  case WAIT_PATIENT:
    handleWaitPatient();
    break;
  case WEIGHT:
    handleWeight();
    break;
  case SPO2:
    handleSPO2();
    break;
  case TEMP:
    handleTemperature();
    break;
  case ECG:
    handleECG();
    break;
  case IDLE:
    while (true);
    break;
  default:
    break;
  }
}