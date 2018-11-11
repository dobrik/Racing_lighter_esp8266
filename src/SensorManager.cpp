//
// Created by Dobrik on 11/1/2018.
//

#include "SensorManager.h"
#define USE_SERIAL Serial

void SensorManager::loop() {
    frontSensor->check(200);
    rearSensor->check(200);

    if (frontSensor->state == HIGH) {
        if (rearSensor->state == HIGH) {
            state = STATE_READY;
        } else {
            state = STATE_BACKWARD;
        }
    } else if (rearSensor->state == HIGH) {
        state = STATE_FORWARD;
    } else {
        state = STATE_WAIT;
    }

    data = (frontSensor->state << frontSensor->dataByte)|(rearSensor->state << rearSensor->dataByte);
    if (state != statePrev) {
        USE_SERIAL.printf("Sensor manager event state updated = '%d'\n", state);
        USE_SERIAL.printf("Sensor data = '%d'\n", data);
        runEvent(state);
        statePrev = state;
    }
}

void SensorManager::onUpdate(SensorManager::SensorManagerEvent _event) {
    event = _event;
}

void SensorManager::runEvent(SMState state) {
    if (event) {
        event(state, &data);
    }
}