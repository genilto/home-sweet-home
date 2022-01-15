#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
//#include <ESPmDNS.h>

#include "Configurations.h"
#include "controller/WebPanelController.h"
#include "controller/DelayController.h"
#include "task/WebServerTask.h"

void webServerTask(void *parameter)
{
    extern SemaphoreHandle_t hshSemaphoreSerial;

    if (hshSemaphoreSerial != NULL && xSemaphoreTake(hshSemaphoreSerial, (TickType_t)TICKS_TO_WAIT_SEMAPHORE) == pdTRUE)
    {
        Serial.print("Core do webServerTask: ");
        Serial.println(xPortGetCoreID());

        xSemaphoreGive(hshSemaphoreSerial);
    }

    WebPanelController webPanelController;
    DelayController delay(20000);

    webPanelController.init();

    /* loop forever */
    for (;;)
    {
        webPanelController.loop();

        if (delay.expired())
        {
            if (hshSemaphoreSerial != NULL && xSemaphoreTake(hshSemaphoreSerial, (TickType_t)TICKS_TO_WAIT_SEMAPHORE) == pdTRUE)
            {
                Serial.print("webServerTask: uxTaskGetStackHighWaterMark: ");
                Serial.println(uxTaskGetStackHighWaterMark(NULL));

                xSemaphoreGive(hshSemaphoreSerial);
            }
        }
        yield();
    }

    /* delete a task when finish, this will never happen because this is infinity loop */
    vTaskDelete(NULL);
}
