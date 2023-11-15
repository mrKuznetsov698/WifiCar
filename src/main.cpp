#include <Arduino.h>
#include "ESP8266WiFi.h"
#include "ESP8266WebServer.h"

// ********************* Settings *********************
#define L_DIR D6
#define R_DIR D5
#define L_SPD D1
#define R_SPD D2

#define SSID "ArseniiNet"
#define PASS "mio9led1"

// ********************* Variables *********************

ESP8266WebServer server(80);

int speed = 100;
double dir1 = 0;
double dir2 = 0;

// ************** Functions declaration **************

void motor_run(int spd1, int spd2);
void api_change_speed();
void api_stop();
void api_run();

// ****************** Loop & setup *******************

void setup() {
    SPIFFS.begin();
    Serial.begin(115200);
    
    WiFi.mode(WIFI_STA);
    WiFi.begin(SSID, PASS);

    while (WiFi.status() != WL_CONNECTED) {
        Serial.print('.');
        delay(500);
    }
    Serial.println("connected");
    Serial.println(WiFi.localIP());

    pinMode(L_DIR, OUTPUT);
    pinMode(R_DIR, OUTPUT);
    pinMode(L_SPD, OUTPUT);
    pinMode(R_SPD, OUTPUT);

    server.serveStatic("/", SPIFFS, "/index.html");
    server.serveStatic("/static/", SPIFFS, "/static/");
    server.on("/api/stop", HTTP_GET, api_stop);
    server.on("/api/speed", HTTP_GET, api_change_speed);
    server.on("/api/run", HTTP_GET, api_run);

    server.begin();
}

void loop() {
    server.handleClient();
}

// ************* Functions implementation *****************

void motor_update() {
    int s1 = speed * dir1;
    int s2 = speed * dir2;
    if (s1 > 0) {
        digitalWrite(L_DIR, HIGH);
    } else {
        digitalWrite(L_DIR, LOW);
    }

    if (s2 > 0) {
        digitalWrite(R_DIR, HIGH);
    } else {
        digitalWrite(R_DIR, LOW);
    }

    analogWrite(L_SPD, abs(s1));
    analogWrite(R_SPD, abs(s2));
}

void api_stop() {
    dir1 = dir2 = 0;
    motor_update();
    server.send(200, "text/plain", "cat stopped");
}

void api_change_speed() {
    int n = server.args();
    if (n != 1 || server.argName(0) != "spd") {
        server.send(400, "text/plain", "bad request");
        return;
    }
    int val = server.arg(0).toInt();
    speed = val;
    motor_update();
    server.send(200, "text/plain", "speed changed");
}

void api_run() {
    int n = server.args();
    if (n != 1 || server.argName(0) != "act") {
        server.send(400, "text/plain", "bad request");
        return;
    }
    auto val = server.arg(0);
    if (val == "forw") {
        dir1 = dir2 = 1;
    } else if (val == "back") {
        dir1 = dir2 = -1;
    } else if (val == "right") {
        dir1 = -1, dir2 = 1;
    } else if (val == "left") {
        dir1 = 1, dir2 = -1;
    } else if (val == "leftup") {
        dir1 = 0.75, dir2 = 1;
    } else if (val == "rightup") {
        dir1 = 1, dir2 = 0.75;
    } else if (val == "rightdown") {
        dir1 = -1, dir2 = -0.75;
    } else if (val == "leftdown") {
        dir1 = -0.75, dir2 = -1;
    } else {
        server.send(400, "text/plain", "bad request");
        return;
    }
    motor_update();
    server.send(200, "text/plain", "status changed");
}