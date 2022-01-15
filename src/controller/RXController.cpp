#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include "Configurations.h"
#include "domain/Topic.h"
#include "controller/DisplayController.h"
#include "controller/DBController.h"
#include "controller/RXController.h"

RXController::RXController()
{
    // Conecta o switch na porta configurada
    this->_switch.enableReceive(12); // D5
   
    // Timer entre leitura do RX
    //this->_delay.setDelayTime(2000);
}
void RXController::init()
{
}
void RXController::loop()
{
    //if (this->_delay.expired()) {
        
    //}

    if (this->_switch.available()) {
        long received = this->_switch.getReceivedValue();
        if (received == 0) {
            
        } else {

            extern SemaphoreHandle_t hshSemaphoreSerial;

            if (hshSemaphoreSerial != NULL && xSemaphoreTake(hshSemaphoreSerial, (TickType_t)TICKS_TO_WAIT_SEMAPHORE) == pdTRUE)
            {
                Serial.print("INTERRUPTOR CLICADO: ");
                Serial.println(received);
                xSemaphoreGive(hshSemaphoreSerial);
            }

            // Carrega as configurações de portas atuais, mantendo as que estavam ligadas e desligadas
            //this->definePort(0, "d7", "Varanda");
            //this->definePort(1, "d4", "Sala");
            //this->definePort(2, "d2", "Lustre");
            //this->definePort(3, "d5", "Cozinha");
            //this->definePort(4, "d6", "Quarto");
            //this->definePort(5, "d1", "Quarto 2");

            if (received == 10010568) { // Sala
                this->changePort ("1");
            } else if (received == 10010564) { // Varanda
                this->changePort ("0");
            } else if (received == 10010562) { // Quarto 2
                this->changePort ("5");
            }

            //DBController::insertLogQueue(LogType::PORT, LogSource::RX_SENSOR, "RX", String(received), LogFlag::NO, "RX RECEIVED");

            // Aguarda um segundo depois de receber algum comando RX
            delay(300);
        }

        this->_switch.resetAvailable();
    }
}
void RXController::changePort(String port)
{
    extern QueueHandle_t hshPortChangedQueue;

    // Monta a struct para envio dos dados
    struct Topic topic;
    topic.type = TopicType::RX_RECEIVED;

    strncpy(topic.code, port.c_str(), MQTT_TOPIC_CODE_SIZE);

    // Adiciona o pacote recebido na queue do controlador de portas
    xQueueSend(hshPortChangedQueue, (void *) &topic, (TickType_t) 10);
}