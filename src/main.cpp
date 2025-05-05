#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <time.h>

const char* ssid = "ssid";                              // fix wifi creds before demo
const char* password = "password";

int micPin = 34;  
int micValue = 0; 

const char* url = "http://localhost:8080/api/noise/add"; // fix ip address before demo

WiFiUDP udp;

float analogToDecibel(int value);

void setup() {
  Serial.begin(115200);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to Wi-Fi...");
  }
  Serial.println("Connected to Wi-Fi");

  pinMode(micPin, INPUT);
}

void loop() {
  micValue = analogRead(micPin);
  
  JsonDocument doc;

  // doc["id"] = millis();
  doc["noiseLevel"] = (double) analogToDecibel(micValue);
  doc["latitude"] = (double) 20.980928;       // uni location
  doc["longitude"] = (double) 105.787401;
  doc["timestamp"] = "none"; // time handled by backend

  String json;
  serializeJson(doc, json);

  HTTPClient http;
  http.begin(url);
  http.addHeader("Content-Type", "application/json");

  if (http.POST(json)) {
    Serial.println("Data:");
    Serial.print("Raw value: ");
    Serial.println(micValue);
    Serial.print("Decibel value: ");
    Serial.println(analogToDecibel(micValue));
    Serial.println("Sent");
  } else {
    Serial.print("Error sending data");
    // Serial.println(http.errorToString(http.errorCode()));
  }
  http.end();

  delay(2000);
}

float analogToDecibel(int value) {
    float voltage = value * (5.0 / 4095.0);
    return -20 * log10(voltage / 5.0);
  }