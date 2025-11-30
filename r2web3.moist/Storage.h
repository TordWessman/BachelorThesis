#ifndef __STORAGE_H__
#define __STORAGE_H__

#include <vector>
#include <r2web3.h>
#include "Err.h"
#include "RemoteLogger.h"

using namespace blockchain;
#define ERROR_DESCRIPTION_TYPE_CALL_CONTRACT 1 // contract call failed
#define ERROR_DESCRIPTION_TYPE_GET_RECEIPT 2   // receipt call failed
#define ERROR_DESCRIPTION_TYPE_GET_BLOCK 2     // block call failed
#define ERROR_DESCRIPTION_TYPE_NO_RECEIPT 3    // receipt call successfull, but got no receipt
#define ERROR_DESCRIPTION_TYPE_NO_BLOCK 4      // block call successfull, but got no block

struct RodPair {
  RodPair(uint8_t first, uint8_t second) : first(first), second(second) {}
  const uint8_t first;
  const uint8_t second;
};

class MoistureReader {
  public:
  
    MoistureReader(const std::vector<RodPair> ppp) : rodPorts(ppp) { }

    std::vector<uint16_t> Values() {

      std::vector<uint16_t> values(rodPorts.size());

      for(int i = 0; i < rodPorts.size(); i++) {
        values[i] = (uint16_t) Read(i);
      }
      return values;
    }

  
  uint16_t Read(size_t index) {

    uint16_t result = (uint16_t)(readOneRod(rodPorts[index].first, rodPorts[index].second) +
           readOneRod(rodPorts[index].second, rodPorts[index].first)) / 2;
    delay(200);
    return result;
  }

  private:
  
  uint32_t readOneRod(uint8_t inputPort, uint8_t outputPort) {

    //Serial.print("Reading input port: "); Serial.print(inputPort); Serial.print(" Output port: "); Serial.println(outputPort);
    pinMode(inputPort, INPUT);
    pinMode(outputPort, OUTPUT);
    digitalWrite(outputPort, HIGH); 
    delay(1);
    uint32_t val = analogRead(inputPort);
    //Serial.print("Got value: "); Serial.println(val);
    digitalWrite(outputPort, LOW);
    return val;
  }
  
  std::vector<RodPair> rodPorts;
  
};

typedef enum AddEntryResult {

  AddEntrySuccess = 65534,
  AddEntryFailureOther = 65533,
  AddEntryFailureReceipt = 65532
};

class MoistureStorage {
public:
  MoistureStorage(Account *account, Chain *chain, const char *contractAddress, int receiptDelay, RemoteLogger *logger) : account(account), chain(chain), address(Address(contractAddress)), receiptDelay(receiptDelay), logger(logger) {

    std::vector<RodPair> rodPairs = {
      RodPair(25, 26),
      RodPair(27, 14),
      RodPair(12, 13)
    };
    reader = new MoistureReader(rodPairs);
  }

// TODO: Add verification that the transaction has been processed.

  bool Init() {
    Result<BigNumber> gasPriceResult = chain->GetGasPrice();
    if(gasPriceResult.HasValue()) {
      gasPrice = gasPriceResult.Value();
      return true;
    }
    return false;
  }

  int AddEntries()
  {
    std::vector<uint16_t> values = reader->Values();
    values.push_back(0); // temperature
    values.push_back(0); // humidity
    Result<TransactionTime> result = AddEntry(values);
    if (result.HasValue()) {
     return AddEntrySuccess; 
    } else {
      return AddEntryFailureOther;
    }
  }

  Result<TransactionTime> AddEntry(std::vector<uint16_t> values) {
    TransactionTime rtime;
    // Fill TransactionConfiguration if needed
    // time.configuration = ...;
    rtime.receipt = nullptr;
    rtime.transactionHash = nullptr;

    rtime.sendTime = ESPNetwork::GetUTCTimestamp();
    Serial.print("Send time:"); Serial.println(rtime.sendTime);
    BigNumber mergedValues(values);
    ContractCall call("storeValue", {ENC(&mergedValues)});
    BigNumber sendAmount = BigNumber((uint32_t)0u);
    Result<TransactionResponse> transactionResult = chain->Send(account, address, sendAmount, 6721975, &gasPrice, &call);
    rtime.transationDuration = ESPNetwork::GetUTCTimestamp() - rtime.sendTime;

    delay(receiptDelay);

    if (!transactionResult.HasValue()) {
      logger->logError(rtime, ERROR_DESCRIPTION_TYPE_CALL_CONTRACT, transactionResult, chain->Id());
      return Result<TransactionTime>::Err(transactionResult);
    }

    TransactionResponse transactionResponse = transactionResult.Value();
    rtime.transactionHash = transactionResponse.Result();

    Result<TransactionReceipt *> receiptResult = chain->GetTransactionReceipt(transactionResponse.Result());
    if (!receiptResult.HasValue()) {
      logger->logError(rtime, ERROR_DESCRIPTION_TYPE_GET_RECEIPT, transactionResult, chain->Id());
      return Result<TransactionTime>::Err(receiptResult);
    }

    if (receiptResult.Value() == nullptr) {
      Result<TransactionTime> error = Result<TransactionTime>::Err(ERROR_DESCRIPTION_TYPE_NO_RECEIPT, "No receipt");
      logger->logError(rtime, ERROR_DESCRIPTION_TYPE_NO_RECEIPT, error, chain->Id());
      return error;
    }
    rtime.receipt = receiptResult.Value();

    Result<BlockInformation *> blockResult = chain->GetBlockInformation(receiptResult.Value()->blockHash);
    if (!blockResult.HasValue())
    {
      logger->logError(rtime, ERROR_DESCRIPTION_TYPE_GET_BLOCK, blockResult, chain->Id());
      delete rtime.receipt;
      return Result<TransactionTime>::Err(blockResult);
    }

    if (blockResult.Value() == nullptr)
    {
      Result<TransactionTime> error = Result<TransactionTime>::Err(ERROR_DESCRIPTION_TYPE_NO_BLOCK, "No block");
      logger->logError(rtime, ERROR_DESCRIPTION_TYPE_NO_BLOCK, error, chain->Id());
      delete rtime.receipt;
      delete blockResult.Value();
      return error;
    }
    uint32_t confirmationTime = ((long)blockResult.Value()->timestamp - (long)rtime.sendTime) < 0 ? 0 : (uint32_t)((long)blockResult.Value()->timestamp - (long)rtime.sendTime);
    rtime.confirmationDuration = confirmationTime;
    Serial.print("Confirmation time: "); Serial.println(confirmationTime);
    logger->logTransaction(rtime, chain->Id());

    // Clean up if needed
    delete rtime.receipt;
    delete blockResult.Value();

    return Result<TransactionTime>(rtime);
  }
private:
  int   receiptDelay; 
  const Account *account;
  Chain *chain;
  const Address address;
  long lastTransmissionTime;
  MoistureReader *reader;
  RemoteLogger *logger;
  BigNumber gasPrice;
};
#endif
