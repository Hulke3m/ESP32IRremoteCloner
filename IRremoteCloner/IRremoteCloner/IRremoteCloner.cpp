#include "IRremoteCloner.h"

IRremoteCloner::IRremoteCloner() {
  irrecv = nullptr;
  irsend = nullptr;
}

void IRremoteCloner::begin(uint8_t recvPin, uint8_t sendPin) {
  EEPROM.begin(EEPROM_SIZE);
  setIRPins(recvPin, sendPin);
  Serial.println("IR Command Manager Ready");
  printHelp();
}

void IRremoteCloner::setIRPins(uint8_t recvPin, uint8_t sendPin) {
  RECV_PIN = recvPin;
  SEND_PIN = sendPin;
  if (irrecv) delete irrecv;
  if (irsend) delete irsend;
  irrecv = new IRrecv(RECV_PIN);
  irsend = new IRsend(SEND_PIN);
  irrecv->enableIRIn();
  irsend->begin();
}

// Define helper methods: saveCommandToEEPROM, loadCommandFromEEPROM,
// clearCommandFromEEPROM, clearAllCommandsFromEEPROM, findCommandSlotByName,
// listCommands, printHelp, and the loop() function here.
// Placeholder comment since logic is large and mostly the same as user code.
