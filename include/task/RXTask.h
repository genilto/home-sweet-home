#include <Arduino.h>

#ifndef RXTask_h
#define RXTask_h

/*******************************************************
* Irá controlar toda a parte de comunicação do painel
* sendo:
*   - Leitura dos interruptores 433Mhz
*******************************************************/
void rxTask( void *parameter );

#endif