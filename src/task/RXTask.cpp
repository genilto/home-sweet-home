#include <Arduino.h>
#include "Configurations.h"
#include "controller/RXController.h"
#include "task/RXTask.h"

void rxTask(void *parameter)
{
    RXController rxController;
    rxController.init();

    // loop forever
    for (;;)
    {
        rxController.loop();
        vTaskDelay(1);
    }

    // delete a task when finish, this will never happen because this is infinity loop
    vTaskDelete(NULL);
}
