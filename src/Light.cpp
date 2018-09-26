//
// Created by Dobrik on 8/28/2018.
//

#include "Light.h"

Light::Light(int pin_light, uint8_t dataByte) {
    this->pin_light = pin_light;
    this->dataByte = dataByte;
    this->init();
}

void Light::setOn() {
    digitalWrite((uint8_t) this->pin_light, HIGH);
};

void Light::setOff() {
    digitalWrite((uint8_t) this->pin_light, LOW);
};

void Light::invert() {
    digitalWrite((uint8_t) this->pin_light, !digitalRead((uint8_t) this->pin_light));
}

void Light::init() {
    pinMode((uint8_t) this->pin_light, OUTPUT);
    digitalWrite((uint8_t) this->pin_light, (uint8_t) default_state);
}