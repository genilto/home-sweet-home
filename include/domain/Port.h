#include <Arduino.h>

#ifndef Port_h
#define Port_h

// Classe com as informações referentes a portas
struct Port {
    uint8_t id;         // ID da porta
    uint8_t switchPort; // Numero da porta referente a entrada do interruptor
    uint8_t relayPort;  // Numero da porta referente a saida para o relê
    String  user;       // Código do usuário do GBRIDGE
    String  gbridge;    // Código do dispositivo no GBRIDGE
    String  name;       // Nome do dispositivo
    uint8_t status;     // Indicativo de se a porta esta ligada ou desligada
};

#endif
