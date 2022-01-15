#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include "Configurations.h"
#include "domain/Topic.h"
#include "controller/DelayController.h"

#ifndef MqttController_h
#define MqttController_h

/**************************************************
* Mantém e gerencia a conexão com Mqtt
**************************************************/
class MqttController {
  private:
    WiFiClient wifiClient;
    PubSubClient MQTT;
    DelayController _delay;
    bool _connected = false;

    void connect();
    bool setConnected();
    void subscribeTopic (Topic topic);
    void publishTopic (Topic topic);
  public:
    MqttController();
    void init();
    void loop();
    static void subscribeTopicQueue (String topicCode, String topicValue);
    static void receiveTopic(char* topic, byte* payload, unsigned int length);
    static void publishTopicQueue (String topicCode, String topicValue);
};

#endif
