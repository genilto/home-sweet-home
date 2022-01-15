#include <Arduino.h>
#include "Configurations.h"
#include "images.h"
#include "controller/DisplayController.h"

DisplayController::DisplayController(SSD1306Wire &display)
{
    this->_display = &display;
    this->_timeToChangeDisplay.setDelayTime(15000);
}
unsigned int DisplayController::getPanelIndex()
{
    return this->_panelIndex;
}
void DisplayController::changePanel()
{
    if (this->_panelIndex >= 1)
    {
        this->_panelIndex = 0;
    }
    else
    {
        this->_panelIndex++;
    }
    this->setMustUpdate(-1);
}
void DisplayController::init()
{
    // Initialising the UI will init the display too.
    this->_display->init();

    // Configurações do Display
    this->_display->flipScreenVertically();
    this->_display->setFont(ArialMT_Plain_10);
    this->_display->setTextAlignment(TEXT_ALIGN_LEFT);

    this->drawLogo();
}
void DisplayController::setMustUpdate(int panelIndex)
{
    if (panelIndex == -1 || this->getPanelIndex() == panelIndex)
    {
        this->_mustUpdate = true;
    }
}
bool DisplayController::mustUpdate()
{
    extern QueueHandle_t hshDisplayQueue;
    DisplayInfo displayInfo;
    int indexChanged = -2; // Nenhuma ação

    // Verifica se tem algo na fila
    // Verifica a Fila caso exista algo a ser publicado
    // Não deverá bloquear em caso de não ter nada na fila
    if (xQueueReceive(hshDisplayQueue, &(displayInfo), 0) == pdTRUE)
    {
        // Comando a ser executado
        if (displayInfo.key == DisplayKey::CMD)
        {
            String cmd = String(displayInfo.value);
            String icon = String(displayInfo.icon);
            if (cmd.equals("CP")){
                this->changePanel();
            }
        }
        else if (displayInfo.key == DisplayKey::WIFI_CHANGED)
        {
            this->_wifiConnected = String(displayInfo.value).equals("1");
            indexChanged = -1;
        }
        else if (displayInfo.key == DisplayKey::MQTT_CHANGED)
        {
            this->_mqttConnected = String(displayInfo.value).equals("1");
            indexChanged = -1;
        }
        else if (displayInfo.key == DisplayKey::POWER_CHANGED)
        {
            this->_hasPower = String(displayInfo.value).equals("1");
            indexChanged = -1;
        }
        else if (displayInfo.key == DisplayKey::SDCARD_CHANGED)
        {
            this->_SDCardInitialized = String(displayInfo.value).equals("1");
            indexChanged = -1;
        }
        else if (displayInfo.key == DisplayKey::HOUR_CHANGED)
        {
            this->_hour = String(displayInfo.value);
            indexChanged = -1;
        }
        else if (displayInfo.key == DisplayKey::BAT_VOLTAGE_CHANGED)
        {
            this->_batteryIcon = displayInfo.icon;
            this->_batteryVoltage = String(displayInfo.value);
            indexChanged = 1;
        }
        else if (displayInfo.key == DisplayKey::MEMORY_CHANGED)
        {
            this->_memoryPercent = String(displayInfo.value);
            indexChanged = 1;
        }
        else if (displayInfo.key == DisplayKey::EXT_TEMP_CHANGED)
        {
            this->_externalTemperature = String(displayInfo.value);
            indexChanged = 1;
        }
        else if (displayInfo.key == DisplayKey::BOILER_TEMP_CHANGED)
        {
            this->_boilerTemperature = String(displayInfo.value);
            indexChanged = 1;
        }
        else if (displayInfo.key == DisplayKey::INT_TEMP_CHANGED)
        {
            this->_internalTemperature = String(displayInfo.value);
            indexChanged = 1;
        }
        else if (displayInfo.key == DisplayKey::INT_HUMIDITY_CHANGED)
        {
            this->_internalHumidity = String(displayInfo.value);
            indexChanged = 1;
        }
        else if (displayInfo.key == DisplayKey::WATER_LEVEL_CHANGED)
        {
            this->_waterTankLevel = String(displayInfo.value);
            indexChanged = 1;
        }
        // Apenas marca para atualizar o visor quando alguma coisa modificar no painel corrente
        this->setMustUpdate(indexChanged);
    }

    if (this->_mustUpdate)
    {
        this->_display->clear();
        return true;
    }
    return false;
}
void DisplayController::update()
{
    this->_updateQty++;

    // Atualiza o display
    this->drawHeader();
    this->drawPanel();
    this->_display->display();

    // Após atualizar, define que não precisa mais atualiza
    // até que alguma alteração ocorra
    this->_mustUpdate = false;
}
void DisplayController::drawLogo()
{
    this->_display->clear();
    this->_display->drawXbm(34, 14, WiFi_Logo_width, WiFi_Logo_height, WiFi_Logo_bits);
    this->_display->display();
}
void DisplayController::drawHeader()
{
    int posX = 0;
    int distance = 18;
    // Exibe o ícones de Energia
    if (this->_hasPower)
    {
        this->_display->drawXbm(posX, 0, Icon_width, Icon_height, plug_icon16x16);
    }
    else
    {
        this->_display->drawXbm(posX, 0, Icon_width, Icon_height, this->getBatteryIcon());
    }

    posX += distance;
    // Exibe o ícone do cartão SD
    if (this->_SDCardInitialized)
    {
        this->_display->drawXbm(posX, 0, Icon_width, Icon_height, sdcard_icon16x16);
    }

    posX += distance;
    // Exibe o ícones de conexão Wifi quando conectado
    if (this->_wifiConnected)
    {
        this->_display->drawXbm(posX, 0, Icon_width, Icon_height, wifi1_icon16x16);
    }

    posX += distance;
    // Exibe o ícones de conexão MQTT quando conectado
    if (this->_mqttConnected)
    {
        this->_display->drawXbm(posX, 0, Icon_width, Icon_height, speak_icon16x16);
    }

    posX += (distance + 3);
    // Exibe o percentual de memória utilizada pelo ESP
    this->_display->drawString(posX, 0, this->_memoryPercent + "%");

    // Exibe a hora atual
    this->_display->drawString(100, 0, this->_hour);
}
const uint8_t *DisplayController::getBatteryIcon()
{
    if (this->_hasPower)
        return bat4_icon16x16;
    else if (this->_batteryIcon == 1)
        return bat1_icon16x16;
    else if (this->_batteryIcon == 2)
        return bat2_icon16x16;
    else if (this->_batteryIcon == 3)
        return bat3_icon16x16;
    return bat0_icon16x16;
}
void DisplayController::drawPanel()
{
    // Panel 0 exibe informações das portas, diretamente no controlador de portas

    // Panel 1 exibe demais informações
    if (this->_panelIndex == 1)
    {
        int posX = 0;
        int posY = 15;

        // 1.1
        // Exibe a Temperatura Interna
        this->drawIconInfo(posX, posY, temperature_icon16x16, this->_internalTemperature + "°C");

        // 1.2
        posY += 16;
        // Exibe a Temperatura Externa
        this->drawIconInfo(posX, posY, temperature_icon16x16, this->_externalTemperature + "°C");

        // 1.3
        posY += 16;
        // Exibe a temperatura do Boiler
        this->drawIconInfo(posX, posY, sun_icon16x16, this->_boilerTemperature + "°C");

        // 2.1
        posX = 70;
        posY = 15;
        // Exibe a Umidade Interna
        this->drawIconInfo(posX, posY, humidity_icon16x16, this->_internalHumidity + "%");

        // 2.2
        posY += 16;
        // Exibe o nível da Caixa Dágua
        this->drawIconInfo(posX, posY, water_tap_icon16x16, this->_waterTankLevel + "%");

        // Exibe a quantidade de memória sendo utilizada pelo ESP
        //this->drawIconInfo(posX, posY, fillstate4_icon16x16, this->_memoryPercent + "%");

        // 2.3
        posY += 16;
        // Exibe a tensão da bateria
        this->drawIconInfo(posX, posY, this->getBatteryIcon(), this->_batteryVoltage + "v");

        // Quantidade de Atualizações do display
        //this->_display->drawString(45, 50, String(this->_updateQty));
    }
}
void DisplayController::drawIconInfo(int posX, int posY, const uint8_t icon[], String value)
{
    this->_display->drawXbm(posX, posY, Icon_width, Icon_height, icon);
    this->_display->drawString((posX + Icon_width + 3), (posY + 2), value);
}
void DisplayController::updateInfo(DisplayKey key, String value, int icon)
{
    extern QueueHandle_t hshDisplayQueue;

    // Monta a struct para envio dos dados
    struct DisplayInfo info;
    info.key = key;
    strncpy(info.value, value.c_str(), DISPLAY_FIELD_SIZE_VALUE);
    info.icon = icon;

    // Adiciona o pacote recebido na queue
    xQueueSend(hshDisplayQueue, (void *)&info, (TickType_t)10);
}
void DisplayController::updateInfo(DisplayKey key, String value)
{
    updateInfo(key, value, 0);
}