//
// Created by Dobrik on 9/23/2018.
//

#include "ESP8266WiFi.h"
#include "LighterManager.h"

#define USE_SERIAL Serial

void LighterManager::loop() {
    if (functionDelay != 0) {
        Delay.start(functionDelay);
        if (Delay.elapsed()) {
            (this->*((LighterManager *) this)->LighterManager::callback)();
        }
    }
}

void LighterManager::lighterTestStart(int delay) {
    state = LIGHTER_STATE_TEST;
    functionDelay = delay;
    USE_SERIAL.println("lighterTestStart call");
    callback = &LighterManager::lighterTestFunction;
}

void LighterManager::lighterStart(int delay) {
    state = LIGHTER_STATE_COUNTDOWN;
    functionDelay = delay;
    USE_SERIAL.println("lighterStart call");
    callback = &LighterManager::lighterStartFunction;
}

void LighterManager::reset() {
    functionDelay = 0;
    callbackIteration = 0;
    state = LIGHTER_STATE_READY;
    stage = LIGHTER_STAGE_STANDBY;
    callback = nullptr;
    runEvent(stage);
}

void LighterManager::onEvent(LighterManagerEvent _event) {
    event = _event;
}

void LighterManager::runEvent(LighterStage stage) {
    if (event) {
        event(stage);
    }
}

void LighterManager::lighterTestFunction() {
    if (callbackIteration >= (sizeof(LIGHTER_TEST_STEPS) / sizeof(LighterStage))) {
        reset();
    } else {
        stage = LIGHTER_TEST_STEPS[callbackIteration];
        callbackIteration++;
        runEvent(stage);
    }

}

void LighterManager::lighterStartFunction() {
    if (callbackIteration >= (sizeof(LIGHTER_START_STEPS) / sizeof(LighterStage))) {
        reset();
    } else {
        stage = LIGHTER_START_STEPS[callbackIteration];
        callbackIteration++;
        runEvent(stage);
    }
}
