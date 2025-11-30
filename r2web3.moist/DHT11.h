#ifndef __DHT11_H__
#define __DHT11_H__

#ifdef ESP8266
  #include <DHTesp.h>
#else
  #include "Dht11.h"
#endif

class DHT11
{
public:
  DHT11(const int port)
  {

#ifdef ESP8266
  dht11 = new DHTesp();
  dht11->setup(port, DHTesp::AUTO_DETECT);
#else
  dht11 = new Dht11(port);
#endif  
  }
  ~DHT11() {
    free(dht11);
  }
  uint8_t Temperature() const { return temperature; }
  uint8_t Humidity() const { return humidity; }
  bool Read() {

#ifdef ESP8266
    delay(dht11->getMinimumSamplingPeriod());
    TempAndHumidity th = dht11->getTempAndHumidity();
        
      if (dht11->getStatus() != DHTesp::ERROR_NONE)
      {
          temperature = 0; humidity = 0;
          return false;
      } 
      else
      {
         temperature = th.temperature;
         humidity = th.humidity;
         return true;
      }
  #else
     if(dht11->read() == Dht11::OK)
     {
        temperature = dht11->getTemperature();
        humidity = dht11->getHumidity();
        return true;
     }
     else
     {
        temperature = 0; humidity = 0;
        return false;
     }
  #endif
  }
private:
  uint8_t temperature;
  uint8_t humidity;
#ifdef ESP8266
  DHTesp *dht11;
#else
  Dht11 *dht11;
#endif
};

#endif
