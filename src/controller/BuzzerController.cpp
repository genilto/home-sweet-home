#include <Arduino.h>
#include "Configurations.h"
#include "EasyBuzzer.h"
#include "controller/BuzzerController.h"

BuzzerController::BuzzerController () {}

void BuzzerController::init() {
    // Define o pino do Buzzer
    EasyBuzzer.setPin(BUZZER_PIN);
}
void BuzzerController::doBeep(unsigned int beeps) {
    if (beeps > 0) {
        EasyBuzzer.beep(3000, beeps, BuzzerController::finishedBeep);
    }
}
void BuzzerController::finishedBeep() {
    
}
void BuzzerController::beep(unsigned int beeps) {
    extern QueueHandle_t hshBeepQueue;

    if (beeps > 0) {
        // Adiciona o pacote recebido na queue
        //xQueueOverwrite ( hshBeepQueue, (void *) &beeps );
        xQueueSend( hshBeepQueue, (void *) &beeps, ( TickType_t ) 10 );
    }
}
void BuzzerController::loop() {
    extern QueueHandle_t hshBeepQueue;

    // Always call this function in the loop for EasyBuzzer to work. //
    EasyBuzzer.update();

    // Verifica se existe na fila alguma solicitação de beeps
    // Não deverá bloquear em caso de não ter nada na fila
    unsigned int beeps = 0;
    if (xQueueReceive(hshBeepQueue, &(beeps), 0) == pdTRUE) {
        this->doBeep(beeps);
    }
}