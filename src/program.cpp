//
// Created by Dobrik on 15.08.2018.
//

#include <Arduino.h>

#ifdef ESP8266

#include <ESP8266WiFi.h>
#endif


#ifdef ESP32

#include <WiFi.h>
#include <WiFiAP.h>

#endif

#include "Pins.h"
#include "SPI.h"
#include "Light.h"
#include "LighterManager.h"
#include "SensorManager.h"
#include "../lib/ws/src/WebSocketsServer.h"

#define CLOCK_PIN 2
#define DATA_PIN 4
#define LATCH_PIN 15

#define TEST_BUTTON_PIN 21

#define FRONT_LEFT_SENSOR_PIN 27
#define REAR_LEFT_SENSOR_PIN 14
#define FRONT_RIGHT_SENSOR_PIN 12
#define REAR_RIGHT_SENSOR_PIN 13

#define FRONT_LEFT_YELLOW_LIGHTER_BYTE (1 << 0)
#define REAR_LEFT_YELLOW_LIGHTER_BYTE (1 << 1)
#define FRONT_RIGHT_YELLOW_LIGHTER_BYTE (1 << 2)
#define REAR_RIGHT_YELLOW_LIGHTER_BYTE (1 << 3)


#define FIRST_LEFT_YELLOW_LIGHTER_BYTE (1 << 4)
#define FIRST_RIGHT_YELLOW_LIGHTER_BYTE (1 << 5)
#define SECOND_LEFT_YELLOW_LIGHTER_BYTE (1 << 6)
#define SECOND_RIGHT_YELLOW_LIGHTER_BYTE (1 << 7)
#define THIRD_LEFT_YELLOW_LIGHTER_BYTE (1 << 8)
#define THIRD_RIGHT_YELLOW_LIGHTER_BYTE (1 << 9)
#define GREEN_LEFT_LIGHTER_BYTE (1 << 10)
#define GREEN_RIGHT_LIGHTER_BYTE (1 << 11)
#define RED_LEFT_LIGHTER_BYTE (1 << 12)
#define RED_RIGHT_LIGHTER_BYTE (1 << 13)

#define USE_SERIAL Serial

/**
 * Data payload packet size is 12 bytes
 * First 4 bytes is action type
 * Last 8 bytes is data payload
 */

// lighters bytes 16
uint16_t lightersShiftData;

typedef enum WS_ACTIONS_EMIT {
    CONTROL_LIGHTER_STATE_UPDATE = (1 << 0),
    LIGHTER_STATE_UPDATE = (1 << 1),
    RACE_RESULT = (1 << 2),
    FALSE_START = (1 << 3)
} WS_ACTIONS;

enum WS_ACTION_LISTEN {
    LIGHTER_START = (1 << 0),
    LIGHTER_TEST = (1 << 1)
};


WebSocketsServer webSocket = WebSocketsServer(81);


Light leftSensorLight = Light(FRONT_LEFT_YELLOW_LIGHTER_BYTE, REAR_LEFT_YELLOW_LIGHTER_BYTE); //left sensor
Light rightSensorLight = Light(FRONT_RIGHT_YELLOW_LIGHTER_BYTE, REAR_RIGHT_YELLOW_LIGHTER_BYTE); //right sensor

Light firstYellowLight = Light(FIRST_LEFT_YELLOW_LIGHTER_BYTE, FIRST_RIGHT_YELLOW_LIGHTER_BYTE); //ready 1
Light secondYellowLight = Light(SECOND_LEFT_YELLOW_LIGHTER_BYTE, SECOND_RIGHT_YELLOW_LIGHTER_BYTE); //ready 2
Light thirdYellowLight = Light(THIRD_LEFT_YELLOW_LIGHTER_BYTE, THIRD_RIGHT_YELLOW_LIGHTER_BYTE); //ready 3
Light greenLight = Light(GREEN_LEFT_LIGHTER_BYTE, GREEN_RIGHT_LIGHTER_BYTE); //race
Light redLight = Light(RED_LEFT_LIGHTER_BYTE, RED_RIGHT_LIGHTER_BYTE);//stop

LighterManager lighterManger = LighterManager();

Sensor leftFrontSensor = Sensor(FRONT_LEFT_SENSOR_PIN);
Sensor leftRearSensor = Sensor(REAR_LEFT_SENSOR_PIN);
Sensor rightFrontSensor = Sensor(FRONT_RIGHT_SENSOR_PIN);
Sensor rightRearSensor = Sensor(REAR_RIGHT_SENSOR_PIN);

