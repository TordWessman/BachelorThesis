//#define USE_MATIC_TESTNET
//#define USE_IOTA_SHIMMER_TESTNET
//#define USE_AVALANCHE
//#define USE_GANACHE
//#define USE_MATIC
//#define USE_IOTEX
//#define USE_ETH
//#define USE_IMMUTABLE_X
#define USE_IOTA_SHIMMER
//#define USE_PRODUCTION

#include <r2web3.h>

#ifdef ESP8266
#include <ESP8266WiFi.h>
#endif
#ifdef ESP32
#include <WiFi.h>
#include "esp_32_bt.h"
#include "Io.h"

#endif

#include "Storage.h"
#include "RemoteLogger.h"
#include "Secrets.h"
#include "ChainConfig.h"
#include "config.h"
#include "Err.h"

    std::vector<RodPair> rodPairs = {
      RodPair(25, 26),
      RodPair(27, 14),
      RodPair(12, 13),
      RodPair(15, 2)
    };
MoistureReader moistureReader(rodPairs);
MoistureStorage *storage;
RemoteLogger *remoteLogger;

using namespace blockchain;

#ifdef ESP32
void restartDevice() { ESP.restart(); }
#endif
#ifdef ESP8266
void(* restartDevice) (void) = 0;
#endif

const char*pk = SECRET_PRIVATE_KEY;
//const char*pk = SECRET_PRIVATE_KEY_FALLBACK;
Account *account = NULL;
bool readyToRumble = false;

#ifdef USE_PRODUCTION
  ESPNetwork networkFacade(cert, true);
  Chain chain(&networkFacade);
#endif

#define EEPROM_WRITE_TIME 10 // number of reads required before a write is performed. Will also act as a denomiator for the actual count.
#define EEPROM_SUCCESS_ADDRESS 4
#define EEPROM_FAIL_TRANSACTION_ADDRESS 6
#define EEPROM_FAIL_OTHER_ADDRESS 8
uint16_t cSuccess = 0;
uint16_t cFailTransaction = 0;
uint16_t cFailOther = 0;

#define TRANSACTION_CHECK_DELAY 20*1000 // Time to wait for receipt

//#define SLEEP_SECONDS 10*60*1000-TRANSACTION_CHECK_DELAY   // delay before each log attempt

//#define HYBERNATION_TIME 4*60*60
#define HYBERNATION_TIME 4*60*60
#define ____HYBERNATION_TIME_US HYBERNATION_TIME*1000*1000

AppConfig appConfig;

int connectToWifi() {
    WiFi.mode(WIFI_STA);
    Serial.print("SSID: "); Serial.println(appConfig.ssid);
    Serial.print("PWD: "); Serial.println(appConfig.password);
    WiFi.begin(appConfig.ssid, appConfig.password);
  int connectionAttempts = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Log::m(".");
    delay(500);
    if(connectionAttempts++ > 10) { return err("Unable to connect to WiFi", ERROR_CODE_WIFI); }
  }

  Log::m("Connected, IP address: ");
  #ifndef R2WEB3_LOGGING_DISABLED
    Serial.println(WiFi.localIP());
  #endif
  return 0;
}

int startBlockchain() {

#ifdef USE_PRODUCTION
  
  if (bleHub != NULL) {
    Log::m("Can't use network. BLE is allocated. FIX THIS! Rebooting");
    restartDevice();
  }
#endif
  Log::m("Starting chain. Free heap: ", ESP.getFreeHeap());
 
  if(chain.Start()) {
    Log::m("...done");
    Log::m("Free heap after success: ", ESP.getFreeHeap());;
  } else {
    return err("Failed to start chain", ERROR_CODE_FAILED_CHAIN);
  }

  while (!storage->Init()) {
    return err("Unable to fetch gasPrice.", ERROR_CODE_FAILED_GAS_PRICE);
  }
  return 0;
}

#ifdef USE_PRODUCTION
void transactionCompletedDelegate() {
  Log::m("All done. Restarting device");
  restartDevice();
}


int networkConfigReceivedDelegate(AppConfig config) {
  appConfig = config;
  Log::m("networkConfigReceived called.");
  return connectToWifi();
}
#endif

uint16_t moist1 = 0;
uint16_t moist2 = 0;
uint16_t moist3 = 0;
uint16_t moist4 = 0;
uint16_t temp = 0;
uint16_t humid = 0;

