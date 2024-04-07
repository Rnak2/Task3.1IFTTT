#include <Wire.h>
#include <WiFiNINA.h>
#include "secret.h"
#include <BH1750.h>
BH1750 lightMeter;

// Please enter your sensitive data in the Secret tab
char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASS;

WiFiClient client;

char HOST_NAME[] = "maker.ifttt.com";
String PATH_NAME_ON = "/trigger/terrarium_sunlight_start/with/key/b0GGmp9DzmjuUjPq81sZ1I"; 
String PATH_NAME_OFF = "/trigger/terrarium_sunlight_stop/with/key/b0GGmp9DzmjuUjPq81sZ1I"; 
String queryString = "?value1=";
const float light_value = 100;
bool prevlightstatus = false;

void setup() {
  // Initialize BH1750 sensor with specified SDA and SCL pins
  Wire.begin();
  lightMeter.begin();

  // Initialize serial communication
  Serial.begin(9600);
  while (!Serial);

  // Initialize WiFi connection
  WiFi.begin(ssid, pass);
  Serial.println("Connecting to WiFi");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
}

void loop() {
  
  float lux = lightMeter.readLightLevel();
  bool sunlightStatus = lux > light_value;
  

  if (sunlightStatus) {
    Serial.println("Sunlight");
  } else {
    Serial.println("No Sunlight");
  }

 
  if (sunlightStatus != prevlightstatus) {
    if (sunlightStatus) {
      triggerIFTTTNotification(PATH_NAME_ON);
    } else {
      triggerIFTTTNotification(PATH_NAME_OFF);
    }
    prevlightstatus = sunlightStatus;
  }

  
  delay(5000);
}

void triggerIFTTTNotification(String PATH_NAME) {
  
  if (client.connect(HOST_NAME, 80)) {
    // Send HTTP request
    client.print("GET " + PATH_NAME + queryString + " HTTP/1.1\r\n");
    client.print("Host: " + String(HOST_NAME) + "\r\n");
    client.print("Connection: close\r\n");
    client.print("\r\n");

    Serial.println("Sending message to IFTTT");

    unsigned long timeout = millis();
    while (!client.available()) {
      if (millis() - timeout > 5000) { 
        Serial.println("Timeout");
        client.stop();
        return;
      }
    }

    // Print response from server
    while (client.available()) {
      char c = client.read();
      Serial.print(c);
    }

    // Close connection
    client.stop();
  } else {
    Serial.println("Failed to connect to IFTTT server.");
  }
}
