#include <Arduino.h>
#include <Adafruit_MCP23017.h>
#include "Configurations.h"
#include "images.h"
#include "domain/Topic.h"
#include "controller/PortController.h"
#include "controller/PortListController.h"

PortListController::PortListController(SSD1306Wire &display, String user)
{
    this->_display = &display;
    this->_user = user;
}

void PortListController::init()
{
    // Inicializa o MCP e define o valor padrão como ligado para as portas de saída para os relês, desligando os relês
    // As portas de entrada dos interruptores são inicializadas desligadas
    this->_mcp.begin();
    this->_mcp.writeGPIOAB(0xF0F0);

    // Carrega as configurações de portas atuais, mantendo as que estavam ligadas e desligadas
    this->definePort(0, "d7", "Varanda");
    this->definePort(1, "d4", "Sala");
    this->definePort(2, "d2", "Lustre");
    this->definePort(3, "d5", "Cozinha");
    this->definePort(4, "d6", "Quarto");
    this->definePort(5, "d1", "Quarto 2");

    // Inicializa cada porta do MCP
    for (uint8_t i = 0; i < 8; i++)
    {
        this->defaultPorts[i].init(this->_mcp);
    }
}

bool PortListController::loop(bool updateDisplay)
{
    extern QueueHandle_t hshPortChangedQueue;

    Topic localTopic;
    bool hasChanged = false;

    // Verifica se ocorreu alguma alteração de porta via MQTT
    // Ou seja, se a tarefa de MQTT adicionou algo na fila
    // Não deverá bloquear em caso de não ter nada na fila
    BaseType_t received = xQueueReceive(hshPortChangedQueue, &(localTopic), 0);

    // Exibe as portas no display
    int posX = 0;
    int posY = -1;

    // Percorre cada porta do MCP
    for (uint8_t i = 0; i < 8; i++)
    {
        // Garante o loop da porta atual
        PortController *port = &this->defaultPorts[i];
        if (port->loop())
        {
            hasChanged = true;
        }

        // Verifica se o tópico recebido é referente a porta em questão
        if (received == pdTRUE)
        {
            // Indica que deverá se inscrever nos tópicos
            if (localTopic.type == TopicType::TO_SUBSCRIBE)
            {
                port->subscribe();
            }
            else if (localTopic.type == TopicType::MQTT_RECEIVED)
            {
                if (port->changeStatusMQTT(localTopic))
                {
                    hasChanged = true;
                }
            }
            else if (localTopic.type == TopicType::RX_RECEIVED)
            {
                if (port->toggleStatusRX(localTopic))
                {
                    hasChanged = true;
                }
            }
        }

        // Atualiza as informações no display apenas quando houver alguma alteração
        // Sempre irá atualizar no próximo loop
        if (updateDisplay)
        {
            if (port->canPublishOrSubscribe())
            {
                if (i == 3)
                {
                    posX = 70;
                    posY = 15;
                }
                else
                {
                    posY += 16;
                }
                this->printPort(posX, posY, port->getPort().status, port->getPort().name);
            }
        }
    }
    return hasChanged;
}

void PortListController::definePort(uint8_t index, String gbridge, String name)
{
    this->defaultPorts[index].defineNames(this->_user, gbridge, name);
}

PortController *PortListController::getPortByTopic(String topic)
{
    // Busca o indice da porta desejada
    for (uint8_t i = 0; i < 8; i++)
    {
        PortController *port = &this->defaultPorts[i];
        if (port->isPortByTopic(topic))
        {
            return port;
        }
    }
    // Não encontrada
    return NULL;
}

void PortListController::printPort(int posX, int posY, int status, String name)
{
    this->_display->drawXbm(posX, posY, Icon_width, Icon_height, (status == 1 ? bulb_on_icon16x16 : bulb_off_icon16x16));
    this->_display->drawString((posX + Icon_width), (posY + 3), name);
}