//
// Created by Dobrik on 11/1/2018.
//

#include "SensorManager.h"

#define USE_SERIAL Serial

void SensorManager::loop() {
    frontSensor->check(200);
    rearSensor->check(200);

    if (frontSensor->state == LOW) {
        if (rearSensor->state == LOW) {
            state = SENSOR_READY;
        } else {
            state = SENSOR_BACKWARD;
        }
    } else if (rearSensor->state == LOW) {
        state = SENSOR_FORWARD;
    } else {
        state = SENSOR_WAIT;
    }

    if (functionDelay != 0 && falseStartEvent) {
        Delay.start(functionDelay);
        if (Delay.elapsed()) {
            falseStartEvent(this, callbackIteration);
            callbackIteration++;
        }
    }

    if (state != statePrev) {
        USE_SERIAL.printf("Sensor manager event state updated = '%d'\n", state);
        runEvent(state, this);
        statePrev = state;
    }
}

void SensorManager::falseStart() {
    functionDelay = 500;
}

void SensorManager::reset(){
    functionDelay = 0;
}

void SensorManager::onUpdate(SensorManager::SensorManagerEvent _event) {
    event = _event;
}

void SensorManager::runEvent(SMState state, SensorManager *manager) {
    if (event) {
        event(state, manager);
    }
}

void SensorManager::onFalseStart(SensorManager::SensorManagerOnFalseStartEvent _event) {
    falseStartEvent = _event;
}
