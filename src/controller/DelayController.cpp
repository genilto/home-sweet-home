#include <Arduino.h>
#include "Configurations.h"
#include "controller/DelayController.h"

DelayController::DelayController () {}
DelayController::DelayController (int delayTime) {
    this->setDelayTime( delayTime );
}

void DelayController::setDelayTime (int delayTime) {
    this->_delayTime = delayTime;
}

bool DelayController::expired () {
    int timeSinceLast = (millis() - this->_startTime);
    if(timeSinceLast >= this->_delayTime) {
        this->reset();
        return true;
    }
    return false;
}

void DelayController::reset () {
    this->_startTime = millis();
}