//
// Created by Dobrik on 9/23/2018.
//

#ifndef WS_LIGHTERMANAGER_H
#define WS_LIGHTERMANAGER_H


#include <cstdint>
#include "Light.h"
#include "../lib/virtualDelay/avdweb_VirtualDelay.h"

class LighterManager {
public:
    LighterManager(Light *lights);

    void updateLighterState(uint8_t data);

    void lighterTestStart(int delay = 500);

    void lighterStart(int delay = 500);

    //TODO: create on state changed listeners
    void loop();

    void lighterStartFunction();

    void lighterTestFunction();

private:
    VirtualDelay Delay;

    long functionDelay = 0;

    void (LighterManager::* callback)(void);

    Light *FIRST_YELLOW_LIGHT;
    Light *SECOND_YELLOW_LIGHT;
    Light *THIRD_YELLOW_LIGHT;
    Light *GREEN_LIGHT;
    Light *RED_LIGHT;

    Light *LIGHTER_TEST_STEPS[5] = {
            FIRST_YELLOW_LIGHT,
            SECOND_YELLOW_LIGHT,
            THIRD_YELLOW_LIGHT,
            GREEN_LIGHT, RED_LIGHT
    };

    Light *LIGHTER_START_STEPS[4] = {
            FIRST_YELLOW_LIGHT,
            SECOND_YELLOW_LIGHT,
            THIRD_YELLOW_LIGHT,
            GREEN_LIGHT
    };

    const uint8_t FUNCTION_START = 0x01;
    const uint8_t FUNCTION_TEST = 0x02;

    uint8_t functionExecute = 0x00;
    uint8_t lighterData = 0x00;
    uint8_t lighterTestIteration = 0;
    uint8_t lighterStartIteration = 0;
};


#endif //WS_LIGHTERMANAGER_H
