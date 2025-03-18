#include "mqtt_client.h"
#include <WiFiClient.h>
#include "config.h"

WiFiClient espClient;
PubSubClient client(espClient);

// =============================
// Connexion MQTT
// =============================
void connectMQTT() {
    if (!client.connected()) {
        digitalWrite(PIN_LED, HIGH);
        client.setServer(MQTT_SERVER, 1883);

        while (!client.connected()) {
            Serial.print("Connexion MQTT... ");
            if (client.connect(ID_CLIENT_MQTT)) {
                Serial.println("Connecté !");

                String topic = String(TOPIC_ROOT) + "/#";
                client.subscribe(topic.c_str());
            } else {
                Serial.print("Echec, code erreur : ");
                Serial.println(client.state());

                if (client.state() == -2) {
                    Serial.println("Erreur : Connexion échouée au réseau");
                } else if (client.state() == -1) {
                    Serial.println("Erreur : Serveur MQTT inaccessible");
                } else {
                    Serial.println("Erreur inconnue");
                }

                delay(5000);
            }
        }

        digitalWrite(PIN_LED, LOW);
    }
    client.loop();  // Maintenir la connexion active
}

// =============================
// Publication MQTT
// =============================
void publishMessage(const char *topic, const String &payload, const char *idname) {
    String fullTopic = String(TOPIC_ROOT) + "/" + String(idname) + "/" + String(topic);

    Serial.print("Envoyé : ");
    Serial.print(fullTopic);
    Serial.print(" : ");
    Serial.println(payload);
}

void publishMessageNotif(const String &payload, const char *idname) {
    String topicnotif = String(TOPIC_ROOT) + "/" + String(idname) + "/" + String(TOPIC_NOTIFICATION);
    client.publish(topicnotif.c_str(), payload.c_str());

    Serial.print("Notification envoyée : ");
    Serial.println(payload);
}

// =============================
// Gestion des LEDs
// =============================
void setLED(int machineIndex, int red, int green, int blue) {
    analogWrite(redPins[machineIndex], red);
    analogWrite(greenPins[machineIndex], green);
    analogWrite(bluePins[machineIndex], blue);
}

// =============================
// Callback MQTT (Réception des messages)
// =============================
void callback(char* topic, byte* payload, unsigned int length) {
    Serial.print("Message reçu [");
    Serial.print(topic);
    Serial.print("] : ");
    
    String message = "";
    for (unsigned int i = 0; i < length; i++) {
        message += (char)payload[i];
    }
    Serial.println(message);

    for (int i = 0; i < 3; i++) {
        // Définition des topics pour chaque machine
        String startTopic = String(TOPIC_ROOT) + "/" + String(machineIDs[i]) + "/" + String(TOPIC_COMMAND_START);
        String cancelTopic = String(TOPIC_ROOT) + "/" + String(machineIDs[i]) + "/" + String(TOPIC_RESERVATION_CANCEL);
        String reserveTopic = String(TOPIC_ROOT) + "/" + String(machineIDs[i]) + "/" + String(TOPIC_RESERVATION);
        String expectedTopic = String(TOPIC_ROOT) + "/" + String(machineIDs[i]) + "/" + String(TOPIC_COMMAND);
        String maintenanceTopic = String(TOPIC_ROOT) + "/" + String(machineIDs[i]) + "/" + String(TOPIC_MAINTENANCE);

        if (String(topic) == startTopic) {
            Serial.println("Préparation d'un café pour la machine " + String(machineIDs[i]) + " : " + message);
            publishMessage(TOPIC_COMMAND_START, "Préparation en cours : " + message, machineIDs[i]);
            setLED(i, 0, 0, 255); // LED bleue pour signaler la préparation
            delay(5000);
            
            publishMessageNotif("Cafe pret", machineIDs[i]);
            setLED(i, 0, 0, 0);
        } 
        else if (String(topic) == cancelTopic) {
            Serial.println("Réservation annulée sur la machine " + String(machineIDs[i]));
            publishMessage(TOPIC_RESERVATION_CANCEL, "Réservation annulée", machineIDs[i]);
            setLED(i, 255, 0, 0); // LED rouge pour annulation
            delay(2000);
            setLED(i, 0, 0, 0);
        }
        else if (String(topic) == reserveTopic) {
            Serial.println("Réservation reçue : " + message);
            publishMessage(TOPIC_RESERVATION, "Réservation confirmée : " + message, machineIDs[i]);
            setLED(i, 255, 105, 180); // LED rose pour indiquer la réservation
        }
        else if (String(topic) == expectedTopic) {
            Serial.println("Commande reçue pour la machine " + String(machineIDs[i]));
            if (message == "start") {
                publishMessage(TOPIC_STATUS, "Préparation en cours", machineIDs[i]);
                setLED(i, 0, 255, 0); // LED verte
                Serial.println("LED verte allumée");
            } else if (message == "stop") {
                publishMessage(TOPIC_STATUS, "Arrêt", machineIDs[i]);
                setLED(i, 0, 0, 0); // LED éteinte
                Serial.println("LED éteinte");
            }
        }
        if (String(topic) == maintenanceTopic) {
            Serial.println("Maintenance " + String(machineIDs[i]) + " : " + message);
            publishMessage(TOPIC_MAINTENANCE, "Préparation en cours : " + message, machineIDs[i]);
            setLED(i, 255, 0, 0); // LED rouge
            publishMessageNotif(message, machineIDs[i]);
        } 
    }
}
