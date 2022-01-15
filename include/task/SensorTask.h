#include <Arduino.h>

#ifndef SensorTask_h
#define SensorTask_h

/*******************************************************
* Irá fazer as leituras de sensores em geral
* exceto sensores que trabalham com I2C
* Estes serão lidos dentro da tarefa que contra o I2C
*******************************************************/
void sensorTask( void *parameter );

#endif