SensorManager leftSensorManager = SensorManager(&leftFrontSensor, &leftRearSensor);
SensorManager rightSensorManager = SensorManager(&rightFrontSensor, &rightRearSensor);

int adminNum = -1;

void sendWebSocketActionData(WS_ACTIONS action) {
    if (adminNum == -1) {
        return;
    }

    uint8_t shiftData[2];
    memcpy(shiftData, &lightersShiftData, sizeof(shiftData));

    uint8_t packet[4] = {0, 0, 0, 0};
    packet[0] = action;

    packet[1] = shiftData[0];
    packet[2] = shiftData[1];

    USE_SERIAL.printf("Send action packet[0] '%d'\n", packet[0]);
    USE_SERIAL.printf("Send data packet[1] '%d'\n", packet[1]);
    USE_SERIAL.printf("Send data packet[2] '%d'\n", packet[2]);
    USE_SERIAL.printf("Send data packet[3] '%d'\n", packet[3]);
    webSocket.sendBIN(adminNum, packet, sizeof(packet));
}

void processAction(uint8_t *payload, uint8_t size) {
    if (payload[0] == LIGHTER_START) {
        if (leftSensorManager.state == SENSOR_READY && rightSensorManager.state == SENSOR_READY) {
            USE_SERIAL.println("LIGHTER_START");
            lighterManger.lighterStart();
        }
    } else if (payload[0] == LIGHTER_TEST) {
        USE_SERIAL.println("LIGHTER_TEST");
        lighterManger.lighterTestStart();
    }
    for (uint8_t i = 0; size > i; i++) {
        USE_SERIAL.printf("Byte received: '%d'!\n", payload[i]);
    }
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length) {
    switch (type) {
        case WStype_DISCONNECTED:
            USE_SERIAL.printf("[%u] Disconnected!\n", num);
            //hexdump(payload, length);
            break;
        case WStype_CONNECTED: {
            IPAddress ip = webSocket.remoteIP(num);
            adminNum = num;
            USE_SERIAL.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);

            sendWebSocketActionData(LIGHTER_STATE_UPDATE);
            // send message to client
            webSocket.sendTXT(num, "Connected");
        }
            break;
        case WStype_TEXT:
            USE_SERIAL.printf("[%u] get Text: %s\n", num, payload);
            webSocket.sendTXT(num, payload);
            // send message to client
            // webSocket.sendTXT(num, "message here");

            // send data to all connected clients
            // webSocket.broadcastTXT("message here");
            break;
        case WStype_BIN:
            USE_SERIAL.printf("[%u] get binary length: %u\n", num, length);
            processAction(payload, length);
            //hexdump(payload, length);

            break;
        case WStype_ERROR:
            USE_SERIAL.println("Error\n");
            break;
    }

}

void sendShift(uint16_t data);

void sendStateChange();

void lighterManagerEvent(LighterStage stage) {
    lightersShiftData &= ~(firstYellowLight.dataByteLeft | firstYellowLight.dataByteRight);
    lightersShiftData &= ~(secondYellowLight.dataByteLeft | secondYellowLight.dataByteRight);
    lightersShiftData &= ~(thirdYellowLight.dataByteLeft | thirdYellowLight.dataByteRight);
    lightersShiftData &= ~(greenLight.dataByteLeft | greenLight.dataByteRight);
    lightersShiftData &= ~(redLight.dataByteLeft | redLight.dataByteRight);

    switch (stage) {
        case LIGHTER_STAGE_STOP:
            lightersShiftData |= (redLight.dataByteLeft | redLight.dataByteRight);
        case LIGHTER_STAGE_RACE:
            lightersShiftData |= (greenLight.dataByteLeft | greenLight.dataByteRight);
        case LIGHTER_STAGE_READY_3:
            lightersShiftData |= (thirdYellowLight.dataByteLeft | thirdYellowLight.dataByteRight);
        case LIGHTER_STAGE_READY_2:
            lightersShiftData |= (secondYellowLight.dataByteLeft | secondYellowLight.dataByteRight);
        case LIGHTER_STAGE_READY_1:
            lightersShiftData |= (firstYellowLight.dataByteLeft | firstYellowLight.dataByteRight);
            break;
    }
    sendWebSocketActionData(LIGHTER_STATE_UPDATE);
    sendStateChange();
}

