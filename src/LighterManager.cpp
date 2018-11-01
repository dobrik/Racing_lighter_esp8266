//
// Created by Dobrik on 9/23/2018.
//

#include "ESP8266WiFi.h"
#include "LighterManager.h"
#define USE_SERIAL Serial

LighterManager::LighterManager(Light _lights[LIGHTS_COUNT]) {
    lights = _lights;
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
    USE_SERIAL.println("lighterTestStart call");
    callback = &LighterManager::lighterTestFunction;
}

void LighterManager::lighterStart(int delay) {
    functionDelay = delay;
    USE_SERIAL.println("lighterStart call");
    callback = &LighterManager::lighterStartFunction;
}

void LighterManager::onEvent(LighterManagerEvent _event){
    event = _event;
}

void LighterManager::runEvent(LMEType event_type, uint8_t * payload, size_t length) {
    if(event) {
        event(event_type, payload, length);
    }
}

void LighterManager::lighterTestFunction() {
    USE_SERIAL.println("lighterTestFunction callback call");
    if (lighterTestIteration >= sizeof(LIGHTER_TEST_STEPS)) {
        lighterData = 0x00;
        lighterTestIteration = 0;
        functionDelay = 0;
    } else {
        lighterData ^= lights[LIGHTER_TEST_STEPS[lighterTestIteration]].dataByte;
        lighterTestIteration++;
    }

    runEvent(LIGHTER_UPDATE, &lighterData, sizeof(lighterData));
}

void LighterManager::lighterStartFunction() {
    USE_SERIAL.println("lighterStartFunction callback call");
    if (lighterStartIteration >= sizeof(LIGHTER_START_STEPS)) {
        lighterData = 0x00;
        lighterStartIteration = 0;
        functionDelay = 0;
    } else {
        lighterData ^= lights[LIGHTER_START_STEPS[lighterStartIteration]].dataByte;
        lighterStartIteration++;
    }

    runEvent(LIGHTER_UPDATE, &lighterData, sizeof(lighterData));
}
