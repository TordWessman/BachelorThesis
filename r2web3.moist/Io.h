#ifndef __IO_H__
#define __IO_H__

#define DHTTYPE DHT11
//#include <DHTesp.h>
//#include <DHT11.h>
#include "DHT.h"


#define RESET_PULLUP_BUTTON 21 //D21
#define ERROR_LED 32//19
#define BOOT_LED 33//32
#define TRANSFER_LED 33
#define DHT11_PORT 4
#define PERIPHERALS_POWER_PORT 5

DHT dht(DHT11_PORT, DHTTYPE);

void setUpIO() {

   pinMode(RESET_PULLUP_BUTTON, INPUT_PULLUP);
   pinMode(ERROR_LED, OUTPUT);
   pinMode(BOOT_LED, OUTPUT);
   pinMode(TRANSFER_LED, OUTPUT);
   pinMode(PERIPHERALS_POWER_PORT, OUTPUT);
   digitalWrite(ERROR_LED, false);
   digitalWrite(BOOT_LED, false);
   digitalWrite(TRANSFER_LED, false);
}

bool shouldReset() {
  return digitalRead(RESET_PULLUP_BUTTON) == 0;
}

void setErrorLED(bool value) { digitalWrite(ERROR_LED, value); }
void setBootLED(bool value) { digitalWrite(BOOT_LED, value); }
void setTransferLED(bool value) { digitalWrite(TRANSFER_LED, value); }

void setPeripherals(bool available) {
  digitalWrite(PERIPHERALS_POWER_PORT, available);
  if (available) {
    delay(2000);
  }
}

uint16_t readTemperature() {
  uint16_t result = dht.readTemperature();
  delay(1000);
  return result;
}

uint16_t readHumidity() {
  uint16_t result = dht.readHumidity();
  delay(1000);
  return result;
}
#endif
