//
// Created by Dobrik on 9/23/2018.
//

#ifndef WS_LIGHTERMANAGER_H
#define WS_LIGHTERMANAGER_H


#include <cstdint>
#include <list>
#include "Light.h"
#include "../lib/virtualDelay/avdweb_VirtualDelay.h"

typedef enum {
    LIGHTER_UPDATE
} LMEType;

enum lighters {
    FIRST_YELLOW_LIGHT,
    SECOND_YELLOW_LIGHT,
    THIRD_YELLOW_LIGHT,
    GREEN_LIGHT,
    RED_LIGHT,
    LIGHTS_COUNT
};

class LighterManager {
public:
    LighterManager(Light _lights[LIGHTS_COUNT]);

    typedef std::function<void (LMEType event_type, uint8_t * payload, size_t length)> LighterManagerEvent;

    void onEvent(LighterManagerEvent _event);

    void lighterTestStart(int delay = 500);

    void lighterStart(int delay = 500);

    //TODO: create on state changed listeners
    void loop();

    void lighterStartFunction();

    void lighterTestFunction();
private:
    LighterManagerEvent event;

    VirtualDelay Delay;

    long functionDelay = 0;

    void (LighterManager::* callback)(void);

    uint8_t LIGHTER_TEST_STEPS[5] = {
            FIRST_YELLOW_LIGHT,
            SECOND_YELLOW_LIGHT,
            THIRD_YELLOW_LIGHT,
            GREEN_LIGHT,
            RED_LIGHT
    };

    uint8_t LIGHTER_START_STEPS[4] = {
            FIRST_YELLOW_LIGHT,
            SECOND_YELLOW_LIGHT,
            THIRD_YELLOW_LIGHT,
            GREEN_LIGHT
    };

    Light *lights;

    uint8_t lighterData = 0x00;
    uint8_t lighterTestIteration = 0;
    uint8_t lighterStartIteration = 0;

    void runEvent(LMEType event_type, uint8_t * payload, size_t length);
};


#endif //WS_LIGHTERMANAGER_H
