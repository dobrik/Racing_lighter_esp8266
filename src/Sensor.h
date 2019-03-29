//
// Created by Dobrik on 11/1/2018.
//

#ifndef WS_SENSOR_H
#define WS_SENSOR_H


#include <cstdint>
#include <functional>
#include <Arduino.h>
#include "Light.h"

class Sensor {

public:
    Sensor(uint8_t sensorPin);

    void check(uint16_t checkInterval);

    uint8_t pin;
    int state = LOW;
private:
    uint32_t lastCheck = 0;
};


#endif //WS_SENSOR_H
