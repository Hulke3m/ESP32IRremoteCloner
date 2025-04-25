#include <IRremoteCloner.h>

void setup() {
  Serial.begin(115200);
  delay(200);
  IRremoteCloner::begin();
}

void loop() {
  IRremoteCloner::handleSerial();
}