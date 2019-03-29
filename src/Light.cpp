//
// Created by Dobrik on 8/28/2018.
//

#include "Light.h"
#include "SPI.h"

Light::Light(uint16_t dataByteLeft, uint16_t dataByteRight) {
    this->dataByteLeft = dataByteLeft;
    this->dataByteRight = dataByteRight;
}