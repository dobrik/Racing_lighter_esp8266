//
// Created by Dobrik on 8/28/2018.
//
#include "Arduino.h"

#ifndef WS_LIGHTER_H
#define WS_LIGHTER_H


class Light {
public:
    Light(int pin_light, uint8_t dataByte);

    void setOn();
    void setOff();
    void invert();
    uint8_t dataByte;
private:
    int pin_light;
    int default_state = LOW;

    void init();

};


#endif //WS_LIGHTER_H
