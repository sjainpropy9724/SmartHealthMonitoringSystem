#include "WiFi.h"
#include "HTTPClient.h"

const char* ssid = "Swarit\'s realme";     // Replace with your WiFi name
const char* password = "swarit@123"; // Replace with your WiFi password

void setup() {
    Serial.begin(115200);
    WiFi.begin(ssid, password);

    Serial.print("Connecting to WiFi...");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("\nWiFi connected!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());  // Print assigned IP address
}

void loop() {
    if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;
        http.begin("http://worldtimeapi.org/api/timezone/Etc/UTC"); // This might be down
        int httpResponseCode = http.GET();

        if (httpResponseCode > 0) {
            Serial.print("HTTP Response code: ");
            Serial.println(httpResponseCode);
            String payload = http.getString();
            Serial.println(payload);  // Print response
        } else {
            Serial.println("Error fetching from WorldTimeAPI, trying Google...");
            http.end();

            // Try Google API as a backup
            http.begin("http://clients3.google.com/generate_204"); // Google connectivity check
            httpResponseCode = http.GET();

            if (httpResponseCode == 204) {
                Serial.println("✅ Internet ! (Google response: 204 No Content)");
            } else {
                Serial.println("❌ No Internet Access! Check connection.");
            }
        }
        http.end();
    } else {
        Serial.println("WiFi Disconnected!");
    }

    delay(10000);  // Wait 10 seconds before next request
}
