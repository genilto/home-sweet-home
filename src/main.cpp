#include <Arduino.h>
#include <ThreeWire.h>
#include <RtcDS1302.h>

#include "Configurations.h"
#include "domain/Topic.h"
#include "domain/Log.h"
#include "domain/DisplayInfo.h"

#include "task/I2CDevicesTask.h"
#include "task/DBTask.h"
#include "task/WifiTask.h"
#include "task/MqttTask.h"
#include "task/RXTask.h"
#include "task/SensorTask.h"
#include "task/WebServerTask.h"
//#include "task/BuzzerTask.h"

#include "controller/DelayController.h"
#include "controller/DBController.h"

/*******************************************************
* Declaração de todos os Semaforos que serão utilizados
*******************************************************/
SemaphoreHandle_t hshSemaphoreSerial = NULL;

/*******************************************************
* Declaração de todas as Filas que serão criadas
*******************************************************/
QueueHandle_t hshPortChangedQueue = NULL; // Informações recebidas por MQTT a serem enviadas para o controlador de portas
QueueHandle_t hshMQTTSendQueue = NULL;    // Informações a serem enviadas para o MQTT
QueueHandle_t hshInsertLogQueue = NULL;   // Informações a serem enviadas para o Banco de Dados
QueueHandle_t hshDisplayQueue = NULL;     // Informações a serem enviadas para o Display
//QueueHandle_t hshBeepQueue         = NULL; // Informações a serem enviadas para acionar o Beep

// IO, SCLK, CE
ThreeWire myWire(TM_DAT_PIN, TM_CLK_PIN, TM_RST_PIN);
RtcDS1302<ThreeWire> rtc(myWire);

#define countof(a) (sizeof(a) / sizeof(a[0]))
void printDateTime(const RtcDateTime& dt)
{
    char datestring[20];

    snprintf_P(datestring, 
            countof(datestring),
            PSTR("%02u/%02u/%04u %02u:%02u:%02u"),
            dt.Month(),
            dt.Day(),
            dt.Year(),
            dt.Hour(),
            dt.Minute(),
            dt.Second() );
    Serial.print(datestring);
}

// Inicializa o RTC
void RTCBegin() 
{
    Serial.print("Compilado em: ");
    Serial.print(__DATE__);
    Serial.print(" as ");
    Serial.println(__TIME__);

    // Inicializa o RTC
    rtc.Begin();

    RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);
    printDateTime(compiled);
    Serial.println();

    if (!rtc.IsDateTimeValid()) 
    {
        // Common Causes:
        //    1) first time you ran and the device wasn't running yet
        //    2) the battery on the device is low or even missing
        Serial.println("RTC lost confidence in the DateTime!");
        rtc.SetDateTime(compiled);
    }

    if (rtc.GetIsWriteProtected())
    {
        Serial.println("RTC was write protected, enabling writing now");
        rtc.SetIsWriteProtected(false);
    }

    if (!rtc.GetIsRunning())
    {
        Serial.println("RTC was not actively running, starting now");
        rtc.SetIsRunning(true);
    }

    RtcDateTime now = rtc.GetDateTime();
    if (now < compiled) 
    {
        Serial.println("RTC is older than compile time!  (Updating DateTime)");
        rtc.SetDateTime(compiled);
    }
    else if (now > compiled) 
    {
        Serial.println("RTC is newer than compile time. (this is expected)");
    }
    else if (now == compiled) 
    {
        Serial.println("RTC is the same as compile time! (not expected but all is fine)");
    }
}

