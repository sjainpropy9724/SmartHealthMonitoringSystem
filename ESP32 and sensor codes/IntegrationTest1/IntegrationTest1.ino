#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_MLX90614.h>
#include "MAX30105.h"
#include "spo2_algorithm.h"
#include <WiFi.h>
#include <PubSubClient.h>

// OLED Configuration
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// MLX90614 Configuration
Adafruit_MLX90614 mlx = Adafruit_MLX90614();

// MAX30102 Configuration
MAX30105 particleSensor;

// WiFi/MQTT Configuration
const char* ssid = "Swarit realme";
const char* password = "swarit@123";
const char* mqtt_server = "industrial.api.ubidots.com";
const char* token = "BBUS-8AeNsL5TMlF6cxdWm1CzBwhmLmONCp"; // Use API token from Ubidots API Credentials
const char* device_label = "esp32_healthmonitor";

WiFiClient espClient;
PubSubClient client(espClient);

// ECG Configuration
const int ECG_PIN = 35;
const int ECG_DURATION = 15000; // 15 seconds

// State Machine Variables
enum State { TEMP, SPO2, ECG, IDLE };
State currentState = TEMP;
unsigned long stateStartTime;

// MQTT Payload Buffers
char payload[700];
char topic[150];

// Function Prototypes
void initializeOLED();
void connectToWiFi();
void initializeMQTT();
void publishData(float temp, int bpm, int spo2, String ecgData);
void handleTemperature();
void handleSpO2();
void handleECG();
void showCompletionMessage();

void setup() {
    Serial.begin(115200);
    initializeOLED();
    connectToWiFi();
    initializeMQTT();

    // Initialize MLX90614
    if (!mlx.begin()) {
        display.clearDisplay();
        display.setCursor(0, 0);
        display.println("MLX90614 Error");
        display.display();
        while (1);
    }

    // Initialize MAX30102
    if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) {
        display.clearDisplay();
        display.setCursor(0, 0);
        display.println("MAX30102 Error");
        display.display();
        while (1);
    }
    particleSensor.setup(0x1F, 4, 2, 100, 411, 4096);

    currentState = TEMP;
    stateStartTime = millis();
}

void loop() {
    if (!client.connected()) client.connect("ESP32Client", token, "");
    client.loop();

    switch (currentState) {
        case TEMP:
            handleTemperature();
            break;

        case SPO2:
            handleSpO2();
            break;

        case ECG:
            handleECG();
            break;

        case IDLE:
            if (Serial.available()) {
                String input = Serial.readStringUntil('\n');
                if (input.indexOf("YES") != -1) {
                    currentState = TEMP;
                    stateStartTime = millis();
                }
            }
            break;
    }
}

// OLED Initialization
void initializeOLED() {
    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Use correct I2C address
        Serial.println("OLED initialization failed");
        while (1);
    }
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    display.println("System Ready");
    display.display();
    delay(2000);
}

// WiFi Connection
void connectToWiFi() {
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWiFi Connected");
}

// MQTT Initialization
void initializeMQTT() {
    client.setServer(mqtt_server, 1883);
}

// Publish Data to Ubidots
void publishData(float temp, int bpm, int spo2, String ecgData) {
    if (!client.connected()) client.connect("ESP32Client", token, "");

    sprintf(topic, "/v1.6/devices/%s", device_label);

    sprintf(payload,
            "{\"temperature\":{\"value\":%.2f},"
            "\"bpm\":{\"value\":%d},"
            "\"spo2\":{\"value\":%d},"
            "\"ecg\":{\"value\":\"%s\"}}",
            temp,
            bpm,
            spo2,
            ecgData.c_str());

    client.publish(topic, payload);
}

// Handle Temperature Measurement
void handleTemperature() {
    static float tempReadings[50];
    static int index = 0;

    if (millis() - stateStartTime < 3000) {
        display.clearDisplay();
        display.setCursor(0, 0);
        display.println("Place finger on");
        display.println("Temp sensor...");
        display.display();
        return;
    }

    tempReadings[index++] = mlx.readObjectTempC();

    if (index >= 50) {
        float avgTemp = 0;
        for (int i = 0; i < 50; i++) avgTemp += tempReadings[i];
        avgTemp /= 50;

        display.clearDisplay();
        display.setCursor(0, 0);
        display.print("Temp: ");
        display.print(avgTemp, 1);
        display.println(" C");
        display.display();

        delay(5000); // Show reading for 5 seconds

        publishData(avgTemp, 0 /* BPM */, 0 /* SpO2 */, "");
        currentState = SPO2;
        stateStartTime = millis();
    }
}

// Handle SpO2 Measurement
void handleSpO2() {
    static int32_t spo2, heartRate;
    static int8_t validSPO2, validHeartRate;
    static uint32_t irBuffer[100], redBuffer[100];

    if (millis() - stateStartTime < 3000) {
        display.clearDisplay();
        display.setCursor(0, 0);
        display.println("Place finger on");
        display.println("SpO2 sensor...");
        display.display();
        return;
    }

    for (int i = 0; i < 100; i++) {
        redBuffer[i] = particleSensor.getRed();
        irBuffer[i] = particleSensor.getIR();
        particleSensor.nextSample();
    }

    maxim_heart_rate_and_oxygen_saturation(irBuffer, 100, redBuffer,
                                           &spo2, &validSPO2,
                                           &heartRate, &validHeartRate);

    if (validSPO2 && validHeartRate) {
        display.clearDisplay();
        display.setCursor(0, 0);
        display.print("BPM: ");
        display.println(heartRate);
        display.print("SpO2: ");
        display.println(spo2);
        display.display();

        delay(5000); // Show reading for 5 seconds

        publishData(0.0f /* Temp */, heartRate /* BPM */, spo2 /* SpO2 */, "");
        currentState = ECG;
        stateStartTime = millis();
    }
}


// Handle ECG Measurement
void handleECG() {
    static String ecgData;
    static unsigned long startTime;
    static bool initialized = false;

    // Initialize on first entry to function
    if (!initialized) {
        ecgData = "";
        startTime = millis();
        initialized = true;
        
        display.clearDisplay();
        display.setCursor(0, 0);
        display.println("Monitoring ECG...");
        display.display();
    }

    if (millis() - stateStartTime < ECG_DURATION) {
        if (millis() - startTime > 20) { // Sample at ~50Hz
            ecgData += String(analogRead(ECG_PIN)) + ",";
            startTime = millis();
        }
        return;
    }

    // Remove last comma if string is not empty
    if (ecgData.length() > 0) {
        ecgData.remove(ecgData.length() - 1);
    }

    // Publish ECG data
    publishData(0.0f /* Temp */, 0 /* BPM */, 0 /* SpO2 */, ecgData);
    
    // Show completion message
    showCompletionMessage();
    
    // Reset for next cycle
    initialized = false;
    
    // Move to idle state
    currentState = IDLE;
}

// Show completion message on OLED
void showCompletionMessage() {
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("Sensing Complete");
    display.println("Send 'YES' to restart");
    display.display();
}

