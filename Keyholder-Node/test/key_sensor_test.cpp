#include <Arduino.h>

#define SENSOR_PIN 21
#define LED_PIN 2


void setup() {
    Serial.begin(115200);

    pinMode(SENSOR_PIN, INPUT);
    pinMode(LED_PIN, OUTPUT);

}

void loop() {
    
    int sensor = digitalRead(SENSOR_PIN);

    if (sensor == 1){
        digitalWrite(LED_PIN, HIGH);
    } else {
        digitalWrite(LED_PIN, LOW);
    };

delay(800);

}