#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include "DHTesp.h"
#include "controller/DelayController.h"

#ifndef SensorController_h
#define SensorController_h

class SensorController
{
private:
    OneWire _oneWire;
    DallasTemperature _dallasSensor;
    DHTesp _dht;

    // Controle dos delays para gravação de logs
    DelayController _memoryDelay;
    DelayController _dallasDelay;
    DelayController _dhtDelay;

    // Informações lidas
    int _hasPower             = -1;
    String _memoryPercent     = "--";
    String _extTemperature    = "--.--";
    String _boilerTemperature = "--.--";
    
    int _qtyErrorDHTReadings  = 0;
    String _intTemperature    = "--.--";
    String _intHumidity       = "--";

    void readPower();
    void readMemory();
    void readDallasSensor();
    void readDHT();
public:
    SensorController();
    void init();
    void loop();
};

#endif
