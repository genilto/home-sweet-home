#include <Arduino.h>
#include <Adafruit_MCP23017.h>
#include "SSD1306Wire.h"
#include "controller/PortController.h"

#ifndef PortListController_h
#define PortListController_h

/******************
Phy Name	ID
21	GPA0	0
22	GPA1	1
23	GPA2	2
24	GPA3	3
25	GPA4	4
26	GPA5	5
27	GPA6	6
28	GPA7	7
1	GPB0	8
2	GPB1	9
3	GPB2	10
4	GPB3	11
5	GPB4	12
6	GPB5	13
7	GPB6	14
8	GPB7	15
******************/
class PortListController {
  private:
    Adafruit_MCP23017 _mcp;
    SSD1306Wire* _display;
    String _user;
    
    // Configuração padrão das portas
    PortController defaultPorts[8] = {
        { 0, 12, 11, "@", "@", "N/A", 0 },  // 0 - B4 - B3
        { 1, 13, 10, "@", "@", "N/A", 0 },  // 1 - B5 - B2
        { 2, 14, 9,  "@", "@", "N/A", 0 },  // 2 - B6 - B1
        { 3, 15, 8,  "@", "@", "N/A", 0 },  // 3 - B7 - B0
        { 4, 7,  0,  "@", "@", "N/A", 0 },  // 4 - A7 - A0
        { 5, 6,  1,  "@", "@", "N/A", 0 },  // 5 - A6 - A1
        { 6, 5,  2,  "@", "@", "N/A", 0 },  // 6 - A5 - A2
        { 7, 4,  3,  "@", "@", "N/A", 0 },  // 7 - A4 - A3
    };

    void definePort(uint8_t index, String gbridge, String name);
    PortController* getPortByTopic(String topic);
    String getFullTopic(String gbridge);
    void printPort (int posX, int posY, int status, String name);
  public:
    PortListController(SSD1306Wire& display, String user);
    void init();
    bool loop(bool updateDisplay);
};

#endif
