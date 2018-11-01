//
// Created by Dobrik on 11/1/2018.
//

#ifndef WS_SENSOR_H
#define WS_SENSOR_H


#include <cstdint>
#include <functional>
#include <Arduino.h>

typedef enum {
    SENSOR_ACTIVE = HIGH,
    SENSOR_DISABLED = LOW
} SEType;


class Sensor {

public:
    typedef std::function<void (SEType event_type)> SensorEvent;

    Sensor(uint8_t sensorPin);

    void check(uint16_t checkInterval);

    uint8_t pin;
    int current_state = LOW;
private:
    SensorEvent event;

    uint32_t lastCheck = 0;

    void runEvent(SEType event_type);
};


#endif //WS_SENSOR_H
