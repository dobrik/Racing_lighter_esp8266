//
// Created by Dobrik on 9/23/2018.
//

#include "LighterManager.h"

LighterManager::LighterManager(Light *lights) {
    this->FIRST_YELLOW_LIGHT = &lights[0];
    this->SECOND_YELLOW_LIGHT = &lights[1];
    this->THIRD_YELLOW_LIGHT = &lights[2];
    this->GREEN_LIGHT = &lights[3];
    this->RED_LIGHT = &lights[4];
}

void LighterManager::loop() {
    if (functionDelay != 0) {
        Delay.start(functionDelay);
        if (Delay.elapsed()) {
            (this->*((LighterManager*)this)->LighterManager::callback)();
        }
    }
}

void LighterManager::lighterTestStart(int delay) {
    functionDelay = delay;
    callback = &LighterManager::lighterTestFunction;
}

void LighterManager::lighterStart(int delay) {
    functionDelay = delay;
    callback = &LighterManager::lighterStartFunction;
}

void LighterManager::updateLighterState(uint8_t data) {
    if ((data & FIRST_YELLOW_LIGHT->dataByte) == FIRST_YELLOW_LIGHT->dataByte) {
        FIRST_YELLOW_LIGHT->setOn();
    } else {
        FIRST_YELLOW_LIGHT->setOff();
    }
    if ((data & SECOND_YELLOW_LIGHT->dataByte) == SECOND_YELLOW_LIGHT->dataByte) {
        SECOND_YELLOW_LIGHT->setOn();
    } else {
        SECOND_YELLOW_LIGHT->setOff();
    }
    if ((data & THIRD_YELLOW_LIGHT->dataByte) == THIRD_YELLOW_LIGHT->dataByte) {
        THIRD_YELLOW_LIGHT->setOn();
    } else {
        THIRD_YELLOW_LIGHT->setOff();
    }
    if ((data & GREEN_LIGHT->dataByte) == GREEN_LIGHT->dataByte) {
        GREEN_LIGHT->setOn();
    } else {
        GREEN_LIGHT->setOff();
    }
    if ((data & RED_LIGHT->dataByte) == RED_LIGHT->dataByte) {
        RED_LIGHT->setOn();
    } else {
        RED_LIGHT->setOff();
    }
}

void LighterManager::lighterTestFunction() {
    if (lighterTestIteration >= sizeof(LIGHTER_TEST_STEPS)) {
        lighterData = 0x00;
        lighterTestIteration = 0;
        functionDelay = 0;
    } else {
        lighterData ^= LighterManager::LIGHTER_TEST_STEPS[lighterTestIteration]->dataByte;
        lighterTestIteration++;
    }

//    updateLighterState(lighterData);

}

void LighterManager::lighterStartFunction() {
//    if (lighterStartIteration >= sizeof(LIGHTER_START_STEPS)) {
//        lighterData = 0x00;
//        lighterStartIteration = 0;
//        functionDelay = 0;
//    } else {
//        lighterData ^= LIGHTER_START_STEPS[lighterStartIteration]->dataByte;
//        lighterStartIteration++;
//    }
//
//    updateLighterState(lighterData);
}
