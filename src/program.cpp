//
// Created by Dobrik on 15.08.2018.
//

#include "ESP8266WiFi.h"
#include "Hash.h"
#include "Light.h"
#include "LighterManager.h"
#include "ESP8266WiFiMulti.h"
#include "../lib/ws/src/WebSocketsServer.h"
#include "../lib/virtualDelay/avdweb_VirtualDelay.h"

#define YELLOW1_PIN 5
#define YELLOW2_PIN 4
#define YELLOW3_PIN 0
#define GREEN_PIN 2
#define RED_PIN 14

#define FIRST_YELLOW_LIGHTER_BYTE 0x01
#define SECOND_YELLOW_LIGHTER_BYTE 0x02
#define THIRD_YELLOW_LIGHTER_BYTE 0x04
#define GREEN_LIGHTER_BYTE 0x08
#define RED_LIGHTER_BYTE 0x10

/**
 * Data payload packet size is 12 bytes
 * Last 4 bytes is action type
 * First 8 bytes is data payload
 */

enum ACTIONS_EMIT {
    WHEEL_SENSOR_UPDATE = 0x01,
    LIGHTER_STATE_UPDATE = 0x02,
    CONTROL_LIGHTER_STATE_UPDATE = 0x04,
};

enum ACTION_LISTEN {
    LIGHTER_START = 0x01,
    LIGHTER_TEST = 0x02,
};

uint8_t CONTROL_LIGHTER_DATA[4] = {
        0x01, //LEFT FRONT
        0x02, //LEFT REAR
        0x04, //RIGHT FRONT
        0x08, //RIGHT REAR
};

ESP8266WiFiMulti WiFiMulti = ESP8266WiFiMulti();

WebSocketsServer webSocket = WebSocketsServer(81);

#define USE_SERIAL Serial
Light lighters[5] = {
        Light(YELLOW1_PIN, FIRST_YELLOW_LIGHTER_BYTE),
        Light(YELLOW2_PIN, SECOND_YELLOW_LIGHTER_BYTE),
        Light(YELLOW3_PIN, THIRD_YELLOW_LIGHTER_BYTE),
        Light(GREEN_PIN, GREEN_LIGHTER_BYTE),
        Light(RED_PIN, RED_LIGHTER_BYTE)
};

LighterManager lighterManger = LighterManager(lighters);

uint8_t adminNum;
void sendActionData(uint8_t action, uint8_t payload) {
    uint8_t packet[2] = {
            action, payload
    };

    webSocket.sendBIN(adminNum, packet, sizeof(packet));
}

void processAction(uint8_t *payload, uint8_t size) {
    if (payload[0] == LIGHTER_START) {
        USE_SERIAL.println("LIGHTER_START");
        lighterManger.lighterStart();
    }else if(payload[0] == LIGHTER_TEST){
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


uint32_t lastClick = 0;

void testToggle() {
    uint16_t buttonDebouce = 200;
    if ((millis() - lastClick) > buttonDebouce) {
        USE_SERIAL.println("Toggle emit");
        lighterManger.lighterTestStart(500);
    }
    lastClick = millis();
}

void setup() {
    USE_SERIAL.begin(115200);

    pinMode(13, INPUT);
    attachInterrupt(13, testToggle, ONHIGH);

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

    webSocket.begin();
    webSocket.onEvent(webSocketEvent);
}

void loop() {
    webSocket.loop();
    lighterManger.loop();
}