/*
#include "RemoteLogger.h"
#include <Arduino.h>

static std::string toStringAndDelete(cJSON *json) {
  char *printed = cJSON_PrintUnformatted(json);
  std::string result = printed ? std::string(printed) : std::string();
  if (printed) free(printed);
  cJSON_Delete(json);
  return result;
}

std::string RemoteLogger::logTransaction(const TransactionTime &time, uint32_t chainId) {
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
  std::string s = toStringAndDelete(root);
  Serial.println(s.c_str());
  return s;
}

std::string RemoteLogger::logError(const TransactionTime &time, int errorStep, const ErrorDescription &errorDescription, uint32_t chainId) {
  cJSON *root = cJSON_CreateObject();
  cJSON_AddNumberToObject(root, "errorStep", errorStep);
  cJSON_AddNumberToObject(root, "chainId", chainId);
  cJSON_AddNumberToObject(root, "sendTime", (double)time.sendTime);
  cJSON_AddStringToObject(root, "description", errorDescription.ErrorMessage());
  cJSON_AddNumberToObject(root, "code", errorDescription.ErrorCode());
  cJSON_AddStringToObject(root, "transactionHash", time.transactionHash ? time.transactionHash : "");
  std::string s = toStringAndDelete(root);
  Serial.println(s.c_str());
  return s;
}
*/
