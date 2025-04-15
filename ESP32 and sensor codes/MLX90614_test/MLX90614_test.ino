#include <Wire.h>
#include <Adafruit_MLX90614.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128  // OLED width
#define SCREEN_HEIGHT 64  // OLED height

#define OLED_RESET -1      // Reset pin (not used for SSD1306)
#define SCREEN_ADDRESS 0x3C // OLED I2C address

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
Adafruit_MLX90614 mlx = Adafruit_MLX90614();

void setup() {
    Serial.begin(115200);
    Wire.begin();

    // Initialize OLED
    if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
        Serial.println("SSD1306 initialization failed");
        for (;;); // Halt if initialization fails
    }
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);

    // Initialize MLX90614
    if (!mlx.begin()) {
        Serial.println("MLX90614 initialization failed");
        while (1);
    }

    Serial.println("MLX90614 and OLED initialized successfully");
}

void loop() {
    float ambientTemp = mlx.readAmbientTempC();  // Get ambient temperature
    float objectTemp = mlx.readObjectTempC();    // Get object temperature

    Serial.print("Ambient Temp: ");
    Serial.print(ambientTemp);
    Serial.print(" °C\t Object Temp: ");
    Serial.print(objectTemp);
    Serial.println(" °C");

    // Display on OLED
    display.clearDisplay();
    display.setCursor(0, 10);
    display.print("Ambient: ");
    display.print(ambientTemp);
    display.print(" C");

    display.setCursor(0, 30);
    display.print("Object: ");
    display.print(objectTemp);
    display.print(" C");

    display.display();
    delay(1000); // Refresh every second
}
