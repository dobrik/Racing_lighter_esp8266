//
// Created by Dobrik on 8/28/2018.
//

#include "Arduino.h"

#ifndef WS_LIGHTER_H
#define WS_LIGHTER_H


class Lighter {
    const uint8_t STATE_WAITING = 0x00;
    const uint8_t STATE_RACE = 0x01;
    const uint8_t STATE_COUNTDOWN = 0x02;
};


#endif //WS_LIGHTER_H