void setup() {

#ifndef R2WEB3_LOGGING_DISABLED
  Serial.begin(9600);
#endif

  //return;
  Serial.println("I'm alive.");
  //AppConfig mamma = loadConfiguration();
  //Serial.print(mamma.ssid); Serial.print(" "); Serial.print(mamma.password); Serial.print(" "); Serial.print(mamma.chainURL); Serial.print(" "); Serial.print(mamma.contractAddress); Serial.print(" ");
  //Serial.println(loadPrivateKey() | byte_array :: hex_string);

#ifdef USE_PRODUCTION

  setUpIO();
while (true) {
  setPeripherals(true);
  temp = readTemperature();
  humid = readHumidity();
  setPeripherals(false);

  moist1 = moistureReader.Read(0);
  moist2 = moistureReader.Read(1);
  moist3 = moistureReader.Read(2);
  moist4 = moistureReader.Read(3);

  Log::m("Temp: ", temp); Log::m(" Humidity: ", humid);
  Log::m(" Moisture 1: ", moist1);
  Log::m(" Moisture 2: ", moist2);
  Log::m(" Moisture 3: ", moist3);
  Log::m(" Moisture 4: ", moist3);
delay(5000);
}
//restartDevice();

  setBootLED(true);
  eeprom_init();
  //setUnconfigured();
  if(shouldReset()) { 
    setUnconfigured(); 
    setBootLED(true);
    setTransferLED(true);
    delay(1000);
  }
  
  if (!isConfigured()) {
    setUpBT(networkConfigReceivedDelegate, transactionCompletedDelegate);
    return;
  } else {

    appConfig = loadConfiguration();
    account = new Account(loadPrivateKey()); 
    chain.SetRPCUrl(appConfig.chainURL);
  remoteLogger = new RemoteLogger(SECRET_REMOTE_LOGGER_HOST_PROD, SECRET_REMOTE_LOGGER_PORT);
    storage = new MoistureStorage(account, &chain, appConfig.contractAddress, TRANSACTION_CHECK_DELAY, remoteLogger);
  }

  eeprom_end();  
  
#else

  appConfig.ssid = SECRET_WIFI_SSID_DEV | char_string::copy;
  appConfig.password = SECRET_WIFI_PASSWORD_DEV | char_string::copy;
  account = new Account(pk);
  remoteLogger = new RemoteLogger(SECRET_REMOTE_LOGGER_HOST_DEV, SECRET_REMOTE_LOGGER_PORT);
  storage = new MoistureStorage(account, &chain, MOISTURE_CONTRACT_ADDRESS, TRANSACTION_CHECK_DELAY, remoteLogger);
#endif

  Log::m("My Address: ", account->GetAddress().AsString());
  char *hexString = account->GetPrivateKey() | byte_array::hex_string;
  Log::m("Private key:", hexString);
  delete []hexString; 
  int startResult = connectToWifi();
  if (startResult != 0) { restartDevice(); }
  startResult = startBlockchain();
  setBootLED(false);
  if (startResult == 0) {
    readyToRumble = true;
  } else {
    restartDevice();
  }
}

void sendToSleep(uint64_t sleepTime) {
 #ifdef USE_PRODUCTION
  #ifdef ESP_32
  setPeripherals(false);
  delay(500);
  esp_sleep_enable_timer_wakeup(sleepTime);
  
  delay(500);
  WiFi.disconnect();
  WiFi.mode(WIFI_OFF);
  //esp_bluedroid_disable();
  //esp_wifi_stop();
  delay(500);
  esp_deep_sleep_start();
  #else
  delay(60 * 60 * 1000);
  #endif
 #else
  delay(5 * 60 * 1000);
 #endif
}

void loop() {

  delay(1000); 
  if(readyToRumble == false) {
    return; 
  }

  
  Log::m("Free heap: ", ESP.getFreeHeap());

  //Serial.println("<------------------------ BALANCE BEFORE: ------------------------->");
  //chain->GetBalance(account.GetAddress());
  //Serial.println("</----------------------------------------------------------------->");
  setTransferLED(true);

  int result = storage->AddEntries();
  //int result = storage->AddEntry({(uint16_t)32, 0u, (uint16_t)64, (uint16_t)128});
  
  if (result != AddEntrySuccess) {
    Log::m("Unable to add entry: ", result);
    if (result == AddEntryFailureOther) {
      err("ERROR: Blockchain: Other", ERROR_CODE_BLOCKCHAIN_NETWORK);
      cFailOther++;
      if (cFailOther % EEPROM_WRITE_TIME == 0) {
        Log::m("Writing to EEPROM: ", cFailOther);
        eeprom_write(EEPROM_FAIL_OTHER_ADDRESS, cFailOther / EEPROM_WRITE_TIME);
      }
    } else if (result == AddEntryFailureReceipt) {
      err("ERROR: Blockchain", ERROR_CODE_BLOCKCHAIN_TRANSACTION);
      cFailTransaction++;
      if (cFailTransaction % EEPROM_WRITE_TIME == 0) {
        Log::m("Writing to EEPROM: ", cFailTransaction);
        eeprom_write(EEPROM_FAIL_TRANSACTION_ADDRESS, cFailTransaction / EEPROM_WRITE_TIME);
      }
    } else {
      err("ERROR: Blockchain", ERROR_CODE_BLOCKCHAIN_NETWORK);
      cFailTransaction++;
      if (cFailTransaction % EEPROM_WRITE_TIME == 0) {
        Log::m("Writing to EEPROM: ", cFailTransaction);
        eeprom_write(EEPROM_FAIL_TRANSACTION_ADDRESS, cFailTransaction / EEPROM_WRITE_TIME);
      }
    }

    Log::m("Will pause for a bit and hopefully recover.");
    setTransferLED(false);
    //sendToSleep(____HYBERNATION_TIME_US / 60);
    delay((HYBERNATION_TIME * 1000) / 60);
  } else {
    
    clearError();
    cSuccess++;
    if (cSuccess % EEPROM_WRITE_TIME == 0) {
      Log::m("Writing to EEPROM: ", cSuccess);
      eeprom_write(EEPROM_SUCCESS_ADDRESS, cSuccess / EEPROM_WRITE_TIME);
    }

    Log::m("Will hybernate now");
    setTransferLED(false);
    sendToSleep(____HYBERNATION_TIME_US);
    //delay(HYBERNATION_TIME * 1000);
  }

}
/*
*/
