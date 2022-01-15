#include <Arduino.h>
#include <Wire.h>
#include "SSD1306Wire.h"
#include <Adafruit_ADS1015.h>

#include "Configurations.h"
#include "domain/Log.h"
#include "controller/DelayController.h"
#include "controller/DisplayController.h"
#include "controller/PortListController.h"
#include "controller/AnalogController.h"
#include "controller/DBController.h"
#include "task/I2CDevicesTask.h"

/*******************************************************
* Irá controlar toda a parte de comunicação I2C
* sendo:
*   - Display
*   - Módulo Controlador de Portas - MCP
*******************************************************/
void i2cDevicesTask(void *parameter)
{
    // Instancia o Display
    SSD1306Wire display(DISPLAY_ADDR, SDA_PIN, SCL_PIN);
    Adafruit_ADS1115 ads(ADS1115_ADDR);

    // Inicializa o controlador do Display
    DisplayController displayController(display);

    // Cria uma instancia do controlador de portas
    PortListController portListController(display, "u1");

    // Irá controlar as leituras do ADS
    AnalogController analogController(ads);

    // Inicializa o Display
    displayController.init();

    // Inicia as configurações do controlador de portas
    // Nesse momento irá carregar quais portas estão ligadas ou desligadas
    // e os nomes de portas e configurações do GBRIDGE
    portListController.init();

    // Inicia o ADS1015
    analogController.init();

    /* loop forever */
    for (;;)
    {
        int panelPorts = 0;
        bool mustUpdateDisplay = displayController.mustUpdate();
        bool mustUpdatePorts = (mustUpdateDisplay && 
                                    displayController.getPanelIndex() == panelPorts);

        // Controle das portas do MCP
        if (portListController.loop(mustUpdatePorts))
        {
            displayController.setMustUpdate(panelPorts);
        }

        // Executa o loop do controlador ADS1115
        analogController.loop();

        if (mustUpdateDisplay)
        {
            // Loop do Display
            displayController.update();
        }

        yield();
    }

    /* delete a task when finish, this will never happen because this is infinity loop */
    vTaskDelete(NULL);
}