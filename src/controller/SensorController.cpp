#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#include "Configurations.h"
#include "controller/DBController.h"
#include "controller/DisplayController.h"
#include "controller/SensorController.h"

SensorController::SensorController()
{
    // Inicializa o onewire
    this->_oneWire.begin(ONE_WIRE_PIN);

    // Inicializa a biblioteca para leitura dos sensores de temperatura
    this->_dallasSensor.setOneWire(&this->_oneWire);

    // Inicializa o DHT
    this->_dht.setup(DHT_SENSOR_PIN, DHTesp::DHT11);

    // Delay de 1 minuto para gravação do log com informações de memória
    this->_memoryDelay.setDelayTime(60000);

    // Delay de 5 minutos para gravação do log com informações de temperatura externa e do boiler
    this->_dallasDelay.setDelayTime(5 * 60000);

    // Delay de 5 minutos para gravação do log com informações de temperatura e umidade interna
    this->_dhtDelay.setDelayTime(5 * 60000);
}
void SensorController::init()
{
    // Start up the library
    this->_dallasSensor.begin();
}
void SensorController::loop()
{
    this->readPower();
    this->readMemory();
    this->readDallasSensor();
    this->readDHT();
}
void SensorController::readPower()
{
    int hasPower = digitalRead(POWER_SENSOR_PIN);

    // Atualiza o display quando mudar o estado da energia
    if (this->_hasPower != hasPower)
    {
        String c = "0";
        String obs = (this->_hasPower == -1 ? "Sem energia" : "Acabou a energia");

        if (hasPower == 1)
        {
            c = "1";
            obs = (this->_hasPower == -1 ? "Com energia" : "Voltou a energia");
        }

        // Atualiza o display e envia o evento para o Log
        DisplayController::updateInfo(DisplayKey::POWER_CHANGED, c);
        DBController::insertLogQueue(LogType::POWER, LogSource::ESP_PIN, "@", c, LogFlag::YES, obs);

        this->_hasPower = hasPower;
    }
}
void SensorController::readMemory()
{
    double total = (512.0 * 1024.0);
    double free = ESP.getFreeHeap();
    double used = (total - free);
    String memoryPercent = String((int) round((used / total) * 100.00));

    // Quando o percentual de memória variar atualiza o display
    if (!memoryPercent.equals(this->_memoryPercent))
    {
        DisplayController::updateInfo(DisplayKey::MEMORY_CHANGED, memoryPercent);
        this->_memoryPercent = memoryPercent;
    }

    // Gera logs de memoria a cada periodo de tempo
    if (this->_memoryDelay.expired())
    {
        DBController::insertLogQueue(LogType::FREE_MEMORY, LogSource::INTERNAL, "@", String(free), LogFlag::NO, "Total de Memória Livre");
    }
}
void SensorController::readDallasSensor()
{
    // Sensor de Temperatura
    // Envia o comando para leitura dos sensores
    this->_dallasSensor.requestTemperatures();

    // Conforme você adiciona os sensores, é necessário mapear qual deles está
    // em qual porta
    int externalIndex = 1;
    int boilerIndex = 0;

    // Lê a Temperatura Externa
    float extTemperature = this->_dallasSensor.getTempCByIndex(externalIndex);
    bool extTempError = (extTemperature < -100);
    String extTemperatureString = String(extTemperature);

    // Lê a Temperatura do Boiler
    float boilerTemperature = this->_dallasSensor.getTempCByIndex(boilerIndex);
    bool boilerTempError = (boilerTemperature < -100);
    String boilerTemperatureString = String(boilerTemperature);

    // Verifica se ocorreu alteração da temperatura externa
    if (!extTempError && !this->_extTemperature.equals(extTemperatureString))
    {
        DisplayController::updateInfo(DisplayKey::EXT_TEMP_CHANGED, extTemperatureString);
        this->_extTemperature = extTemperatureString;
    }

    // Verifica se ocorreu alteração da temperatura do boiler
    if (!boilerTempError && !this->_boilerTemperature.equals(boilerTemperatureString))
    {
        DisplayController::updateInfo(DisplayKey::BOILER_TEMP_CHANGED, boilerTemperatureString);
        this->_boilerTemperature = boilerTemperatureString;
    }

    // Grava um log com a temperatura atual a cada período predefinido
    if (this->_dallasDelay.expired())
    {
        if (!extTempError)
            DBController::insertLogQueue(LogType::EXT_TEMPERATURE, LogSource::DS1820_SENSOR, "@", extTemperatureString, LogFlag::NO, "Temperatura Externa");

        if (!boilerTempError)
            DBController::insertLogQueue(LogType::BOILER_TEMPERATURE, LogSource::DS1820_SENSOR, "@", boilerTemperatureString, LogFlag::NO, "Temperatura do Boiler");
    }
}
void SensorController::readDHT()
{
    // Reading temperature for humidity takes about 250 milliseconds!
    // Sensor readings may also be up to 2 seconds 'old' (it's a very slow sensor)
    TempAndHumidity newValues = this->_dht.getTempAndHumidity();

    // Check if any reads failed and exit early (to try again).
    if (this->_dht.getStatus() != 0)
    {
        this->_qtyErrorDHTReadings++;
        
        // Apenas grava o log com erro em caso de 5 erros
        if (this->_qtyErrorDHTReadings >= 5) {
            DBController::insertLogQueue(LogType::ERROR, LogSource::DHT_SENSOR, "DHT11", "Erro", LogFlag::NO, "Erro ao ler DHT11: " + String(this->_dht.getStatusString()));
            this->_qtyErrorDHTReadings = 0;
        }
    }
    else
    {
        this->_qtyErrorDHTReadings = 0;
        String temperature = String(newValues.temperature);
        String humidity = String(((int)roundf(newValues.humidity)));

        if (!this->_intTemperature.equals(temperature))
        {
            DisplayController::updateInfo(DisplayKey::INT_TEMP_CHANGED, temperature);
            this->_intTemperature = temperature;
        }
        if (!this->_intHumidity.equals(humidity))
        {
            DisplayController::updateInfo(DisplayKey::INT_HUMIDITY_CHANGED, humidity);
            this->_intHumidity = humidity;
        }

        // Envia os dados para o log a cada periodo de tempo
        if (this->_dhtDelay.expired())
        {
            DBController::insertLogQueue(LogType::IN_TEMPERATURE, LogSource::DHT_SENSOR, "@", temperature, LogFlag::NO, "Temperatura Interna");
            DBController::insertLogQueue(LogType::IN_HUMIDITY, LogSource::DHT_SENSOR, "@", humidity, LogFlag::NO, "Umidade Interna");
        }
    }

    /*
    extern SemaphoreHandle_t hshSemaphoreSerial;
    if (hshSemaphoreSerial != NULL && xSemaphoreTake(hshSemaphoreSerial, (TickType_t)TICKS_TO_WAIT_SEMAPHORE) == pdTRUE)
    {  
        // Comfort profile
        ComfortState cf;

        float heatIndex = this->_dht.computeHeatIndex(newValues.temperature, newValues.humidity);
        float dewPoint = this->_dht.computeDewPoint(newValues.temperature, newValues.humidity);
        float cr = this->_dht.getComfortRatio(cf, newValues.temperature, newValues.humidity);

        String comfortStatus;
        switch (cf)
        {
        case Comfort_OK:
            comfortStatus = "Comfort_OK";
            break;
        case Comfort_TooHot:
            comfortStatus = "Comfort_TooHot";
            break;
        case Comfort_TooCold:
            comfortStatus = "Comfort_TooCold";
            break;
        case Comfort_TooDry:
            comfortStatus = "Comfort_TooDry";
            break;
        case Comfort_TooHumid:
            comfortStatus = "Comfort_TooHumid";
            break;
        case Comfort_HotAndHumid:
            comfortStatus = "Comfort_HotAndHumid";
            break;
        case Comfort_HotAndDry:
            comfortStatus = "Comfort_HotAndDry";
            break;
        case Comfort_ColdAndHumid:
            comfortStatus = "Comfort_ColdAndHumid";
            break;
        case Comfort_ColdAndDry:
            comfortStatus = "Comfort_ColdAndDry";
            break;
        default:
            comfortStatus = "Unknown:";
            break;
        };

        Serial.println(" T:" + String(newValues.temperature) + " H:" + String(newValues.humidity) + " I:" + String(heatIndex) + " D:" + String(dewPoint) + " " + comfortStatus);
        xSemaphoreGive(hshSemaphoreSerial);
    }
    */
}