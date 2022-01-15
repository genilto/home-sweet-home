#include <Arduino.h>
#include "Configurations.h"

#ifndef Topic_h
#define Topic_h

enum TopicType {
    MQTT_RECEIVED,
    TO_PUBLISH,
    TO_SUBSCRIBE,
    RX_RECEIVED
};

// Classe com as informações de tópicos MQTT
struct Topic {
    char code[MQTT_TOPIC_CODE_SIZE] = "\0";   // Código inteiro do tópico
    char value[MQTT_TOPIC_VALUE_SIZE] = "\0"; // Valor do Topico
    TopicType type; // Tipo do Tópico
};

#endif