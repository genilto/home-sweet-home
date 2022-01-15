#include <Arduino.h>
#include "Configurations.h"

#ifndef DisplayInfo_h
#define DisplayInfo_h

enum DisplayKey {
    CMD,
    WIFI_CHANGED,
    MQTT_CHANGED,
    POWER_CHANGED,
    SDCARD_CHANGED,
    HOUR_CHANGED,
    BAT_VOLTAGE_CHANGED,
    MEMORY_CHANGED,
    EXT_TEMP_CHANGED,
    BOILER_TEMP_CHANGED,
    INT_TEMP_CHANGED,
    INT_HUMIDITY_CHANGED,
    WATER_LEVEL_CHANGED,
};

// Classe com as informações de tópicos MQTT
struct DisplayInfo {
    DisplayKey key;
    char value[DISPLAY_FIELD_SIZE_VALUE] = "\0"; // Valor
    int icon;
};

#endif