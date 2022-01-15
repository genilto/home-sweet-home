#include <Arduino.h>
#include "Configurations.h"
#include "controller/MqttController.h"
#include "task/MqttTask.h"

void mqttTask(void *parameter)
{
    MqttController mqttController;
    mqttController.init();

    // loop forever
    for (;;)
    {
        mqttController.loop();
        vTaskDelay(1);
    }

    // delete a task when finish, this will never happen because this is infinity loop
    vTaskDelete(NULL);
}
