//
// Created by Dobrik on 11/1/2018.
//

#ifndef WS_SENSOR_H
#define WS_SENSOR_H


#include <cstdint>
#include <functional>
#include <Arduino.h>
#include "Light.h"

typedef enum {
    SENSOR_ENABLED = HIGH,
    SENSOR_DISABLED = LOW
} SEType;


class Sensor {

public:
    typedef std::function<void (SEType event_type, Light *_light)> SensorEvent;
    Sensor(uint8_t sensorPin, Light *_light);
    void onEvent(SensorEvent _event);

    void check(uint16_t checkInterval);

    uint8_t pin;
    int current_state = LOW;
private:
    SensorEvent event;

    Light *light;

    uint32_t lastCheck = 0;

    void runEvent(SEType event_type);
};


#endif //WS_SENSOR_H
