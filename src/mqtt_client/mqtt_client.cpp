#include "mqtt_client.h"
#include <WiFiClient.h>
#include "config.h"

WiFiClient espClient;
PubSubClient client(espClient);

void connectMQTT()
{
    if (!client.connected())
    {
        digitalWrite(PIN_LED, HIGH);
        client.setServer(MQTT_SERVER, 1883);
        while (!client.connected())
        {
            Serial.print("Connexion MQTT...");
            if (client.connect(ID_CLIENT_MQTT))
            {
                Serial.println("Connecté au serveur MQTT!");
                for (int i = 0; i < 3; i++) {
                    String topic = String(TOPIC_ROOT) + "/" + String(machineIDs[i]) + "/" + String(TOPIC_COMMAND);
                    client.subscribe(topic.c_str());
                }
                
            }
            else
            {
                Serial.print("Échec, code erreur : ");
                int state = client.state();
                Serial.println(state);

                if (state == -2) {
                    Serial.println("Erreur de connexion : Connexion échouée au réseau");
                } else if (state == -1) {
                    Serial.println("Erreur de connexion : Serveur inaccessible");
                } else {
                    Serial.println("Autre erreur de connexion");
                }

                delay(5000);
            }

        }
        digitalWrite(PIN_LED, LOW);
    }
}

// void publishMessage(const char *topic, const char *payload, const char *idname)
// {
//     String fullTopic = String(TOPIC_ROOT) + "/" + String(idname) + "/" + String(topic);
//     client.publish(fullTopic.c_str(), payload);
//     Serial.print("Send :");
//     Serial.print(fullTopic);
//     Serial.print(" :");
//     Serial.println(payload);
// }

void publishMessage(const char *topic, const String &payload, const char *idname) {
    String fullTopic = String(TOPIC_ROOT) + "/" + String(idname) + "/" + String(topic);

    client.publish(fullTopic.c_str(), payload.c_str());
    Serial.print("Send : ");
    Serial.print(fullTopic);
    Serial.print(" : ");
    Serial.println(payload);
}

void setLED(int machineIndex, int red, int green, int blue) {
    analogWrite(redPins[machineIndex], red);
    analogWrite(greenPins[machineIndex], green);
    analogWrite(bluePins[machineIndex], blue);
}

// void callback(char* topic, byte* payload, unsigned int length) {
//     Serial.print("Message reçu [");
//     Serial.print(topic);
//     Serial.print("] : ");
//     String message = "";
//     for (unsigned int i = 0; i < length; i++) {
//         message += (char)payload[i];
//     }
//     Serial.println(message);

//     // Vérification du message reçu
//     for (int i = 0; i < 3; i++) {
//         String expectedTopic = String(TOPIC_ROOT) + "/" + String(machineIDs[i]) + "/" + String(TOPIC_COMMAND);
//         if (String(topic) == expectedTopic) {
//             Serial.println("Commande reçue pour la machine " + String(machineIDs[i]));
//             if (message == "start") {
//                 publishMessage(TOPIC_STATUS, "Préparation en cours", machineIDs[i]);
//                 setLED(i, 0, 0, 255); // LED bleue
//                 Serial.println("LED bleue allumée");
//             } else if (message == "stop") {
//                 publishMessage(TOPIC_STATUS, "Arrêt", machineIDs[i]);
//                 setLED(i, 255, 0, 0); // LED rouge
//                 Serial.println("LED rouge allumée");
//             }
//         }
//     }
// }

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
        String startTopic = String(TOPIC_ROOT) + "/" + String(machineIDs[i]) + String(TOPIC_COMMAND_START);
        String cancelTopic = String(TOPIC_ROOT) + "/" + String(machineIDs[i]) + String(TOPIC_RESERVATION_CANCEL);
        String reserveTopic = String(TOPIC_ROOT) + "/" + String(machineIDs[i]) + String(TOPIC_RESERVATION);

        if (String(topic) == startTopic) {
            Serial.println("Préparation d'un café pour la machine " + String(machineIDs[i]) + " : " + message);
            publishMessage(TOPIC_COMMAND_START, "Préparation en cours : " + message, machineIDs[i]);
            setLED(i, 0, 0, 255); // LED bleue pour signaler la préparation
        } 
        else if (String(topic) == cancelTopic) {
            Serial.println("Réservation annulée sur la machine " + String(machineIDs[i]));
            publishMessage(TOPIC_RESERVATION_CANCEL, "Réservation annulée", machineIDs[i]);
            setLED(i, 255, 0, 0); // LED rouge pour annulation
        }
        else if (String(topic) == reserveTopic) {
            Serial.println("Réservation reçue : " + message);
            publishMessage(TOPIC_RESERVATION, "Réservation confirmée : " + message, machineIDs[i]);
            setLED(i, 255, 105, 180); // LED rose pour indiquer la réservation
        }
    }
}




