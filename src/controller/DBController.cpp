#include <Arduino.h>
#include <ThreeWire.h>  
#include <RtcDS1302.h>
#include "FS.h"
#include "SD.h"
#include <SPI.h>

#include "Configurations.h"
#include "domain/Log.h"
#include "controller/DisplayController.h"
#include "controller/DBController.h"

DBController::DBController()
{
    if (SD.begin())
    {
        // Guarda o tipo de cartão que foi inserido
        this->_cardType = SD.cardType();

        if (this->hasSDCard())
        {
            DisplayController::updateInfo (DisplayKey::SDCARD_CHANGED, "1");
            double cardSize = SD.cardSize() / (1024.0 * 1024.0 * 1024.0);

            extern SemaphoreHandle_t hshSemaphoreSerial;
            if (hshSemaphoreSerial != NULL && xSemaphoreTake(hshSemaphoreSerial, (TickType_t)TICKS_TO_WAIT_SEMAPHORE) == pdTRUE)
            {
                Serial.print("SD: SD Card Type: ");
                Serial.println(this->getCardTypeName());

                Serial.print("SD: SD Card Size: ");
                Serial.print(String(cardSize));
                Serial.println(" GB");
                xSemaphoreGive(hshSemaphoreSerial);
            }
        }
    }
}
String DBController::getDate(const RtcDateTime& dt)
{
    String date = "0000-00-00 00:00:00";

    if (dt.IsValid()) {
        // Format the time and date and insert into the temporary buffer.
        char buf[LOG_FIELD_SIZE_DATE];
        snprintf(buf, sizeof(buf), "%04d-%02d-%02d %02d:%02d:%02d",
                dt.Year(), dt.Month(), dt.Day(),
                dt.Hour(), dt.Minute(), dt.Second());

        date = String(buf);
    }

    // Print the formatted string to serial so we can see the time.
    return date;
}
String DBController::getHour(const RtcDateTime& dt)
{
    String hour = "--:--";

    if (dt.IsValid()) {
        // Format the time and date and insert into the temporary buffer.
        char buf[10];
        snprintf(buf, sizeof(buf), "%02d:%02d",
                dt.Hour(), dt.Minute());

        hour = String(buf);
    }

    // Print the formatted string to serial so we can see the time.
    return hour;
}
bool DBController::hasSDCard()
{
    return !(this->_cardType == CARD_NONE);
}
String DBController::getCardTypeName()
{
    switch (this->_cardType)
    {
    case CARD_MMC:
        return "MMC";
        break;
    case CARD_SD:
        return "SDSC";
        break;
    case CARD_SDHC:
        return "SDHC";
        break;
    default:
        return "UNKNOWN";
        break;
    }
}
void DBController::appendLog(String data)
{
    extern SemaphoreHandle_t hshSemaphoreSerial;
/*
    if (hshSemaphoreSerial != NULL && xSemaphoreTake(hshSemaphoreSerial, (TickType_t)TICKS_TO_WAIT_SEMAPHORE) == pdTRUE)
    {
        Serial.print("SD DATA: ");
        Serial.println(data);
        xSemaphoreGive(hshSemaphoreSerial);
    }
*/
    if (!this->hasSDCard())
        return;

    // Tenta abrir o arquivo para inclusão, senão conseguir cria o arquivo
    File file = SD.open(LOG_FILE, FILE_APPEND);
    if (!file)
        file = SD.open(LOG_FILE, FILE_WRITE);

    if (file)
    {
        // Adiciona uma Quebra de linha
        size_t size = file.print(data + "\r\n");
        file.close();

        if (hshSemaphoreSerial != NULL && xSemaphoreTake(hshSemaphoreSerial, (TickType_t)TICKS_TO_WAIT_SEMAPHORE) == pdTRUE)
        {
            Serial.print(" -> ");
            if (size) {
                Serial.print("SD (");
                Serial.print(size);
                Serial.print(") ");
            } else {
                Serial.print("SD FALHA: ");
            }
            Serial.println(data);
            xSemaphoreGive(hshSemaphoreSerial);
        }
    }
}
void DBController::loop()
{
    extern RtcDS1302<ThreeWire> rtc;
    RtcDateTime now = rtc.GetDateTime();
    
    String hour = getHour(now);

    // Atualiza o display quando mudar o minuto
    if (!this->_lastHourString.equalsIgnoreCase(hour))
    {
        DisplayController::updateInfo(DisplayKey::HOUR_CHANGED, hour);
        this->_lastHourString = hour;
    }

    this->insertLogs(getDate(now));
}
// Insere o registro do log no banco de dados
void DBController::insertLogs(String date)
{
    extern QueueHandle_t hshInsertLogQueue;

    // Percorre todos os elementos na fila para fazer um insert
    // de uma vez só. Isso melhora drasticamente a performance
    Log log;

    // Verifica a Fila caso exista algo a ser inserido
    // Não deverá bloquear em caso de não ter nada na fila
    while (xQueueReceive(hshInsertLogQueue, &(log), 0) == pdTRUE)
    {
        String data = date; //log.date;
        data += ",";
        data += String(log.type);
        data += ",";
        data += String(log.source);
        data += ",";
        data += log.key;
        data += ",";
        data += log.value;
        data += ",";
        data += log.applied;
        data += ",";
        data += log.obs;

        this->appendLog(data);
    }
}
// Insere na queue as informações do log ocorrido. Deve ser chamado de outra tarefa.
void DBController::insertLogQueue(LogType logType, LogSource logSource, String key, String value, LogFlag applied, String obs)
{
    extern QueueHandle_t hshInsertLogQueue;

    // Monta a struct para envio dos dados
    struct Log log;

    log.type    = logType;
    log.source  = logSource;
    log.applied = applied;

    //strncpy(log.date, getDateTime().c_str(), LOG_FIELD_SIZE_DATE);
    strncpy(log.key, key.c_str(), LOG_FIELD_SIZE_KEY);
    strncpy(log.value, value.c_str(), LOG_FIELD_SIZE_VALUE);
    strncpy(log.obs, obs.c_str(), LOG_FIELD_SIZE_OBS);

    // Adiciona o pacote recebido na queue
    xQueueSend(hshInsertLogQueue, (void *) &log, (TickType_t) 10);
}