#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define SENSOR_PIN 21
#define ONE_WIRE_BUS 4 

const char* ssid = "";
const char* password = "";

const char* serverURL = "";

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

int logCounter = 0;
int serverKeyState = -1;

void setup() {
    Serial.begin(115200);
    WiFi.begin(ssid, password);

    pinMode(SENSOR_PIN, INPUT_PULLUP);

    sensors.begin();    

    Serial.print("Connecting to Wi-Fi");
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.print(".");
    }

}

void loop() {
  
  if (WiFi.status() == WL_CONNECTED) {

    sensors.requestTemperatures();  // Send command to get temperature
    float temperature = sensors.getTempCByIndex(0);  // Read first sensor

    int keystate = digitalRead(SENSOR_PIN);
    // if (keystate != serverKeyState){
      String request = String(serverURL) + "/update?state=" + String(keystate) + "&count=" + String(logCounter) + "&temp=" + String(temperature);
      HTTPClient http;
      http.begin(request);
      int responseCode = http.GET();
      Serial.println(http.getString()); 
      http.end();
    // }
      
    logCounter++;
    if (logCounter > 99){
      logCounter = 0;
    };

  } else {
    Serial.println("Disconnected! Reconnecting...");
    WiFi.disconnect();
    WiFi.reconnect();
  }
  
  delay(2000);
}