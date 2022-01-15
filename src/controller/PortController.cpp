#include <Arduino.h>
#include "Configurations.h"
#include <Adafruit_MCP23017.h>
#include "domain/Port.h"
#include "domain/Topic.h"
#include "domain/Log.h"
#include "controller/MqttController.h"
#include "controller/PortController.h"
#include "controller/DBController.h"

// Configurações de Portas
PortController::PortController(uint8_t id, uint8_t relayPort, uint8_t switchPort,
                               String user, String gbridge, String name, uint8_t status)
{
    this->port.id = id;
    this->port.relayPort = relayPort;
    this->port.switchPort = switchPort;
    this->port.status = status;

    // Aguarda este tempo em millisegundos antes de considerar mudança de estado do interruptor
    this->_delayDebounce.setDelayTime(500);

    // Define as configurações da porta
    this->defineNames(user, gbridge, name);
}
void PortController::defineNames(String user, String gbridge, String name)
{
    this->port.user = user;
    this->port.gbridge = gbridge;
    this->port.name = name;
}
void PortController::init(Adafruit_MCP23017 &mcp)
{
    this->_mcp = &mcp;

    // Porta do relê
    this->_mcp->pinMode(this->port.relayPort, OUTPUT);
    this->changeStatus(this->port.status);

    // Porta do Interruptor
    this->_mcp->pinMode(this->port.switchPort, INPUT);
    this->_mcp->pullUp(this->port.switchPort, HIGH); // turn on a 100K pullup internally

    // Faz o controle da porta do interruptor, por isso já faz a leitura do estado atual
    // Para que não ligue as luzes ao iniciar o esp
    this->_lastStatus = this->_mcp->digitalRead(this->port.switchPort);
}
Port PortController::getPort()
{
    return this->port;
}
bool PortController::canPublishOrSubscribe()
{
    return !this->port.gbridge.equalsIgnoreCase("@");
}
bool PortController::isPortByTopic(String topic)
{
    return this->getFullTopic().equalsIgnoreCase(topic);
}
LogFlag PortController::changeStatus(uint8_t status)
{
    if ((status == 0 || status == 1) && this->port.status != status)
    {
        this->port.status = status;

        // Inverte a regra para envio ao MCP para ficar mais clara a programação
        this->_mcp->digitalWrite(this->port.relayPort, (this->port.status == 0 ? 1 : 0));

        // Necessário enviar para o MQTT que a porta em questão foi alterada
        // Para isso enviar para a fila que será lida na tarefa do MQTT
        if (this->canPublishOrSubscribe())
        {
            MqttController::publishTopicQueue(this->getFullTopic(), String(this->port.status));
        }
        return LogFlag::YES;
    }
    return LogFlag::NO;
}
// Chamado pelo MQTT
bool PortController::changeStatusMQTT(Topic topic)
{
    LogFlag changed = LogFlag::NO;

    if (this->isPortByTopic(String(topic.code)))
    {
        String topicValue = String(topic.value);

        // Valida a informação vinda do MQTT
        if (topicValue.equalsIgnoreCase("0") || topicValue.equalsIgnoreCase("1"))
        {
            String obs = "MQTT: " + this->toString();
            changed = this->changeStatus(topicValue.toInt());
            obs += " -> " + String(this->port.status);

            // Solicita inserção do Log
            DBController::insertLogQueue(LogType::PORT, LogSource::MQTT, String(this->port.id), topicValue, changed, obs);
        }
    }
    return (changed == LogFlag::YES);
}
// Chamado pelo RX
bool PortController::toggleStatusRX(Topic topic)
{
    if (!String(this->port.id).equalsIgnoreCase(topic.code)) {
        return false;
    }

    String obs = "RX: " + this->toString();
    this->toggleStatus();
    obs += " -> " + String(this->port.status);

    // Solicita inserção do Log
    DBController::insertLogQueue(LogType::PORT, LogSource::RX_SENSOR, String(this->port.id), topic.code, LogFlag::YES, obs);
    
    return true;
}
void PortController::toggleStatus()
{
    this->changeStatus(this->port.status == 0 ? 1 : 0);
}
// Controle de Interruptores. Retorna true caso tenha alterado o estado da porta
bool PortController::loop()
{
    bool hasChanged = false;

    bool currentStatus = this->_mcp->digitalRead(this->port.switchPort);
    if (currentStatus != this->_lastStatus)
    {
        if (this->_delayDebounce.expired())
        {
            this->_lastStatus = currentStatus;
            String obs = "Interruptor: " + this->toString();

            this->toggleStatus();
            hasChanged = true;

            String status = String(this->port.status);
            obs += " -> " + status;

            // Solicita inserção do Log
            DBController::insertLogQueue(LogType::PORT, LogSource::SWITCH, String(this->port.id), status, LogFlag::YES, obs);
        }
    }
    else
    {
        this->_delayDebounce.reset();
    }
    return hasChanged;
}
void PortController::subscribe()
{
    // Apenas deverá se inscrever quando possui configuração do gBridge
    if (this->canPublishOrSubscribe())
    {
        MqttController::subscribeTopicQueue(this->getFullTopic(), String(this->port.status));
    }
}
String PortController::getFullTopic()
{
    return "gBridge/" + this->port.user + "/" + this->port.gbridge + "/onoff";
}
String PortController::toString()
{
    return this->port.name + " (" + this->port.gbridge + ") - Status: " + String(this->port.status);
}