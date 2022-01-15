#include <Arduino.h>
#include "Configurations.h"
#include "controller/WifiController.h"
#include "task/WifiTask.h"

void wifiTask(void *parameter)
{
    WifiController wifiController;
    wifiController.init();

    /* loop forever */
    for (;;)
    {
        wifiController.loop();
        delay(5000);
    }

    vTaskDelete(NULL);
}
