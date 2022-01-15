#include <Arduino.h>
#include <Adafruit_ADS1015.h>
#include "Configurations.h"
#include "controller/DelayController.h"

#ifndef AnalogController_h
#define AnalogController_h

class AnalogController
{
private:
    Adafruit_ADS1115 *_ads;
    DelayController _delay;
    DelayController _delayLog;

    // Valores lidos
    String _batteryVoltage = "--.--";
    String _waterTankLevel = "--";

    void readBatteryVoltage();
    void readWaterTankLevel();

public:
    AnalogController(Adafruit_ADS1115 &ads);
    void init();
    void loop();
};

#endif
