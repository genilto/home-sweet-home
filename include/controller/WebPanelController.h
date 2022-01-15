#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>

#ifndef WebPanelController_h
#define WebPanelController_h

/***********************
Disponibiliza um painel web para consulta e alteração de configurações
***********************/
class WebPanelController {
  private:
    WebServer _server;

    void showPanel();
  public:
    WebPanelController();
    void init();
    void loop();
};

#endif
