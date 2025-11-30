#ifndef __esp_32_bt_h__
#define __esp_32_bt_h__

#include <r2bt.h>
#include <r2web3.h>
#include "config.h"
#include "Err.h"

using namespace blockchain;

#define _BT_NAME "moisty"
#define _BT_ID "ccaabbbb-1fb5-459e-8fcc-c5c9c331914b"
using ConfigurationReceivedDelegate = int (*)(AppConfig);
using ConfigurationCompleteDelegate = void (*)();

#define BT_ADDRESS_ID 0x42
#define BT_CONFIGURATION_ID 0x43

void bt_setAddress(const char *address);
void bt_setError(const char *message, int code);
R2BTHub *bleHub = NULL;

void saveConfiguration(AppConfig configuration) {

   eeprom_write_string(EEPROM_SSID_ADDRESS, configuration.ssid);
   eeprom_write_string(EEPROM_WIFI_PASSWORD_ADDRESS, configuration.password);
   eeprom_write_string(EEPROM_CONTRACT_ADDRESS_ADDRESS, configuration.contractAddress);
   eeprom_write_string(EEPROM_URL_ADDRESS, configuration.chainURL);
}

AppConfig loadConfiguration() {

  return AppConfig(
    eeprom_read_string(EEPROM_SSID_ADDRESS), // ssid
    eeprom_read_string(EEPROM_WIFI_PASSWORD_ADDRESS), // password
    eeprom_read_string(EEPROM_CONTRACT_ADDRESS_ADDRESS), // contractAddress
    eeprom_read_string(EEPROM_URL_ADDRESS), // chainURL
    true); // isSet
}

class SendConfigurationContainer: public R2BTDataContainer {
public:
  SendConfigurationContainer(uint16_t id) : R2BTDataContainer(id, R2BTDataContainerAccessMode::R2BTEndpointAccessModeReadWrite) { }
  void OnUpdate(cJSON *message, R2BTError *error) override {
    Log::e("ERRORR ERRORRR ERRRORRR: BT value shoul not be possible to update!");
  }
};

void _free_configuration(AppConfig configuration) {
  if (configuration.ssid != NULL) { delete []configuration.ssid; configuration.ssid = NULL; }
  if (configuration.password != NULL) { delete []configuration.password; configuration.password = NULL; }
  if (configuration.contractAddress != NULL) { delete []configuration.contractAddress; configuration.contractAddress = NULL; }
  if (configuration.chainURL != NULL) { delete []configuration.chainURL; configuration.chainURL = NULL; }
}

class ReceiveConfigurationContainer: public R2BTDataContainer {
public:
  ReceiveConfigurationContainer(uint16_t id, ConfigurationReceivedDelegate configurationReceivedDelegate, ConfigurationCompleteDelegate configurationCompleteDelegate) : 
    R2BTDataContainer(id, R2BTDataContainerAccessMode::R2BTEndpointAccessModeReadWrite), configurationReceivedDelegate(configurationReceivedDelegate), configurationCompleteDelegate(configurationCompleteDelegate) { }
  
  void OnUpdate(cJSON *message, R2BTError *error) override {

    Log::m("Got config:");
    char *configString = cJSON_Print(message);
    Log::m(configString);
    free(configString);
    
    cJSON *configOk = cJSON_GetObjectItemCaseSensitive(message, "configDone");
    
    bt_setError("", ERROR_CODE_NONE); // clear bt error message
    
    if (configOk) {
      setConfigurationDone();
      configurationCompleteDelegate();
      
    } else {
      
      cJSON *networkConfigJSON = cJSON_GetObjectItemCaseSensitive(message, "network");
      
      AppConfig configuration(
        cJSON_GetObjectItemCaseSensitive(networkConfigJSON, "ssid")->valuestring | char_string::retain, // ssid
        cJSON_GetObjectItemCaseSensitive(networkConfigJSON, "password")->valuestring | char_string::retain, // password
        cJSON_GetObjectItemCaseSensitive(message, "contractAddress")->valuestring | char_string::retain, // contractAddress
        cJSON_GetObjectItemCaseSensitive(message, "chainURL")->valuestring | char_string::retain, // chainURL
        true); //isSet 
      
      int valueCheck = errAssertLength(configuration.ssid, EEPROM_SSID_SIZE, ERROR_CODE_SSID_TOO_LONG) |
                       errAssertLength(configuration.password, EEPROM_WIFI_PASSWORD_SIZE, ERROR_CODE_WIFI_PASSWORD_TOO_LONG) |
                       errAssertLength(configuration.chainURL, EEPROM_URL_SIZE, ERROR_CODE_URL_TOO_LONG);
      
      if ( valueCheck != 0) {
        _free_configuration(configuration);
      } else {
        
        int connectionResult = configurationReceivedDelegate(configuration);
        if (connectionResult == 0) {
          
          std::vector<uint8_t> pk = generatePrivateKey();
          setPrivateKey(pk);
          saveConfiguration(configuration);
          Account newAccount(pk);
          bt_setAddress(newAccount.GetAddress().AsString());
          
        } else {
          _free_configuration(configuration);
        }
        
      }
    }
  }
private:
  ConfigurationReceivedDelegate configurationReceivedDelegate;
  ConfigurationCompleteDelegate configurationCompleteDelegate;
};

SendConfigurationContainer *sendConfiguration;

void setUpBT(ConfigurationReceivedDelegate configurationReceivedDelegate, ConfigurationCompleteDelegate configurationCompleteDelegate) {

  Log::m("Not configured. Setting up BT. Free heap: ", ESP.getFreeHeap());
  bleHub = new R2BTHub(_BT_NAME, _BT_ID);

  errorDelegate = bt_setError;
  ReceiveConfigurationContainer *cc = new ReceiveConfigurationContainer(BT_CONFIGURATION_ID, configurationReceivedDelegate, configurationCompleteDelegate);
  bleHub->AddDataContainer(cc);

  sendConfiguration = new SendConfigurationContainer(BT_ADDRESS_ID);
  bleHub->AddDataContainer(sendConfiguration);
  
  bleHub->Start();

  if (hasError()) {
    bt_setError("Previous error", getError());
    clearError();
  }
}

void bt_setError(const char* message, int code) {
  cJSON *json = cJSON_CreateObject();
  cJSON *errorJson = cJSON_CreateObject();
  cJSON_AddNumberToObject(errorJson, "code", code);
  cJSON_AddStringToObject(errorJson, "message", message);
  cJSON_AddItemToObject(json,"error", errorJson);
  sendConfiguration->SetValue(json);
  cJSON_Delete(json);
}

void bt_setAddress(const char *address) {
  Log::m("ADDRESS to bt: ", address);
  
  cJSON *json = cJSON_CreateObject();
  cJSON_AddStringToObject(json, "walletAddress", address);
  sendConfiguration->SetValue(json);
  cJSON_Delete(json);
}

#endif
