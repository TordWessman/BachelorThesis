#ifndef __REMOTE_LOGGER_H__
#define __REMOTE_LOGGER_H__

#include <string>
#include <r2web3.h>
#include <Arduino.h>
#include "Storage.h"

using namespace blockchain;

struct TransactionTime
{
    uint64_t sendTime;
    uint64_t transationDuration;
    uint64_t confirmationDuration;
    TransactionReceipt *receipt;
    char *transactionHash;
    // TransactionConfiguration configuration;
};

class RemoteLogger {
public:
  RemoteLogger(const char *ipAddress, uint16_t port) : ipAddress(ipAddress), port(port) {}

  void connect() {
    // Create a new network client each time (no cert, no debug)
    if(network) { delete network; }
    network = new ESPNetwork(nullptr, false);
    // Assume WiFi already connected externally; if HTTPS needed we would add cert.
  }

  void disconnect() {
    if(network) { delete network; network = nullptr; }
  }
  void logTransaction(const TransactionTime &time, uint32_t chainId) {
    connect();
    cJSON *root = cJSON_CreateObject();
    cJSON_AddNumberToObject(root, "chainId", chainId);
    cJSON_AddNumberToObject(root, "sendTime", (double)time.sendTime);
    cJSON_AddNumberToObject(root, "transactionDuration", (double)time.transationDuration);
    cJSON_AddNumberToObject(root, "confirmationDuration", (double)time.confirmationDuration);
    cJSON_AddStringToObject(root, "transactionHash", time.transactionHash ? time.transactionHash : "");
    if (time.receipt) {
      char *gasUsed = time.receipt->gasUsed.GenerateDecimalString();
      if (gasUsed) {
        cJSON_AddStringToObject(root, "gasUsed", gasUsed);
        delete[] gasUsed;
      }
    }
    char *printed = cJSON_PrintUnformatted(root);
    std::string result = printed ? std::string(printed) : std::string();
    if (printed) free(printed);
    cJSON_Delete(root);
    if(network) {
      std::string url = buildUrl("/success");
      HttpResponse resp = network->MakeRequest(url.c_str(), "POST", result.c_str());
      Serial.print("RemoteLogger /success code: "); Serial.println(resp.status);
    } else {
      Serial.println(result.c_str());
    }
    disconnect();
  }

  void logError(const TransactionTime &time, int errorStep, const ErrorDescription &errorDescription, uint32_t chainId) {
    connect();
    cJSON *root = cJSON_CreateObject();
    cJSON_AddNumberToObject(root, "errorStep", errorStep);
    cJSON_AddNumberToObject(root, "chainId", chainId);
    cJSON_AddNumberToObject(root, "sendTime", (double)time.sendTime);
    cJSON_AddStringToObject(root, "description", errorDescription.ErrorMessage());
    cJSON_AddNumberToObject(root, "code", errorDescription.ErrorCode());
    cJSON_AddStringToObject(root, "transactionHash", time.transactionHash ? time.transactionHash : "");
    char *printed = cJSON_PrintUnformatted(root);
    std::string result = printed ? std::string(printed) : std::string();
    if (printed) free(printed);
    cJSON_Delete(root);
    if(network) {
      std::string url = buildUrl("/failure");
      HttpResponse resp = network->MakeRequest(url.c_str(), "POST", result.c_str());
      Serial.print("RemoteLogger /failure code: "); Serial.println(resp.status);
    } else {
      Serial.println(result.c_str());
    }
    disconnect();
  }
private:
  const char *ipAddress;
  uint16_t port;
  ESPNetwork *network = nullptr;

  std::string buildUrl(const char *path) const {
    std::string url = "http://"; // assuming HTTP; adjust if HTTPS + cert
    url += ipAddress;
    url += ":";
    url += std::to_string(port);
    url += path;
    return url;
  }
};

#endif
