#include <Arduino.h>
#include "Configurations.h"
#include "domain/DisplayInfo.h"
#include "controller/DelayController.h"
#include "controller/DisplayController.h"
#include "controller/DBController.h"
#include "controller/SensorController.h"
#include "task/SensorTask.h"

void sensorTask(void *parameter)
{
    // Inicializa o controlador dos sensores
    SensorController sensors;
    sensors.init();

    /* loop forever */
    for (;;)
    {
        sensors.loop();
        delay(1500);
    }

    /* delete a task when finish, this will never happen because this is infinity loop */
    vTaskDelete(NULL);
}