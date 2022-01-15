#include <Arduino.h>
#include <Adafruit_MCP23017.h>
#include "domain/Log.h"
#include "domain/Port.h"
#include "domain/Topic.h"
#include "controller/DelayController.h"

#ifndef PortController_h
#define PortController_h

// Configurações de Portas
class PortController {
    private:
        
        // Informações da porta
        Port port;
        
        // Controles da porta
        Adafruit_MCP23017* _mcp;
        DelayController _delayDebounce;
        bool _lastStatus = HIGH;

    public:
        PortController(uint8_t id, uint8_t relayPort, uint8_t switchPort, 
                    String user, String gbridge, String name, uint8_t status);
        
        void defineNames(String user, String gbridge, String name);
        void init(Adafruit_MCP23017& mcp);
        Port getPort();
        bool canPublishOrSubscribe();
        String getFullTopic();
        void subscribe();
        bool isPortByTopic(String topic);
        LogFlag changeStatus(uint8_t status);
        bool changeStatusMQTT(Topic topic);
        bool toggleStatusRX(Topic topic);
        void toggleStatus();
        bool loop();

        String toString();
};

#endif
