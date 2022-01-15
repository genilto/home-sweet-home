#include <Arduino.h>

#ifndef MqttTask_h
#define MqttTask_h

/*******************************************************
* Irá controlar toda a parte de comunicação do painel
* sendo:
*   - Conexão com Servidor MQTT
*******************************************************/
void mqttTask( void *parameter );

#endif