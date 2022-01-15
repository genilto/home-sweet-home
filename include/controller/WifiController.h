#include <Arduino.h>
#include <WiFi.h>

#include "controller/DelayController.h"

#ifndef WifiController_h
#define WifiController_h

/***********************
Mantém conexão com Wifi
***********************/
class WifiController {
  private:
    bool _connected = false;
    DelayController _timeout;
    void connect();
    void setConnected(bool connected);
  public:
    WifiController();
    static const char* wifiStatusToString(int status);
    void init();
    void loop();
};

#endif
