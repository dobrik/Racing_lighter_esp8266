//
// Created by Dobrik on 11/1/2018.
//

#include <Arduino.h>
#include "Sensor.h"

Sensor::Sensor(uint8_t sensorPin, uint8_t _dataByte) {
    pinMode(sensorPin, INPUT);
    pin = sensorPin;
    dataByte = _dataByte;
}

void Sensor::check(uint16_t checkInterval) {
    if (digitalRead(pin) != state && (millis() - lastCheck) > checkInterval) {
        state = digitalRead(pin);
        lastCheck = millis();
    }
}