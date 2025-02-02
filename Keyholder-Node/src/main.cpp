#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include "secrets.h"

#define KEY_SENSOR_PIN 21
#define TEMPERATURE_SENSOR_PIN 4

const char* ssid = WIFI_SSID;
const char* password = WIFI_PASSWORD;
const char* serverURL = SERVER_IP;

OneWire oneWireInstance(TEMPERATURE_SENSOR_PIN);
DallasTemperature sensors(&oneWireInstance);

int temperatureReadTimer = 0;
int keySensorState = -1;
int temperatureReadsSinceLastLogged = 0;
float temperatureReads[12];


void setup() {
    WiFi.begin(ssid, password);
    pinMode(TEMPERATURE_SENSOR_PIN, INPUT_PULLDOWN);
    sensors.begin();

    // Serial.begin(115200);
    // while (WiFi.status() != WL_CONNECTED) {
    //     delay(1000);
    //     Serial.print(".");
    // }
}


void loop() {
  if (WiFi.status() == WL_CONNECTED) {

    // node logic

    if (digitalRead(KEY_SENSOR_PIN) != keySensorState){
      int keyChangeCount = 0;
      do
      {
        keyChangeCount++;
        if (keyChangeCount >= 250){
          keySensorState = digitalRead(KEY_SENSOR_PIN);
          HTTPClient http;
          String request = String(serverURL) + "/update?state=" + String(keySensorState);
          http.begin(request);
          http.end();
          break;
        }
        delay(1);
      } while (digitalRead(KEY_SENSOR_PIN) != keySensorState);
      
    } else {
      temperatureReadTimer++;
      if (temperatureReadTimer >= 1000){

        if (temperatureReadsSinceLastLogged >= 11){
          //calculate average of all logs and emit new data to the server
          float sum = 0;
          for (int i=0; i < 11; i++){
            sum += temperatureReads[i];
          }
          float averageTemperature = sum / 12;
          HTTPClient http;
          String request = String(serverURL) + "/update?temp=" + String(averageTemperature);
          http.begin(request);
          int responseCode = http.GET();
          // Serial.println(http.getString()); 
          http.end();
          temperatureReadsSinceLastLogged = 0;

        } else {
          temperatureReads[temperatureReadsSinceLastLogged] = digitalRead(TEMPERATURE_SENSOR_PIN);
          temperatureReadsSinceLastLogged++;
        }
      }
    }
    delay(10);
  } else {
    Serial.println("Disconnected! Reconnecting...");
    WiFi.disconnect();
    WiFi.reconnect();
  }
}