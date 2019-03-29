//
// Created by Dobrik on 8/28/2018.
//
#include "Arduino.h"
#include "SPI.h"

#ifndef WS_LIGHTER_H
#define WS_LIGHTER_H


class Light {
public:
    Light(uint16_t dataByteLeft, uint16_t dataByteRight);

    uint16_t dataByteLeft, dataByteRight;
};


#endif //WS_LIGHTER_H
