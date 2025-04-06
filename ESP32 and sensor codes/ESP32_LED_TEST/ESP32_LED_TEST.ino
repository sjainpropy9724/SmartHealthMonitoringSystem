const int ledPin = 13;  // LED connected to GPIO 2

void setup() {
    pinMode(ledPin, OUTPUT);  // Set GPIO 2 as an output
}

void loop() {
    digitalWrite(ledPin, HIGH);  // Turn LED ON
    delay(500);                  // Wait for 500ms
    digitalWrite(ledPin, LOW);   // Turn LED OFF
    delay(500);                  // Wait for 500ms
}