/*******************************************************
// SETUP
*******************************************************/
void setup()
{
    // Inicia a Serial
    Serial.begin(115200);

    // Aguarda a serial estar disponível
    while (!Serial);
    Serial.println("Serial Iniciado.");

    // Definição de pinos
    pinMode(POWER_SENSOR_PIN, INPUT);

    // Inicializa o RTC
    RTCBegin();

    // Cria os Semáforos
    hshSemaphoreSerial = xSemaphoreCreateMutex();

    // Cria as filas
    hshPortChangedQueue = xQueueCreate(20, sizeof(struct Topic));
    hshMQTTSendQueue = xQueueCreate(20, sizeof(struct Topic));
    hshInsertLogQueue = xQueueCreate(20, sizeof(struct Log));
    hshDisplayQueue = xQueueCreate(20, sizeof(struct DisplayInfo));
    //hshBeepQueue        = xQueueCreate ( 2,  sizeof( uint8_t ) );

    // Insere um registro de log inicial, assim que a tarefa subir
    DBController::insertLogQueue(LogType::GENERAL, LogSource::BOOT, "Boot", "@", LogFlag::YES, "Boot do Sistema");

    xTaskCreatePinnedToCore(
        i2cDevicesTask,   // Task function.
        "i2cDevicesTask", // String with name of task.
        5000,             // Stack size in words.
        NULL,             // Parameter passed as input of the task
        1,                // Priority of the task.
        NULL,             // Task handle.
        0);               // Core.

    // Inicia a task que irá ler os sinais 433Mgz dos controles
    xTaskCreatePinnedToCore(
        rxTask,     // Task function.
        "rxTask",   // String with name of task.
        3000,       // Stack size in words.
        NULL,       // Parameter passed as input of the task
        1,          // Priority of the task.
        NULL,       // Task handle.
        0);         // Core.

    // Inicia a task que irá gerenciar o Banco de Dados
    xTaskCreatePinnedToCore(
        dbTask,   // Task function.
        "dbTask", // String with name of task.
        10000,     // Stack size in words.
        NULL,     // Parameter passed as input of the task
        1,        // Priority of the task.
        NULL,     // Task handle.
        1);       // Core.

    // Inicia a task que irá gerenciar a conexão com o Wifi
    xTaskCreatePinnedToCore(
        wifiTask,   // Task function.
        "wifiTask", // String with name of task.
        3000,       // Stack size in words.
        NULL,       // Parameter passed as input of the task
        1,          // Priority of the task.
        NULL,       // Task handle.
        1);         // Core.

    // Inicia a task que irá gerenciar a conexão com o MQTT
    xTaskCreatePinnedToCore(
        mqttTask,   // Task function.
        "mqttTask", // String with name of task.
        3000,       // Stack size in words.
        NULL,       // Parameter passed as input of the task
        1,          // Priority of the task.
        NULL,       // Task handle.
        1);         // Core.

    // Inicia a task que irá gerenciar o buzzer
    xTaskCreatePinnedToCore(
        sensorTask,   // Task function.
        "sensorTask", // String with name of task.
        2000,         // Stack size in words.
        NULL,         // Parameter passed as input of the task
        1,            // Priority of the task.
        NULL,         // Task handle.
        1);           // Core.

    // Inicia a task que irá gerenciar o painel web de configurações
    xTaskCreatePinnedToCore(
        webServerTask,   // Task function.
        "webServerTask", // String with name of task.
        5000,            // Stack size in words.
        NULL,            // Parameter passed as input of the task
        1,               // Priority of the task.
        NULL,            // Task handle.
        1);              // Core.
    /*
    // Inicia a task que irá gerenciar o buzzer
    xTaskCreatePinnedToCore (
                        buzzerTask,               // Task function.
                        "buzzerTask",             // String with name of task.
                        1000,                     // Stack size in words.
                        NULL,                     // Parameter passed as input of the task
                        1,                        // Priority of the task.
                        NULL,                     // Task handle.
                        1);                       // Core.
        */
}

/*******************************************************
// LOOP
*******************************************************/
void loop()
{
    /*
    RtcDateTime now = rtc.GetDateTime();

    if (hshSemaphoreSerial != NULL && xSemaphoreTake(hshSemaphoreSerial, (TickType_t)TICKS_TO_WAIT_SEMAPHORE) == pdTRUE)
    {
        printDateTime(now);
        Serial.println();

        if (!now.IsValid())
        {
            // Common Causes:
            //    1) the battery on the device is low or even missing and the power line was disconnected
            Serial.println("RTC lost confidence in the DateTime!");
        }
        
        xSemaphoreGive(hshSemaphoreSerial);
    }*/
    delay(60000);
}
