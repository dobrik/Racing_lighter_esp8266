//
// Created by Dobrik on 11/1/2018.
//

#ifndef WS_SENSORMANAGER_H
#define WS_SENSORMANAGER_H


#include "Sensor.h"
#include "Light.h"
#include "../lib/virtualDelay/avdweb_VirtualDelay.h"

typedef enum {
    SENSOR_WAIT,
    SENSOR_READY,
    SENSOR_FORWARD,
    SENSOR_BACKWARD
} SMState;

class SensorManager {
public:
    SensorManager(Sensor *_frontSensor, Sensor *_rearSensor) : frontSensor(_frontSensor), rearSensor(_rearSensor) {};

    void loop();

    typedef std::function<void (SMState state, SensorManager *manager)> SensorManagerEvent;

    typedef std::function<void (SensorManager *manager, uint8_t iteration)> SensorManagerOnFalseStartEvent;

    void falseStart();

    void reset();

    void onUpdate(SensorManagerEvent _event);

    void onFalseStart(SensorManagerOnFalseStartEvent _event);

    VirtualDelay Delay;

    Sensor *frontSensor;
    Sensor *rearSensor;

    SMState state = SENSOR_WAIT;
    SMState statePrev = SENSOR_WAIT;

    uint8_t callbackIteration = 0;
    long functionDelay = 0;

private:
    SensorManagerEvent event;

    SensorManagerOnFalseStartEvent falseStartEvent;

    void runEvent(SMState state, SensorManager *manager);
};


#endif //WS_SENSORMANAGER_H
