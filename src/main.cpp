#include "wifi_setup/wifi_setup.h"
#include "mqtt_client/mqtt_client.h"
#include "config.h"

const char* machineIDs[3] = { "00-01", "00-02", "00-03" };
void setup() {
    Serial.begin(115200);
    for (int i = 0; i < 3; i++) {
        pinMode(redPins[i], OUTPUT);
        pinMode(greenPins[i], OUTPUT);
        pinMode(bluePins[i], OUTPUT);

        ledcSetup(i, 5000, 8);
        ledcAttachPin(redPins[i], i);
        ledcAttachPin(greenPins[i], i);
        ledcAttachPin(bluePins[i], i);
    }

    pinMode(PIN_LED, OUTPUT);
    
    connectWiFi();
    connectMQTT();
    client.setCallback(callback);
}

unsigned long lastMsg = 0;

void loop() {

    client.loop();
    // if (millis() - lastMsg > 5000) {
    //     lastMsg = millis();
    // }
}