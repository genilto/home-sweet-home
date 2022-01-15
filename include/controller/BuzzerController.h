#include <Arduino.h>
#include "EasyBuzzer.h"

#ifndef BuzzerController_h
#define BuzzerController_h

class BuzzerController {
  private:
    void doBeep(unsigned int beeps);
  public:
    BuzzerController();
    void init();
    void loop();
    static void beep(unsigned int beeps);
    static void finishedBeep();
};

#endif
