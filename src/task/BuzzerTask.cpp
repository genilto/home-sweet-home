#include <Arduino.h>
#include "Configurations.h"
#include "controller/BuzzerController.h"
#include "task/BuzzerTask.h"

void buzzerTask(void *parameter)
{
    BuzzerController buzzerController;
    buzzerController.init();

    /* loop forever */
    for (;;)
    {
        buzzerController.loop();
        vTaskDelay(10);
    }

    vTaskDelete(NULL);
}
