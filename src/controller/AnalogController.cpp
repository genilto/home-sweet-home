#include <Arduino.h>
#include <Adafruit_ADS1015.h>
#include "Configurations.h"
#include "controller/DBController.h"
#include "controller/DisplayController.h"
#include "controller/AnalogController.h"

AnalogController::AnalogController(Adafruit_ADS1115 &ads)
{
    this->_ads = &ads;
    this->_delay.setDelayTime(1000);

    // Gera um registro no log a cada período
    this->_delayLog.setDelayTime(2 * 60000);
}
void AnalogController::init()
{
    this->_ads->begin();
}
void AnalogController::loop()
{
    // Faz a leitura de sensores analógicos
    if (this->_delay.expired())
    {
        this->readBatteryVoltage();
        this->readWaterTankLevel();

        // Gera no log apenas após um período maior
        if (this->_delayLog.expired())
        {
            DBController::insertLogQueue(LogType::BATTERY_VOLTAGE, LogSource::ADS_SENSOR, "@", this->_batteryVoltage, LogFlag::NO, "Tensão da Bateria");
            DBController::insertLogQueue(LogType::WATER_TANK_LEVEL, LogSource::ADS_SENSOR, "@", this->_waterTankLevel, LogFlag::NO, "Nível da Caixa Dágua");
        }
    }
}
void AnalogController::readBatteryVoltage()
{
    int16_t adc = this->_ads->readADC_SingleEnded(0);
    float voltage = (adc * 0.1875) / 1000;
    if (voltage < 0.0)
    {
        voltage = 0.0;
    }
    String voltageString = String(voltage);

    // Verifica se o valor mudou, e atualiza o display
    if (!this->_batteryVoltage.equals(voltageString))
    {
        int icon = 0;
        if (voltage >= 4.0)
            icon = 3;
        else if (voltage >= 3.5)
            icon = 2;
        else if (voltage >= 3.0)
            icon = 1;

        DisplayController::updateInfo(DisplayKey::BAT_VOLTAGE_CHANGED, voltageString, icon);
        this->_batteryVoltage = voltageString;
    }
}
void AnalogController::readWaterTankLevel()
{
    int16_t full = 25000;
    int16_t adc  = this->_ads->readADC_SingleEnded(1);
    double level = ((double) adc / (double) full) * 100.0;
    
    String levelString = String(((int) round(level)));

    // Verifica se o valor mudou, e atualiza o display
    if (!this->_waterTankLevel.equals(levelString))
    {
        DisplayController::updateInfo(DisplayKey::WATER_LEVEL_CHANGED, levelString);
        this->_waterTankLevel = levelString;
    }
}