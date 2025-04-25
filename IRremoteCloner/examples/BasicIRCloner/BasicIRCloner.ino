#include <IRremoteCloner.h>

IRremoteCloner cloner;

void setup() {
  Serial.begin(115200);
  cloner.begin(34, 25);  // Set your IR receiver and sender pins
}

void loop() {
  cloner.loop();
}
