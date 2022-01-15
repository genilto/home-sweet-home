#include <Arduino.h>
#include <WiFi.h>

#include "Configurations.h"
#include "controller/DBController.h"
#include "controller/DisplayController.h"
#include "controller/WifiController.h"

WifiController::WifiController()
{
    // Define 30 segundos como timeout para conexão
    this->_timeout.setDelayTime(30000);
}
// Retorna o status da conexão
const char *WifiController::wifiStatusToString(int status)
{
    switch (status)
    {
    case WL_NO_SHIELD:
        return "WL_NO_SHIELD";
    case WL_IDLE_STATUS:
        return "WL_IDLE_STATUS";
    case WL_NO_SSID_AVAIL:
        return "WL_NO_SSID_AVAIL";
    case WL_SCAN_COMPLETED:
        return "WL_SCAN_COMPLETED";
    case WL_CONNECTED:
        return "WL_CONNECTED";
    case WL_CONNECT_FAILED:
        return "WL_CONNECT_FAILED";
    case WL_CONNECTION_LOST:
        return "WL_CONNECTION_LOST";
    case WL_DISCONNECTED:
        return "WL_DISCONNECTED";
    }
    return "WL_NOT_FOUND";
}
void WifiController::setConnected(bool connected)
{
    // Envia notificação para o Display quando ocorre alguma mudança
    if ((this->_connected && !connected) || (!this->_connected && connected))
    {
        String c = (connected ? "1" : "0");
        DisplayController::updateInfo(DisplayKey::WIFI_CHANGED, c);
        String obs = "Wifi Status: " + String(wifiStatusToString(WiFi.status()));

        if (connected)
        {
            obs += " - SSID: ";
            obs += WIFI_SSID;
            obs += " - IP: ";
            obs += WiFi.localIP().toString();
        }

        DBController::insertLogQueue(LogType::WIFI_CONNECTION, LogSource::INTERNAL, "@", c, LogFlag::YES, obs);
    }

    this->_connected = connected;
}
void WifiController::connect()
{
    extern SemaphoreHandle_t hshSemaphoreSerial;

    if (!WiFi.isConnected())
    {
        this->setConnected(false);

        if (hshSemaphoreSerial != NULL && xSemaphoreTake(hshSemaphoreSerial, (TickType_t)TICKS_TO_WAIT_SEMAPHORE) == pdTRUE)
        {
            Serial.print("WIFI: ");
            Serial.print(WiFi.status());
            Serial.print(" - ");
            Serial.print(wifiStatusToString(WiFi.status()));
            Serial.print(" -> Tentando conectar em ");
            Serial.println(WIFI_SSID);

            xSemaphoreGive(hshSemaphoreSerial);
        }

        // Conecta na rede WI-FI
        WiFi.mode(WIFI_STA);
        WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

        while (!WiFi.isConnected())
        {
            // Define um timeout para tentar novamente
            if (this->_timeout.expired())
            {
                // Sai do while e tenta uma nova conexão depois do delay da task
                // Definindo novamente as informações de conexão
                break;
            }
            delay(1000);
        }
    }

    // Mantém a informação de conexão atualizada
    this->setConnected(WiFi.isConnected());
}
void WifiController::init()
{
}
void WifiController::loop()
{
    connect();
}