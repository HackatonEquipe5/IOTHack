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
            Serial.print("Connexion MQTT...");
            if (client.connect(ID_CLIENT_MQTT)) {
                Serial.println("Connecté au serveur MQTT !");
                String topic = String(TOPIC_ROOT) + "/#";
                client.subscribe(topic.c_str());
            } else {
                Serial.print("Échec, code erreur : ");
                int state = client.state();
                Serial.println(state);
                
                switch (state) {
                    case -2: Serial.println("Erreur : Connexion échouée au réseau"); break;
                    case -1: Serial.println("Erreur : Serveur inaccessible"); break;
                    default: Serial.println("Erreur inconnue"); break;
                }
                delay(5000);
            }
        }
        digitalWrite(PIN_LED, LOW);
    }
}

// =============================
// Publication MQTT
// =============================
void publishMessage(const char *topic, const String &payload, const char *idname) {
    String fullTopic = String(TOPIC_ROOT) + "/" + String(idname) + "/" + String(topic);
    client.publish(fullTopic.c_str(), payload.c_str());
    
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

    for (int i = 0; i < nbrMachine; i++) {
        //Définition des topics pour chaque machine
        String startTopic = String(TOPIC_ROOT) + "/" + String(machineIDs[i]) + "/" + String(TOPIC_COMMAND_START);
        String cancelTopic = String(TOPIC_ROOT) + "/" + String(machineIDs[i]) + "/" + String(TOPIC_RESERVATION_CANCEL);
        String reserveTopic = String(TOPIC_ROOT) + "/" + String(machineIDs[i]) + "/" + String(TOPIC_RESERVATION);
        String expectedTopic = String(TOPIC_ROOT) + "/" + String(machineIDs[i]) + "/" + String(TOPIC_COMMAND);
        String maintenanceTopic = String(TOPIC_ROOT) + "/" + String(machineIDs[i]) + "/" + String(TOPIC_MAINTENANCE);

        //**Début de la préparation**
        if (String(topic) == startTopic) {
            Serial.println("Préparation café pour " + String(machineIDs[i]) + " : " + message);
            publishMessage(TOPIC_COMMAND_START, "Préparation en cours : " + message, machineIDs[i]);
            setLED(i, 0, 0, 255); // LED bleue
            
            delay(5000); // Simulation du temps de préparation
            
            publishMessageNotif("Café prêt", machineIDs[i]);
            setLED(i, 0, 0, 0); // Éteindre LED après préparation
        } 
        //**Annulation de réservation**
        else if (String(topic) == cancelTopic) {
            Serial.println("Réservation annulée sur " + String(machineIDs[i]));
            publishMessage(TOPIC_RESERVATION_CANCEL, "Réservation annulée", machineIDs[i]);
            setLED(i, 255, 0, 0); // LED rouge
            
            delay(2000);
            setLED(i, 0, 0, 0);
        }
        //**Nouvelle réservation**
        else if (String(topic) == reserveTopic) {
            Serial.println("Réservation reçue : " + message);
            publishMessage(TOPIC_RESERVATION, "Réservation confirmée : " + message, machineIDs[i]);
            setLED(i, 255, 105, 180); // LED rose
        }
        // **Commande de la machine**
        else if (String(topic) == expectedTopic) {
            Serial.println("Commande reçue pour " + String(machineIDs[i]));
            if (message == "start") {
                publishMessage(TOPIC_STATUS, "Préparation en cours", machineIDs[i]);
                setLED(i, 0, 255, 0); // LED verte
                Serial.println("LED verte allumée");
            } else if (message == "stop") {
                publishMessage(TOPIC_STATUS, "Arrêt", machineIDs[i]);
                setLED(i, 0, 0, 0); // Éteindre LED
                Serial.println("LED éteinte");
            }
        }
        //**Maintenance**
        else if (String(topic) == maintenanceTopic) {
            Serial.println("🔧 Maintenance en cours sur " + String(machineIDs[i]) + " : " + message);
            publishMessage(TOPIC_MAINTENANCE, "Préparation en cours : " + message, machineIDs[i]);
            setLED(i, 255, 0, 0); // LED rouge
            publishMessageNotif(message, machineIDs[i]);
        } 
    }
}
