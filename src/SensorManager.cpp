//
// Created by Dobrik on 11/1/2018.
//

#include "SensorManager.h"

void SensorManager::loop() {
    rearSensor->check(200);
    frontSensor->check(200);
}

void SensorManager::init() {
    void backwardUpdate(SEType event_type) {
        switch (event_type) {
            case SENSOR_ENABLED:
                break;
            case SENSOR_DISABLED:
                break;
        }
    }
}
