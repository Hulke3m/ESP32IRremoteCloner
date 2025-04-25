#ifndef IRREMOTECLONER_H
#define IRREMOTECLONER_H

#include <Arduino.h>

class IRremoteCloner {
public:
  static void begin();
  static void handleSerial();
};

#endif