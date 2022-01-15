#include <Arduino.h>
#include "Configurations.h"
#include "controller/DBController.h"
#include "task/DBTask.h"

void dbTask(void *parameter)
{
    DBController db;

    /* loop forever */
    for (;;)
    {
        db.loop();
        delay(1000);
    }

    /* delete a task when finish, this will never happen because this is infinity loop */
    vTaskDelete(NULL);
}
