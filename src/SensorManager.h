//
// Created by Dobrik on 11/1/2018.
//

#ifndef WS_SENSORMANAGER_H
#define WS_SENSORMANAGER_H


#include "Sensor.h"
#include "Light.h"

typedef enum {
    STATE_WAIT,
    STATE_READY,
    STATE_FORWARD,
    STATE_BACKWARD
} SMState;

class SensorManager {
public:
    SensorManager(Sensor *_frontSensor, Sensor *_rearSensor) : frontSensor(_frontSensor), rearSensor(_rearSensor) {};

    void loop();

    typedef std::function<void (SMState state, uint8_t *payload)> SensorManagerEvent;

    void onUpdate(SensorManagerEvent _event);

    uint8_t data = 0x00;
    Sensor *frontSensor;
    Sensor *rearSensor;

    SMState state = STATE_WAIT;
    SMState statePrev = STATE_WAIT;

private:
    SensorManagerEvent event;
    void runEvent(SMState state);
};


#endif //WS_SENSORMANAGER_H
