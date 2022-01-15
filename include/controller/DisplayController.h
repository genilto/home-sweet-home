#include <Arduino.h>
#include "SSD1306Wire.h"
#include "Configurations.h"
#include "domain/DisplayInfo.h"
#include "controller/DelayController.h"

#ifndef DisplayController_h
#define DisplayController_h

class DisplayController
{
private:
    SSD1306Wire *_display;
    DelayController _timeToChangeDisplay;

    // Informações do próprio display
    unsigned int _panelIndex = 0;
    bool _mustUpdate = true;
    unsigned int _updateQty = 0;

    /* Informações do Display */
    String _hour = "--:--";
    bool _wifiConnected = false;
    bool _mqttConnected = false;
    bool _hasPower = false;
    bool _SDCardInitialized = false;
    int _batteryIcon            = 0;
    String _batteryVoltage      = "-.--";
    String _internalTemperature = "--.--";
    String _internalHumidity    = "--";
    String _externalTemperature = "--.--";
    String _boilerTemperature   = "--.--";
    String _waterTankLevel      = "--";
    String _memoryPercent       = "--";

    void drawLogo();
    void drawHeader();
    void drawPanel();
    const uint8_t* getBatteryIcon ();
    void drawIconInfo(int posX, int posY, const uint8_t icon[], String value);

public:
    DisplayController(SSD1306Wire &display);
    unsigned int getPanelIndex();
    void changePanel();
    void init();
    void setMustUpdate(int panelIndex);
    bool mustUpdate();
    void update();    
    
    static void updateInfo(DisplayKey key, String value, int icon);
    static void updateInfo(DisplayKey key, String value);
};

#endif
