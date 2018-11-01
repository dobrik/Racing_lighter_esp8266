//
// Created by Dobrik on 11/1/2018.
//

#include <Arduino.h>
#include "Sensor.h"

Sensor::Sensor(uint8_t sensorPin, Light *_light) {
    pinMode(sensorPin, INPUT);
    pin = sensorPin;
    light = _light;
}

void Sensor::check(uint16_t checkInterval) {
    if (digitalRead(pin) != current_state && (millis() - lastCheck) > checkInterval) {
        current_state = digitalRead(pin);

        switch (digitalRead(pin)) {
            case HIGH:
                runEvent(SENSOR_ENABLED);
                break;
            case LOW:
                runEvent(SENSOR_DISABLED);
                break;
        }
        lastCheck = millis();
    }
}

void Sensor::runEvent(SEType event_type) {
    if (event) {
        event(event_type, light);
    }
}

void Sensor::onEvent(Sensor::SensorEvent _event) {
    event = _event;
}