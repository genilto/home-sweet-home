#include <Arduino.h>
#include "Configurations.h"

#ifndef Log_h
#define Log_h

enum LogType {
    ERROR,              // 0
    GENERAL,            // 1
    PORT,               // 2
    BOILER_TEMPERATURE, // 3
    WATER_TANK_LEVEL,   // 4
    IN_TEMPERATURE,     // 5
    IN_HUMIDITY,        // 6
    EXT_TEMPERATURE,    // 7
    WIFI_CONNECTION,    // 8
    MQTT_CONNECTION,    // 9
    POWER,              // 10
    FREE_MEMORY,        // 11
    BATTERY_VOLTAGE,    // 12
};

enum LogSource {
    INTERNAL,
    BOOT,
    MQTT,
    SWITCH,
    ESP_PIN,
    DHT_SENSOR,
    ADS_SENSOR,
    DS1820_SENSOR,
    RX_SENSOR,
};

enum LogFlag {
    NO, YES
};

// Classe com as informações de tópicos MQTT
struct Log {
    char date[LOG_FIELD_SIZE_DATE]   = "\0"; // Data
    LogType type;                            // Tipo do Log
    LogSource source;                        // Origem do Log
    char key[LOG_FIELD_SIZE_KEY]     = "\0"; // Chave
    char value[LOG_FIELD_SIZE_VALUE] = "\0"; // Valor
    LogFlag applied;
    char obs[LOG_FIELD_SIZE_OBS]     = "\0"; // Observações adicionais
};

#endif