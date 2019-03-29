//
// Created by Dobrik on 9/23/2018.
//

#ifndef WS_LIGHTERMANAGER_H
#define WS_LIGHTERMANAGER_H


#include <cstdint>
#include "Light.h"
#include "../lib/virtualDelay/avdweb_VirtualDelay.h"
#include "SensorManager.h"

typedef enum {
    LIGHTER_STAGE_STANDBY,
    LIGHTER_STAGE_READY_1,
    LIGHTER_STAGE_READY_2,
    LIGHTER_STAGE_READY_3,
    LIGHTER_STAGE_RACE,
    LIGHTER_STAGE_STOP,
} LighterStage;

typedef enum {
    LIGHTER_STATE_TEST,
    LIGHTER_STATE_READY,
    LIGHTER_STATE_COUNTDOWN,
    LIGHTER_STATE_RACE,
} LighterState;

class LighterManager {
public:
    typedef std::function<void(LighterStage state)> LighterManagerEvent;

    void onEvent(LighterManagerEvent _event);

    void lighterTestStart(int delay = 500);

    void lighterStart(int delay = 500);

    //TODO: create on state changed listeners
    void loop();

    void lighterStartFunction();

    void lighterTestFunction();

    void reset();

    LighterStage stage;

    LighterState state = LIGHTER_STATE_READY;
private:
    LighterManagerEvent event;

    VirtualDelay Delay;

    long functionDelay = 0;
    uint8_t callbackIteration = 0;
    void (LighterManager::* callback)(void);

    LighterStage LIGHTER_TEST_STEPS[6] = {
            LIGHTER_STAGE_READY_1,
            LIGHTER_STAGE_READY_2,
            LIGHTER_STAGE_READY_3,
            LIGHTER_STAGE_RACE,
            LIGHTER_STAGE_STOP,
            LIGHTER_STAGE_STANDBY
    };

    LighterStage LIGHTER_START_STEPS[5] = {
            LIGHTER_STAGE_READY_1,
            LIGHTER_STAGE_READY_2,
            LIGHTER_STAGE_READY_3,
            LIGHTER_STAGE_RACE,
            LIGHTER_STAGE_STANDBY
    };


    void runEvent(LighterStage stage);
};


#endif //WS_LIGHTERMANAGER_H
