#include <Arduino.h>
#include <ThreeWire.h>
#include <RtcDS1302.h>
#include "SD.h"

#include "Configurations.h"
#include "domain/Log.h"

#ifndef DBController_h
#define DBController_h

// Classe que irá controlar transações do banco de dados
class DBController {
    private:
        sdcard_type_t _cardType = CARD_NONE;
        String _lastHourString = "--:--";

        String getCardTypeName ();
        bool hasSDCard ();
        void insertLogs (String date);

        String getDate(const RtcDateTime& dt);
        String getHour(const RtcDateTime& dt);
    public:
        DBController ();
        void appendLog(String data);
        void loop ();
        static void insertLogQueue (LogType logType, LogSource logSource, String key, String value, LogFlag applied, String obs);
};

#endif
