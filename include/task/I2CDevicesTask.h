#include <Arduino.h>

#ifndef I2CDevicesTaskTask_h
#define I2CDevicesTaskTask_h

/*******************************************************
* Irá controlar toda a parte de comunicação I2C
* sendo:
*   - Display
*   - Módulo Controlador de Portas - MCP
*******************************************************/
void i2cDevicesTask( void *parameter );

#endif
