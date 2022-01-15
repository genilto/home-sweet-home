#include <Arduino.h>

#ifndef Configurations_h
#define Configurations_h

/*******************************************************
* Configurações de pinos padrões
*******************************************************/
#define BUZZER_PIN   LED_BUILTIN // Usa o pino do LED Interno para o Buzzer
#define SDA_PIN      SDA         // 21
#define SCL_PIN      SCL         // 22
#define DISPLAY_ADDR 0x3c        // Endereço I2C do Display
#define ADS1115_ADDR 0x48        // Endereço I2C do Modulo de conversão analogico digital
#define ONE_WIRE_PIN 32          // Pino para conexões de um fio. Sensor de Temperatura por exemplo
#define DHT_SENSOR_PIN 33        // Pino para o sensor de temperatura e umidade
#define POWER_SENSOR_PIN 34      // Pino que irá identificar se existe tensão da rede (34 pino somente leitura, necessita de pulldown externo)
#define RX_PIN 12                // Pino de comunicação com a placa de RX 433 Mhz

// http://datasheets.maximintegrated.com/en/ds/DS1302.pdf
#define TM_RST_PIN 15  // Chip Enable  - RST
#define TM_DAT_PIN 4   // Input/Output - DAT
#define TM_CLK_PIN 13  // Serial Clock - CLK

/*---------------------------------------------------
 SD Card - ESP32 - Utilização do SQLite
https://github.com/siara-cc/esp32_arduino_sqlite3_lib
Connections for SD Card in SPI Mode:
    * SD Card   | ESP32
    *  DAT2        -
    *  DAT3        SS (D5)
    *  CMD         MOSI (D23)
    *  VSS         GND
    *  VDD         3.3V
    *  CLK         SCK (D18)
    *  DAT0        MISO (D19)
    *  DAT1        -
---------------------------------------------------*/
#define SD_CS_PIN   SS   // 5   - SS      CS (D5)
#define SD_MOSI_PIN MOSI // 23  - CMD     MOSI (D23)
#define SD_CLK_PIN  SCK  // 18  - CLK     SCK (D18)
#define SD_MISO_PIN MISO // 19  - DAT0    MISO (D19)

/*******************************************************
* Configurações de tempos padrões
*******************************************************/
#define TICKS_TO_WAIT_SEMAPHORE 100

/*******************************************************
* Configuraçõres do MQTT
*******************************************************/
#define MQTT_TOPIC_CODE_SIZE  30
#define MQTT_TOPIC_VALUE_SIZE 10

/*******************************************************
* Configuraçõres do Banco de Dados
*******************************************************/
#define LOG_FILE "/logs.txt"
#define LOG_FIELD_SIZE_DATE  20
#define LOG_FIELD_SIZE_KEY   10
#define LOG_FIELD_SIZE_VALUE 10
#define LOG_FIELD_SIZE_OBS   200

#define DISPLAY_FIELD_SIZE_VALUE 10

#endif