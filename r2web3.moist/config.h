#ifndef __CONFIG__H__
#define __CONFIG__H__

#include <EEPROM.h>

#define EEPROM_PK_ADDRESS 10
#define ETH_PK_LENGTH 40
#define EEPROM_CONFIG_STATUS_ADDRESS (EEPROM_PK_ADDRESS + ETH_PK_LENGTH + 2)
#define CONFIG_STATUS_SIZE 2
#define CONFIG_STATUS_COMPLETE 0x1234
#define EEPROM_SSID_ADDRESS (EEPROM_CONFIG_STATUS_ADDRESS + CONFIG_STATUS_SIZE)
#define EEPROM_SSID_SIZE 100
#define EEPROM_WIFI_PASSWORD_ADDRESS (EEPROM_SSID_ADDRESS + EEPROM_SSID_SIZE)
#define EEPROM_WIFI_PASSWORD_SIZE 100
#define EEPROM_CONTRACT_ADDRESS_ADDRESS (EEPROM_WIFI_PASSWORD_ADDRESS + EEPROM_WIFI_PASSWORD_SIZE)
#define EEPROM_CONTRACT_ADDRESS_SIZE 43
#define EEPROM_URL_ADDRESS (EEPROM_CONTRACT_ADDRESS_ADDRESS + EEPROM_CONTRACT_ADDRESS_SIZE)
#define EEPROM_URL_SIZE 100
#define EEPROM_ERROR_CODE_ADDRESS (EEPROM_URL_ADDRESS + EEPROM_URL_SIZE)
#define EEPROM_ERROR_CODE_SIZE 1

#define EEPROM_SIZE (EEPROM_ERROR_CODE_ADDRESS + EEPROM_ERROR_CODE_SIZE + 1)

struct AppConfig {

  AppConfig(char *ssid, char *password, char *contractAddress, char *chainURL, bool isSet) : ssid(ssid), password(password), contractAddress(contractAddress), chainURL(chainURL), isSet(isSet) {}
  AppConfig() : ssid(NULL), password(NULL), contractAddress(NULL), chainURL(NULL), isSet(false) {}
 
  char *ssid;
  char *password;
  char *contractAddress;
  char *chainURL;
  bool isSet;  
};

void eeprom_init() { EEPROM.begin(EEPROM_SIZE); }
void eeprom_end() { EEPROM.end(); }

uint8_t eeprom_read(size_t pos) { return EEPROM.read(pos); }

// First byte read is the length of the string
char *eeprom_read_string(size_t pos) {

    size_t length = eeprom_read(pos);

    char *data = new char[length + 1];
    for(size_t i = 0; i < length; i++) {
      data[i] = eeprom_read(pos + i + 1);
    }
    data[length] = '\0';
    return data;
}

// First byte written is the length of the string
void eeprom_write_string(size_t pos, char *val) { 
  size_t i = 0;
  EEPROM.write(pos, strlen(val));
  for(i = 0; i < strlen(val); i++) {
    EEPROM.write(pos + i + 1, val[i]);
  }
  EEPROM.commit();
}

void eeprom_write(size_t pos, uint8_t val) { 
  EEPROM.write(pos, val); 
  EEPROM.commit();
}

std::vector<uint8_t> generatePrivateKey()
{
    Serial.print("Generating private key:");
    std::vector<uint8_t> pk(ETH_PK_LENGTH);
    for (size_t i = 0; i < ETH_PK_LENGTH; i++)
    {
      
        pk[i] = ((uint8_t)rand());
        Serial.print(" "); Serial.print(pk[i]);
    }
    
    Serial.println();

    return pk;
}

std::vector<uint8_t> loadPrivateKey() {
    std::vector<uint8_t> pk(ETH_PK_LENGTH);
    for (size_t i = 0; i < ETH_PK_LENGTH; i++)
    {
        pk[i] = eeprom_read(EEPROM_PK_ADDRESS + i);
    }

    return pk;
}

void writeError(uint8_t code) { eeprom_write(EEPROM_ERROR_CODE_ADDRESS, code); }
void clearError() { eeprom_write(EEPROM_ERROR_CODE_ADDRESS, 0); }
bool hasError() { return eeprom_read(EEPROM_ERROR_CODE_ADDRESS) == 0; }
uint8_t getError() { return eeprom_read(EEPROM_ERROR_CODE_ADDRESS); }


void setPrivateKey(std::vector<uint8_t> pk) {

  Serial.print("Setting private key:");
  for(uint8_t i = 0; i < ETH_PK_LENGTH; i++) {
      eeprom_write(EEPROM_PK_ADDRESS + i, pk[i]);
      Serial.print(" "); Serial.print(pk[i]);
  }
  Serial.println();
}

void setConfigurationDone() {
    for(uint8_t i = 0; i < CONFIG_STATUS_SIZE; i++) {
        uint8_t v = (CONFIG_STATUS_COMPLETE & (0xFF << (i * 8))) >> (i * 8);
        eeprom_write(EEPROM_CONFIG_STATUS_ADDRESS + i, v);
  }
}

void setUnconfigured() {
   for(uint8_t i = 0; i < CONFIG_STATUS_SIZE; i++) {
       eeprom_write(EEPROM_CONFIG_STATUS_ADDRESS + i, 0x0);
  }
}

bool isConfigured()
{
    for (size_t i = 0; i < CONFIG_STATUS_SIZE; i++)
    {
        uint32_t v = (uint32_t)eeprom_read(EEPROM_CONFIG_STATUS_ADDRESS + i);
        if (v << (i * 8) != (CONFIG_STATUS_COMPLETE & (0xFF << (i * 8))))
        {
            return false;
        }
    }
    return true;
}
#endif
