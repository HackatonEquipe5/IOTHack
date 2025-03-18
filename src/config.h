//Config Wifi a update (moi je suis en partage de co sur le télèphone)
#define SSID "BriseurDeVitre"
#define PASSWORD "123456789GF"
#define MQTT_SERVER "10.220.135.176"

//Dans notre cas 1 ESP32 = x machine a cafe
#define ID_CLIENT_MQTT "ESP32-CAFE"

//const des TOPIC
#define TOPIC_ROOT "machinecafe"

#define TOPIC_ID_MACHINE01 "00-01"
#define TOPIC_ID_MACHINE02 "00-02"
#define TOPIC_ID_MACHINE03 "00-03"

#define TOPIC_STATUS "status"
#define TOPIC_COMMAND "command"
#define TOPIC_STOCK "stock"
#define TOPIC_ALERTE "alerte" //Peut être utile plus tard
#define TOPIC_ERREUR "erreur"

#define ERREUR_BRANCHEMENT "Le capteur est mal branché"


// Configuration des LEDs
const int redPins[] = {25, 32, 14};  
const int greenPins[] = {26, 33, 12};  
const int bluePins[] = {27, 34, 13};
extern const char* machineIDs[3];

#define PIN_LED 2
#define ADRESS 0x77


#define DHTTYPE DHT11
#define DHTPIN 4