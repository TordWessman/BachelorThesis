#ifndef __ERR_H__
#define __ERR_H__

#include "config.h"
#include "Io.h"

#define ERROR_CODE_NONE 0
#define ERROR_CODE_FAILED_CHAIN 1
#define ERROR_CODE_FAILED_GAS_PRICE 2
#define ERROR_CODE_WIFI 4
#define ERROR_CODE_WIFI_PASSWORD_TOO_LONG 8
#define ERROR_CODE_URL_TOO_LONG 16
#define ERROR_CODE_SSID_TOO_LONG 32
#define ERROR_CODE_BLOCKCHAIN_NETWORK 64
#define ERROR_CODE_BLOCKCHAIN_TRANSACTION 128

using ErrorDelegate = void (*)(const char*, int);

static ErrorDelegate errorDelegate = 0;

int err(const char *msg, int code) {

  #ifndef R2WEB3_LOGGING_DISABLED
  Serial.print("ERROR: "); Serial.print(msg); Serial.print(" code: "); Serial.println(code);
  #endif
  setErrorLED(true);
  writeError(code);
  if (errorDelegate) {
    errorDelegate(msg, code);
  }
  //TODO: Set error led & error codes
  delay(1000 * 10);
  setErrorLED(false);
  return code;
}

int errAssertLength(char *str, size_t maxSize, int code) {
  if (strlen(str) >= maxSize) {
    err("String to large", code);
    return code;
  }
  return 0;
}
#endif
