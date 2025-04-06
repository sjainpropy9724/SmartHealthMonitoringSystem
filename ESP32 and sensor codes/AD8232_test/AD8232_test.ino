#define ECG_PIN 39  // ADC input pin

void setup() {
  Serial.begin(9600);
}

void loop() {
  int rawECG = analogRead(ECG_PIN);  
  int zoomedOutECG = rawECG / 4;  // Reduce amplitude for zooming out

  Serial.println(zoomedOutECG);  // Send scaled data to Serial Plotter
  delay(10);
}
