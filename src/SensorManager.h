//
// Created by Dobrik on 11/1/2018.
//

#ifndef WS_SENSORMANAGER_H
#define WS_SENSORMANAGER_H


#include "Sensor.h"
#include "Light.h"

class SensorManager {
public:
    SensorManager(Sensor *_frontSensor, Sensor *_rearSensor) : frontSensor(_frontSensor), rearSensor(_rearSensor) {};

    void SensorManager::loop();

    typedef enum {
        STATE_WAIT,
        STATE_READY,
        STATE_FORWARD,
        STATE_BACKWARD
    } SMState;

    typedef enum {
        EVENT_SENSOR_UPDATE
    } SMEvent;

    uint8_t data = 0x00;

    Sensor *frontSensor;
    Sensor *rearSensor;

    SMState state = STATE_WAIT;

private:
    void init();
};


#endif //WS_SENSORMANAGER_H
