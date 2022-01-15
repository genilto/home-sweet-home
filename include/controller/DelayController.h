#include <Arduino.h>

#ifndef DelayController_h
#define DelayController_h

// Classe com as informações que serão exibidas no Display
class DelayController {
    private:
        int _delayTime  = 500;
        long _startTime = 0;
    public:
        DelayController ();
        DelayController (int delayTime);
        void setDelayTime (int delayTime);
        void reset ();
        bool expired ();
};

#endif
