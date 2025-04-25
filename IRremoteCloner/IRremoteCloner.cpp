#include "IRremoteCloner.h"
#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRsend.h>
#include <IRutils.h>
#include <EEPROM.h>

#define RECV_PIN 34
#define SEND_PIN 25
#define RAW_BUFFER_LENGTH 100
#define MAX_COMMANDS 3
#define MAX_NAME_LEN 10
#define EEPROM_SIZE 900

IRrecv irrecv(RECV_PIN);
IRsend irsend(SEND_PIN);
decode_results results;

struct IRCommand {
  char name[MAX_NAME_LEN + 1];
  uint16_t length;
  uint16_t raw[RAW_BUFFER_LENGTH];
};

void printHelp();
void saveCommandToEEPROM(uint8_t slot, IRCommand &cmd);
IRCommand loadCommandFromEEPROM(uint8_t slot);
void clearCommandFromEEPROM(uint8_t slot);
void clearAllCommandsFromEEPROM();
int findCommandSlotByName(const String &target);
void listCommands();

void IRremoteCloner::begin() {
  EEPROM.begin(EEPROM_SIZE);
  irrecv.enableIRIn();
  irsend.begin();
  Serial.println("IR Command Manager Ready");
  printHelp();
}

void IRremoteCloner::handleSerial() {
  if (Serial.available()) {
    String input = Serial.readStringUntil('\n');
    input.trim();

    if (input == "record") {
      Serial.println("Waiting for IR signal...");
      while (!irrecv.decode(&results)) delay(10);

      IRCommand cmd;
      memset(&cmd, 0, sizeof(cmd));
      cmd.length = results.rawlen - 1;
      if (cmd.length > RAW_BUFFER_LENGTH) cmd.length = RAW_BUFFER_LENGTH;
      for (int i = 1; i <= cmd.length; i++) {
        cmd.raw[i - 1] = results.rawbuf[i] * kRawTick;
      }
      irrecv.resume();
      Serial.println("IR signal captured!");

      Serial.print("Enter a name for this command (max 10 chars): ");
      while (!Serial.available()) delay(10);
      String name = Serial.readStringUntil('\n');
      name.trim();
      name.toCharArray(cmd.name, MAX_NAME_LEN + 1);

      int existingSlot = findCommandSlotByName(name);
      if (existingSlot != -1) {
        Serial.print("Name already exists. Overwrite? (y/n): ");
        while (!Serial.available()) delay(10);
        String confirm = Serial.readStringUntil('\n');
        confirm.trim();
        if (!confirm.equalsIgnoreCase("y")) {
          Serial.println("Canceled.");
          printHelp();
          return;
        }
        saveCommandToEEPROM(existingSlot, cmd);
      } else {
        bool saved = false;
        for (int i = 0; i < MAX_COMMANDS; i++) {
          IRCommand test = loadCommandFromEEPROM(i);
          if (strlen(test.name) == 0) {
            saveCommandToEEPROM(i, cmd);
            saved = true;
            break;
          }
        }
        if (!saved) Serial.println("EEPROM full. Could not save command.");
        printHelp();
      }
    } else if (input.startsWith("play ")) {
      String name = input.substring(5);
      name.trim();
      int slot = findCommandSlotByName(name);
      if (slot == -1) {
        Serial.println("Command not found.");
      } else {
        IRCommand cmd = loadCommandFromEEPROM(slot);
        Serial.printf("Sending command '%s' 10 times...\n", cmd.name);
        for (int i = 0; i < 10; i++) {
          Serial.printf("Sending '%s': Attempt %d of 10\n", cmd.name, i + 1);
          irsend.sendRaw(cmd.raw, cmd.length, 38);
          delay(1000);
        }
      }
      printHelp();
    } else if (input.startsWith("delete ")) {
      String name = input.substring(7);
      name.trim();
      if (name.equalsIgnoreCase("all")) {
        clearAllCommandsFromEEPROM();
      } else {
        int slot = findCommandSlotByName(name);
        if (slot == -1) {
          Serial.println("Command not found.");
        } else {
          clearCommandFromEEPROM(slot);
          Serial.printf("Deleted command '%s'\n", name.c_str());
        }
        printHelp();
      }
    } else if (input == "list") {
      listCommands();
    } else {
      Serial.println("Unknown command. Use: record, play <name>, delete <name>, delete all, list");
      printHelp();
    }
  }
}

void printHelp() {
  Serial.println("Available commands:");
  Serial.println("  record           - capture and name a new IR command");
  Serial.println("  play <name>      - send a saved command");
  Serial.println("  list             - show saved commands");
  Serial.println("  delete <name>    - remove a saved command");
  Serial.println("  delete all       - remove all saved commands");
}

void saveCommandToEEPROM(uint8_t slot, IRCommand &cmd) {
  if (slot >= MAX_COMMANDS) return;
  int addr = slot * 300;
  EEPROM.put(addr, cmd);
  EEPROM.commit();
  Serial.printf("Saved command '%s' to slot %d\n", cmd.name, slot);
  printHelp();
}

IRCommand loadCommandFromEEPROM(uint8_t slot) {
  IRCommand cmd;
  memset(&cmd, 0, sizeof(cmd));
  if (slot >= MAX_COMMANDS) return cmd;
  int addr = slot * 300;
  EEPROM.get(addr, cmd);
  return cmd;
}

void clearCommandFromEEPROM(uint8_t slot) {
  if (slot >= MAX_COMMANDS) return;
  IRCommand empty;
  memset(&empty, 0, sizeof(empty));
  int addr = slot * 300;
  EEPROM.put(addr, empty);
  EEPROM.commit();
}

void clearAllCommandsFromEEPROM() {
  for (int i = 0; i < MAX_COMMANDS; i++) {
    clearCommandFromEEPROM(i);
  }
  Serial.println("All commands deleted.");
  printHelp();
}

int findCommandSlotByName(const String &target) {
  for (int i = 0; i < MAX_COMMANDS; i++) {
    IRCommand temp = loadCommandFromEEPROM(i);
    if (target.equalsIgnoreCase(temp.name)) {
      return i;
    }
  }
  return -1;
}

void listCommands() {
  Serial.println("Saved commands:");
  for (int i = 0; i < MAX_COMMANDS; i++) {
    IRCommand temp = loadCommandFromEEPROM(i);
    if (strlen(temp.name) > 0) {
      Serial.printf("  [%d] %s\n", i, temp.name);
    }
  }
  printHelp();
}