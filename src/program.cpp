//
// Created by Dobrik on 15.08.2018.
//

#include "ESP8266WiFi.h"
#include "Hash.h"
#include "SPI.h"
#include "Light.h"
#include "LighterManager.h"
#include "SensorManager.h"
#include "ESP8266WiFiMulti.h"
#include "../lib/ws/src/WebSocketsServer.h"

#define CLOCK_PIN 14
#define DATA_PIN 4
#define LATCH_PIN 12

#define BUTTON_PIN 5

#define FIRST_YELLOW_LIGHTER_BYTE (1 << 0)
#define SECOND_YELLOW_LIGHTER_BYTE (1 << 1)
#define THIRD_YELLOW_LIGHTER_BYTE (1 << 2)
#define GREEN_LIGHTER_BYTE (1 << 3)
#define RED_LIGHTER_BYTE (1 << 4)

#define FRONT_LEFT_SENSOR_PIN 16
#define REAR_LEFT_SENSOR_PIN 2
//#define FRONT_RIGHT_SENSOR_PIN 13
//#define REAR_RIGHT_SENSOR_PIN 15

#define FRONT_LEFT_LIGHTER_BYTE (1 << 0)
#define REAR_LEFT_LIGHTER_BYTE (1 << 1)
//#define FRONT_RIGHT_LIGHTER_BYTE (1 << 2)
//#define REAR_RIGHT_LIGHTER_BYTE (1 << 3)



#define USE_SERIAL Serial

/**
 * Data payload packet size is 12 bytes
 * First 4 bytes is action type
 * Last 8 bytes is data payload
 */

// second byte data of lighter,First byte data of control lighter
uint8_t lightersShiftData[2] = {0, 0};

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

ESP8266WiFiMulti WiFiMulti = ESP8266WiFiMulti();

WebSocketsServer webSocket = WebSocketsServer(81);

Light lighters[LIGHTS_COUNT] = {
        Light(FIRST_YELLOW_LIGHTER_BYTE),
        Light(SECOND_YELLOW_LIGHTER_BYTE),
        Light(THIRD_YELLOW_LIGHTER_BYTE),
        Light(GREEN_LIGHTER_BYTE),
        Light(RED_LIGHTER_BYTE)
};

LighterManager lighterManger = LighterManager(lighters);

Sensor leftFrontSensor = Sensor(FRONT_LEFT_SENSOR_PIN, FRONT_LEFT_LIGHTER_BYTE);
Sensor leftRearSensor = Sensor(REAR_LEFT_SENSOR_PIN, REAR_LEFT_LIGHTER_BYTE);

SensorManager leftSensorManager = SensorManager(&leftFrontSensor, &leftRearSensor);

uint8_t adminNum;

void sendWebSocketActionData(WS_ACTIONS action, uint8_t payload) {
    uint8_t packet[2] = {
            action, payload
    };
    USE_SERIAL.printf("Send action packet[0] '%d'\n", packet[0]);
    USE_SERIAL.printf("Send data packet[1] '%d'\n", packet[1]);
    webSocket.sendBIN(adminNum, packet, sizeof(packet));
}

void processAction(uint8_t *payload, uint8_t size) {
    if (payload[0] == LIGHTER_START) {
        USE_SERIAL.println("LIGHTER_START");
        lighterManger.lighterStart();
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
            hexdump(payload, length);
            break;
        case WStype_CONNECTED: {
            IPAddress ip = webSocket.remoteIP(num);
            adminNum = num;
            USE_SERIAL.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);

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
            hexdump(payload, length);

            // send message to client
            webSocket.sendBIN(num, payload, length);
            break;
        case WStype_ERROR:
            USE_SERIAL.println("Error\n");
            break;
    }

}

void lighterStateChange();

void lighterManagerEvent(LMEType event_type, uint8_t *payload, size_t length) {
    switch (event_type) {
        case LIGHTER_UPDATE:
            sendWebSocketActionData(LIGHTER_STATE_UPDATE, *payload);
            lightersShiftData[1] = *payload;
            lighterStateChange();
            break;
    }
}

void sensorManagerEvent(SMState state, uint8_t *payload) {
    lightersShiftData[0] = *payload;
    lighterStateChange();
}

void lighterStateChange() {
    digitalWrite(LATCH_PIN, LOW);
    for (int i = 0; i < sizeof(lightersShiftData); i++) {
        shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, lightersShiftData[i]);
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

    WiFi.setSleepMode(WIFI_NONE_SLEEP);
    WiFiMulti.addAP("Dobrik_third", "12345678");
    WiFiMulti.addAP("Racing_Lighter", "12345678");

    while (WiFiMulti.run() != WL_CONNECTED) {
        delay(100);
    }

    pinMode(BUTTON_PIN, INPUT);
    attachInterrupt(BUTTON_PIN, testToggle, ONLOW);

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
}

void loop() {
    webSocket.loop();
    lighterManger.loop();
    leftSensorManager.loop();
}