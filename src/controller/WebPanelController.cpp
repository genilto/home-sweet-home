#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>

#include "Configurations.h"
#include "controller/DisplayController.h"
#include "controller/WebPanelController.h"

WebPanelController::WebPanelController()
{
    this->_server.on("/", [this]() {
        this->showPanel();
    });
    this->_server.on("/changepanel", [this]() {
        DisplayController::updateInfo(DisplayKey::CMD, "CP", 0);
        this->showPanel();
    });
    this->_server.onNotFound([this]() {
        this->_server.send(404, "text/plain", "Não encontrado");
    });
}
void WebPanelController::showPanel() {
    this->_server.send(200, "text/html", "Painel Trocado! <br><input type=\"button\" onclick=\"jasvascript:window.location='/changepanel'\" value=\"Trocar Painel\">");
}
void WebPanelController::init()
{
    // Aguarda conexão com o Wifi
    while (!WiFi.isConnected());

    // Inicializa o servidor web
    this->_server.begin(80);
}
void WebPanelController::loop()
{
    this->_server.handleClient();
}