#include <Arduino.h>
#include "Configurations.h"

#ifndef Log_h
#define Log_h

enum LogType {
    ERROR,
    GENERAL,
    PORT,
    BOILER_TEMPERATURE,
    WATER_TANK_LEVEL,
    IN_TEMPERATURE,
    IN_HUMIDITY,
    EXT_TEMPERATURE,
    WIFI_CONNECTION,
    MQTT_CONNECTION,
    POWER,
    FREE_MEMORY,
    BATTERY_VOLTAGE,
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