void falseStartCallback(SensorManager *manager, uint8_t iteration) {
    if (manager == &leftSensorManager) {
        lightersShiftData &= ~redLight.dataByteLeft;
        if (iteration % 2 == 0) {
            lightersShiftData |= redLight.dataByteLeft;
        }
    } else {
        lightersShiftData &= ~redLight.dataByteRight;
        if (iteration % 2 == 0) {
            lightersShiftData |= redLight.dataByteRight;
        }
    }
    if (iteration == 7) {
        manager->reset();
    }

    sendWebSocketActionData(FALSE_START);
    sendWebSocketActionData(LIGHTER_STATE_UPDATE);
    sendStateChange();
}

void sensorManagerEvent(SMState state, SensorManager *manager) {
    if (manager == &leftSensorManager) {
        lightersShiftData &= ~(leftSensorLight.dataByteLeft | leftSensorLight.dataByteRight);
        if (state == SENSOR_BACKWARD) {
            lightersShiftData |= leftSensorLight.dataByteLeft;
        } else if (state == SENSOR_FORWARD) {
            lightersShiftData |= leftSensorLight.dataByteRight;
        } else if (state == SENSOR_READY) {
            lightersShiftData |= (leftSensorLight.dataByteRight | leftSensorLight.dataByteLeft);
        }
        USE_SERIAL.print("Left sensor manager");
    } else {
        lightersShiftData &= ~(rightSensorLight.dataByteLeft | rightSensorLight.dataByteRight);
        if (state == SENSOR_BACKWARD) {
            lightersShiftData |= rightSensorLight.dataByteLeft;
        } else if (state == SENSOR_FORWARD) {
            lightersShiftData |= rightSensorLight.dataByteRight;
        } else if (state == SENSOR_READY) {
            lightersShiftData |= (rightSensorLight.dataByteRight | rightSensorLight.dataByteLeft);
        }
        USE_SERIAL.print("Right sensor manager");
    }

    if (state == SENSOR_FORWARD && lighterManger.state == LIGHTER_STATE_COUNTDOWN) {
        lighterManger.reset();
        manager->falseStart();
    }
    USE_SERIAL.print(" .Sensor state chanded: ");
    USE_SERIAL.println(state);
    sendWebSocketActionData(LIGHTER_STATE_UPDATE);
    sendStateChange();
}

void sendStateChange() {
    USE_SERIAL.print("Lighter shift data: ");
    USE_SERIAL.println(lightersShiftData);
    sendShift(lightersShiftData);
}

void sendShift(uint16_t data) {
    uint8_t shiftData[2] = {0, 0};
    memcpy(shiftData, &data, sizeof(shiftData));
    digitalWrite(LATCH_PIN, LOW);
    for (int i = 1; i >= 0; i--) {
        shiftOut(DATA_PIN, CLOCK_PIN, LSBFIRST, shiftData[i]);
    }
    digitalWrite(LATCH_PIN, HIGH);
}


uint32_t lastClick = 0;


void testToggle() {
    uint16_t buttonDebouce = 200;
    if ((millis() - lastClick) > buttonDebouce) {
        USE_SERIAL.println("Toggle emit");
        lighterManger.lighterTestStart();
    }
    lastClick = millis();
}

void setup() {

#ifdef ESP8266
    bool wifiResult = WiFi.softAP("RacingLighter", "12345678");
#endif

#ifdef ESP32
    WiFi.softAP("RacingLighter", "12345678");
#endif


    USE_SERIAL.begin(115200);
    USE_SERIAL.setDebugOutput(true);
    USE_SERIAL.println();
    USE_SERIAL.println();
    USE_SERIAL.println();

    for (uint8_t t = 4; t > 0; t--) {
        USE_SERIAL.printf("[SETUP] BOOT WAIT %d...\n", t);
        USE_SERIAL.flush();
        delay(1000);
    }

    pinMode(TEST_BUTTON_PIN, INPUT_PULLUP);
    attachInterrupt(TEST_BUTTON_PIN, testToggle, ONLOW);

    /**
     * 74HC595 begin
     */
    pinMode(CLOCK_PIN, OUTPUT);
    pinMode(DATA_PIN, OUTPUT);
    pinMode(LATCH_PIN, OUTPUT);
    /**
     * 74HC595 end
     */

    webSocket.begin();
    webSocket.onEvent(webSocketEvent);
    lighterManger.onEvent(lighterManagerEvent);
    leftSensorManager.onUpdate(sensorManagerEvent);
    leftSensorManager.onFalseStart(falseStartCallback);
    rightSensorManager.onUpdate(sensorManagerEvent);
    rightSensorManager.onFalseStart(falseStartCallback);
}

void loop() {
    webSocket.loop();
    lighterManger.loop();
    leftSensorManager.loop();
    rightSensorManager.loop();
}