#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128  // OLED width
#define SCREEN_HEIGHT 64  // OLED height
#define OLED_RESET -1     // Reset pin (not used)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup() {
    Serial.begin(115200);

    // Initialize OLED display
    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
        Serial.println(F("SSD1306 allocation failed"));
        for (;;); // Halt
    }

    display.clearDisplay();
    display.display();
}

void loop() {
    int radius = 3; // Circle size

    // Move from (0,0) to (128,0) (Top-left to Top-right)
    for (int x = 0; x < SCREEN_WIDTH; x += 2) {
        drawCircleAt(x, 0, radius);
    }

    // Move from (128,0) to (128,64) (Top-right to Bottom-right)
    for (int y = 0; y < SCREEN_HEIGHT; y += 2) {
        drawCircleAt(SCREEN_WIDTH - 1, y, radius);
    }

    // Move diagonally from (128,64) to (0,0) (Bottom-right to Top-left)
    for (int x = SCREEN_WIDTH - 1, y = SCREEN_HEIGHT - 1; x >= 0 && y >= 0; x -= 2, y -= 1) {
        drawCircleAt(x, y, radius);
    }

    // Move from (0,0) to (0,64) (Top-left to Bottom-left)
    for (int y = 0; y < SCREEN_HEIGHT; y += 2) {
        drawCircleAt(0, y, radius);
    }

    // Move from (0,64) to (128,64) (Bottom-left to Bottom-right)
    for (int x = 0; x < SCREEN_WIDTH; x += 2) {
        drawCircleAt(x, SCREEN_HEIGHT - 1, radius);
    }
}

// Function to draw and refresh the circle
void drawCircleAt(int x, int y, int r) {
    display.clearDisplay();
    display.fillCircle(x, y, r, SSD1306_WHITE);
    display.display();
    delay(20); // Adjust for speed
}
