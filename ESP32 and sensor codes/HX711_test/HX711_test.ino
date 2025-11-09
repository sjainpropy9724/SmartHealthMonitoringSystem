#include "HX711.h"
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// HX711 circuit wiring
const int LOADCELL_DOUT_PIN = 4;
const int LOADCELL_SCK_PIN = 5;

HX711 scale;
float calibration_factor = 205.8;  

void setup() {
  Serial.begin(57600);
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale.set_scale(calibration_factor); // Set calibration factor ONCE
  scale.tare(); // Reset to zero ONCE

  // Initialize OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("SSD1306 allocation failed");
    for (;;);
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("Tare completed...");
  display.display();
  delay(2000);
}

void loop() {
  if (scale.is_ready()) {
    float weight = scale.get_units(10);  // Get weight average

    Serial.print("Weight: ");
    Serial.print(weight);
    Serial.println(" g");

    // Display on OLED
    display.clearDisplay();
    display.setCursor(0, 10);
    display.setTextSize(2);
    display.println("Weight: ");
    display.print(-weight, 2); // Show 2 decimal places
    display.print(" g");
    display.display();
  } else {
    Serial.println("HX711 not found.");
  }
  delay(1000);
}
