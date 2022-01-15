#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include "Configurations.h"
#include "domain/Topic.h"
#include "controller/DisplayController.h"
#include "controller/DBController.h"
#include "controller/MqttController.h"

MqttController::MqttController()
{
    this->MQTT.setClient(this->wifiClient);
    this->MQTT.setServer(BROKER_MQTT, BROKER_PORT);
    this->MQTT.setCallback(MqttController::receiveTopic);

    // Verifica conexão com MQTT a cada 10 segundos
    this->_delay.setDelayTime(10000);
}
bool MqttController::setConnected()
{
    bool connected = (WiFi.isConnected() && MQTT.connected());

    // Envia notificação para o Display quando ocorre alguma mudança
    if ((this->_connected && !connected) || (!this->_connected && connected))
    {
        String c = (connected ? "1" : "0");
        String cs = (connected ? "Conectado" : "Desconectado");
        DisplayController::updateInfo(DisplayKey::MQTT_CHANGED, c);

        DBController::insertLogQueue(LogType::MQTT_CONNECTION, LogSource::MQTT, "@", c, LogFlag::YES, "Mqtt Status: " + cs);
    }

    this->_connected = connected;
    return connected;
}
void MqttController::connect()
{
    extern QueueHandle_t hshPortChangedQueue;

    if (this->setConnected()) return;

    // Apenas tenta conexão quando o Wifi está conectado
    if (WiFi.isConnected())
    {
        if (MQTT.connect(CLIENT_ID, USER_MQTT, PWD_MQTT))
        {
            this->setConnected();

            // Monta a struct para envio dos dados pedindo para a tarefa de controle de portas
            // Enviar os tópicos para inscrição
            struct Topic topic;
            topic.type = TopicType::TO_SUBSCRIBE;

            String topicCode = "@";
            strncpy(topic.code, topicCode.c_str(), MQTT_TOPIC_CODE_SIZE);

            String topicValue = "@";
            strncpy(topic.value, topicValue.c_str(), MQTT_TOPIC_VALUE_SIZE);

            // Envia para o controlador de portas um comando para que ele responda com as portas que deve increver no MQTT
            xQueueSend(hshPortChangedQueue, (void *)&topic, (TickType_t) 10);
        }
    }
}
void MqttController::init()
{
}
void MqttController::loop()
{
    extern QueueHandle_t hshMQTTSendQueue;

    // Verifica a conexão a cada período, não o tempo todo
    if (this->_delay.expired()) connect();

    MQTT.loop();

    Topic localTopic;

    // Verifica a Fila caso exista algo a ser publicado
    // Não deverá bloquear em caso de não ter nada na fila
    if (xQueueReceive(hshMQTTSendQueue, &(localTopic), 0) == pdTRUE)
    {
        if (localTopic.type == TopicType::TO_PUBLISH)
        {
            // Publica o valor do tópico
            this->publishTopic(localTopic);
        }
        else if (localTopic.type == TopicType::TO_SUBSCRIBE)
        {
            // Se inscreve no tópico
            this->subscribeTopic(localTopic);
        }
    }
}
// Se inscreve em algum tópico
void MqttController::subscribeTopic(Topic topic)
{
    if (this->setConnected())
    {
        boolean success = this->MQTT.subscribe(topic.code);

        // Sempre que se inscrever no tópico, atualiza o valor
        if (success)
        {
            this->publishTopic(topic);
        }
        else 
        {
            DBController::insertLogQueue(LogType::ERROR, LogSource::MQTT, "Subscribe", "@", LogFlag::NO, "Erro ao se inscrever no tópico " + String(topic.code));
        }
    }
}
// Publica um valor no Topico
void MqttController::publishTopic(Topic topic)
{
    if (this->setConnected())
    {
        // Adiciona o /set ao tópico para definir o valor sendo enviado
        String topicCode = String(topic.code) + "/set";

        // Envia o dado para o MQTT
        MQTT.publish(topicCode.c_str(), topic.value);
    }
}
// Método estático que adiciona um tópico na Queue para que seja
// então lido pela tarefa de MQTT e enviado
void MqttController::subscribeTopicQueue(String topicCode, String topicValue)
{
    extern QueueHandle_t hshMQTTSendQueue;

    // Monta a struct para envio dos dados
    struct Topic topic;
    topic.type = TopicType::TO_SUBSCRIBE;
    strncpy(topic.code, topicCode.c_str(), MQTT_TOPIC_CODE_SIZE);
    strncpy(topic.value, topicValue.c_str(), MQTT_TOPIC_VALUE_SIZE);

    // Adiciona o pacote recebido na queue
    xQueueSend(hshMQTTSendQueue, (void *) &topic, (TickType_t) 10);
}
// Método estático
// Recebe o pacote dos tópicos o qual está inscrito
void MqttController::receiveTopic(char *code, byte *payload, unsigned int length)
{
    extern QueueHandle_t hshPortChangedQueue;

    if (sizeof(code) <= 0) return;

    // Monta a struct para envio dos dados
    struct Topic topic;
    topic.type = TopicType::MQTT_RECEIVED;
    strncpy(topic.code, code, MQTT_TOPIC_CODE_SIZE);

    String value = "";
    // obtem a string do payload recebido
    for (int i = 0; i < length; i++)
    {
        char c = (char) payload[i];
        value += c;
    }

    strncpy(topic.value, value.c_str(), MQTT_TOPIC_VALUE_SIZE);

    // Adiciona o pacote recebido na queue do controlador de portas
    xQueueSend(hshPortChangedQueue, (void *) &topic, (TickType_t) 10);
}
// Método estático que adiciona um tópico na Queue para que seja
// então lido pela tarefa de MQTT e enviado
void MqttController::publishTopicQueue(String topicCode, String topicValue)
{
    extern QueueHandle_t hshMQTTSendQueue;

    // Monta a struct para envio dos dados
    struct Topic topic;
    topic.type = TopicType::TO_PUBLISH;
    strncpy(topic.code, topicCode.c_str(), MQTT_TOPIC_CODE_SIZE);
    strncpy(topic.value, topicValue.c_str(), MQTT_TOPIC_VALUE_SIZE);

    // Adiciona o pacote recebido na queue
    xQueueSend(hshMQTTSendQueue, (void *) &topic, (TickType_t) 10);
}