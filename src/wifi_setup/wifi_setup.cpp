#include "wifi_setup.h"
#include <WiFi.h>
#include "config.h"

void connectWiFi() {
    if (!WiFi.isConnected()) {
        Serial.print("Connexion au WiFi...");
        WiFi.begin(SSID, PASSWORD);
        Serial.print(SSID PASSWORD);
        while (!WiFi.isConnected()) {
            delay(500);
            Serial.print(".");
        }
        Serial.println("\nWiFi connecté!");
    }
}

