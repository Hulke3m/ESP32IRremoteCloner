#ifndef IRREMOTECLONER_H
#define IRREMOTECLONER_H

#include <Arduino.h>
#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRsend.h>
#include <IRutils.h>
#include <EEPROM.h>

#define RAW_BUFFER_LENGTH 100
#define MAX_COMMANDS 3
#define MAX_NAME_LEN 10
#define EEPROM_SIZE 900

struct IRCommand {
  char name[MAX_NAME_LEN + 1];
  uint16_t length;
  uint16_t raw[RAW_BUFFER_LENGTH];
};

class IRremoteCloner {
  public:
    IRremoteCloner();
    void begin(uint8_t recvPin, uint8_t sendPin);
    void loop();
    void printHelp();

  private:
    IRrecv* irrecv;
    IRsend* irsend;
    decode_results results;
    uint8_t RECV_PIN;
    uint8_t SEND_PIN;

    void setIRPins(uint8_t recvPin, uint8_t sendPin);
    void saveCommandToEEPROM(uint8_t slot, IRCommand &cmd);
    IRCommand loadCommandFromEEPROM(uint8_t slot);
    void clearCommandFromEEPROM(uint8_t slot);
    void clearAllCommandsFromEEPROM();
    int findCommandSlotByName(const String &target);
    void listCommands();
};

#endif
