#include <Arduino.h>
#include <RCSwitch.h>
#include "Configurations.h"
#include "domain/Topic.h"
#include "controller/DelayController.h"

#ifndef RXController_h
#define RXController_h

/**************************************************
* Mantém e gerencia a conexão com Mqtt
**************************************************/
class RXController {
  private:
    RCSwitch _switch = RCSwitch();
    //DelayController _delay;

    void changePort(String port);
  public:
    RXController();
    void init();
    void loop();
};

#endif
